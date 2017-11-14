//---
//	emu:graphical - manage the graphical screen
//---

#ifndef	GRAPHICAL_H
#define GRAPHICAL_H

#include <stddef.h>

/*
	graphical_start() - start the SDL thread for the screen
	Starts an SDL thread that will create a window and wait for events to
	be pushed on the event queue, refreshing the window when events arrive.
	The screen has 16-bit depth so the size of the video RAM should be
	2 * width * height (bytes).

	@arg	width	Screen width
	@arg	height	Screen height
	@arg	vram	Video memory source
	@returns	Zero on success, non-zero on error
*/
int graphical_start(size_t width, size_t height, void *vram);

/*
	graphical_refresh() - send a refresh signal to the SDL thread
	The thread will update the screen contents when it receives the signal.
	It is safe to call this function if the graphical interface is not
	running.
*/
void graphical_refresh(void);

/*
	graphical_wait() - wait for the SDL thread to stop and clean up
	This function waits until the SDL window is closed by the user. It then
	cleans up the graphical module.
*/
void graphical_wait(void);

/*
	graphical_stop() - stop the SDL thread
	Sends an EV_QUIT (user) event to the thread and waits for it t stop.
	Then cleans the graphical module and the SDL library.
*/
void graphical_stop(void);

#endif	/* GRAPHICAL_H */
