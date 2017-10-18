#ifndef H_SCREEN
#define H_SCREEN

//constants about the environnement
const int WIDTH = 160;
const int HEIGHT = 128;
const int MEM_SCREEN_BEGIN = 0x10000;

#include <SDL2/SDL.h>
#include "memory.h"

/* this is the function that runs in the screen thread
 * m -> the simulator's memory
 * force_quit -> shared variable to detect if we must close the screen
 * refresh -> shared variable that instructs the thread to refresh the screen
 */
void simulate_screen(Memory* m, bool *refresh);

#endif
