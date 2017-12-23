#include <graphical.h>
#include <errors.h>
#include <SDL2/SDL.h>

#include <unistd.h>
#include <signal.h>

/* Arguments to be passed to the thread */
struct thread_args
{
	size_t width, height;
	void *vram;
};

/* Main SDL thread and associated function */
SDL_Thread *thread = NULL;
static int thread_main(void *args);

/* Buffer for format conversion */
static void *buffer = NULL;

/* Timer handler */
static Uint32 timer_handler(Uint32 interval, void *arg);

/* User event number registered from SDL */
static Uint32 event_user;

/* Event numbers used by this application */
#define	EV_QUIT		0
#define	EV_REFRESH	1

/* More SDL objects owned by the main thread but used by the screen thread
   (Safety policy: all memory management is performed by the main thread) */
static SDL_Window *window	= NULL;
static SDL_Renderer *renderer	= NULL;
static SDL_Texture *texture	= NULL;
static SDL_TimerID timer	= 0;

/* Callback function and argument for use by the emulator */
static void (*callback)(const uint8_t *keyboard, void *arg) = NULL;
static void *callback_arg = NULL;



/*
	cleanup() - clean everything, even if graphical_start() failed
*/
static void cleanup(void)
{
	/* Init failed, that's really sad */
	if(!SDL_WasInit(SDL_INIT_EVERYTHING)) return;

	if(buffer) free(buffer);
	if(texture) SDL_DestroyTexture(texture);
	if(renderer) SDL_DestroyRenderer(renderer);
	if(window) SDL_DestroyWindow(window);
	if(timer) SDL_RemoveTimer(timer);

	SDL_Quit();
}

/*
	send_event() - send an order to the thread through the event system
*/
static void send_event(int type, void *data1, void *data2)
{
	SDL_Event e;
	SDL_memset(&e, 0, sizeof e);

	/* Use the special event ID we registered from SDL */
	e.type		= event_user;
	e.user.code	= type;
	e.user.data1	= data1;
	e.user.data2	= data2;

	SDL_PushEvent(&e);
}

/* graphical_start() - start the SDL thread for the screen */
int graphical_start(size_t width, size_t height, void *vram,
	void (*func)(const uint8_t *keyboard, void *funcarg), void *funcarg,
	int scale)
{
	/* Shorthand for error management */
	#define fail(format, ...) {			\
		error(format, ##__VA_ARGS__);		\
		cleanup();				\
		return 1;				\
	}

	static struct thread_args args;
	args.width = width;
	args.height = height;
	args.vram = vram;

	/* Initialize SDL to start the video, event and timer services */
	int status = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	if(status < 0)
		fail("cannot initialize SDL: %s", SDL_GetError());

	/* Register new events (and fail early if something occurs) */
	event_user = SDL_RegisterEvents(1);
	if(event_user == (Uint32)(-1))
		fail("cannot register user event: %s", SDL_GetError());

	/* Create a new window */
	window = SDL_CreateWindow("ASN1 2017: Program emulator",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scale * width,
		scale * height, 0);
	if(!window)
		fail("cannot create window: %s", SDL_GetError());

	/* Create a renderer for this window. Stick to good old software
	   rendering because others leak at lot of memory (20 MB) */
	renderer = SDL_CreateRenderer(window, -1,  SDL_RENDERER_SOFTWARE);
	if(!renderer)
		fail("cannot create renderer: %s", SDL_GetError());

	SDL_RenderSetScale(renderer, scale, scale);

	/* Create a texture where we will render our screen data. This format
	   will slow down the rendering (~400 FPS instead of ~1000 FPS) but it
	   will be enough for us */
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING, width, height);
	if(!texture)
		fail("cannot create texture: %s", SDL_GetError());

	/* Create a buffer where to perform format and endianness conversion */
	buffer = malloc(4 * args.width * args.height);
	if(!buffer)
		fail("cannot create a screen buffer");

	/* Apparently nothing needs to be done to free the thread object
	   returned by the library, so we can "safely" lose the pointer */
	thread = SDL_CreateThread(thread_main, "screen", &args);
	if(!thread)
		fail("cannot start the screen thread: %s", SDL_GetError());

	/* Start a timer to let the thread regularly update the texture without
	   sending it too much events */
#ifndef GRAPHICAL_ASYNC
	timer = SDL_AddTimer(20, timer_handler, NULL);
	if(!timer)
		fail("cannot start the timer: %s", SDL_GetError());
#endif

	/* Save the callback info */
	callback = func;
	callback_arg = funcarg;

	/* Time to let the thread run! */
	return 0;
}

/* graphical_refresh() - send a refresh signal to the SDL thread */
void graphical_refresh(void)
{
#ifdef GRAPHICAL_ASYNC
	/* Allow calling this function even if the thread was not started */
	if(!thread) return;

	/* Just send an event, the thread will eventually catch it */
	send_event(EV_REFRESH, NULL, NULL);
#endif
}

/* graphical_freeze() - stop the regular update */
void graphical_freeze(void)
{
	if(!thread) return;

	/* Send a final EV_REFRESH to make sure the latest changes to vram are
	   displayed */
	send_event(EV_REFRESH, NULL, NULL);

	/* Stop the timer, and make sure cleanup() does not remove it again */
	SDL_RemoveTimer(timer);
	timer = 0;
}

/* graphical_wait() - wait for the SDL thread to stop */
void graphical_wait(void)
{
	if(!thread) return;
	int status;

	SDL_WaitThread(thread, &status);
	thread = NULL;

	cleanup();
}

/* graphical_stop() - stop the SDL thread */
void graphical_stop(void)
{
	if(!thread) return;

	/* Send an EV_QUIT event to the queue (the thread will find it) */
	send_event(EV_QUIT, NULL, NULL);

	/* Wait for the thread to stop and clean up everything */
	graphical_wait();
}



//---
//	Timer handler (yet another thread)
//---

static Uint32 timer_handler(Uint32 interval, __attribute__((unused)) void *arg)
{
	/* Send a refresh event to the screen thread */
	send_event(EV_REFRESH, NULL, NULL);

	/* Call back the emulator, if requested */
	if(callback) callback(SDL_GetKeyboardState(NULL), callback_arg);

	return interval;
}



//---
//	Functions that run inside the thread
//---

/*
	thread_update() - update and render the screen contents

	@arg	args		Thread arguments
	@arg	texture		Target texture
	@arg	renderer	Renderer associated with the screen window
*/
void thread_update(struct thread_args *args, SDL_Texture *texture,
	SDL_Renderer *renderer)
{
	void *pixels;
	int pitch;

	/* Convert endianness and format */
	uint16_t *src = args->vram;
	uint32_t *dst = buffer;

	for(size_t n = 0; n < args->width * args->height; n++)
	{
		uint16_t px = be16toh(*src++);
		*dst++ = (px & 0xf800) << 8
		       | (px & 0x07e0) << 5
		       | (px & 0x001f) << 3;
	}

	SDL_LockTexture(texture, NULL, &pixels, &pitch);

	SDL_UpdateTexture(texture, NULL, buffer, 4 * args->width);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	SDL_UnlockTexture(texture);
}

/*
	thread_main() - run an SDL thread that regularly updates the screen

	@arg	args_void	Pointer to struct thread_args with arguments
	@returns		Zero on success, non-zero otherwise
*/
int thread_main(void *args_void)
{
	struct thread_args *args = args_void;

	/* Show the window when beginning */
	thread_update(args, texture, renderer);

	/* Enter a loop of waiting for events and redrawing the texture */
	SDL_Event e;

	while(1)
	{
		SDL_WaitEvent(&e);

		/* Handling program exits */
		if(e.type == SDL_QUIT) break;
		if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
			break;

		/* None other events are handled */
		if(e.type != event_user) continue;

		/* EV_QUIT: Quit the application */
		if(e.user.code == EV_QUIT) break;
		/* EV_REFRESH: Refresh the texture */
		if(e.user.code == EV_REFRESH)
		{
			thread_update(args, texture, renderer);
			continue;
		}
	}

	/* Hide the window. This is because the main thread will only destroy
	   the window when quitting SDL after emulation stops. If the user
	   closes the window early, we want it to disappear quickly */
	SDL_HideWindow(window);

	/* Let the main thread know that we've done all this */
	extern pid_t main_thread;
	kill(main_thread, SIGUSR2);

	/* Simply leave, the main thread will clean up everything */
	return 0;
}
