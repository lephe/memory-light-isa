#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <algorithm>

#include <SDL2/SDL.h>
#include <stdint.h>
#include <thread>
#include <signal.h>

#include "screen.h"
#include "memory.h"
#include "processor.h"


bool force_quit = false;
bool refresh = true;
int verbose=1; 


// Option parsing stuff from Stack Overflow
// use like this:    char * filename = getCmdOption(argv, argv + argc, "-f");

char* getCmdOption(char ** begin, char ** end, const std::string & option) {
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

void usage() {
		std::cerr << "Usage: simu [options] file.obj \n options : -d for debug, -s for step by step, -g for graphical screen" << std::endl;
		exit(0);
}

int main(int argc, char* argv[]) {

	if(argc==1) {
		usage();
	}
	bool debug = cmdOptionExists(argv, argv+argc, "-d");
	bool step_by_step = cmdOptionExists(argv, argv+argc, "-s");
	bool graphical_output = cmdOptionExists(argv, argv+argc, "-g");
 
	std::string filename = argv[argc-1];
	std::ifstream f(filename.c_str());
  if(!f.good()) {
		std::cerr << "can't access obj file" << std::endl;
		usage();
	}

	Memory* m;
	Processor* p;
	std::thread* screen;
		
	m= new Memory();
	p = new Processor(m);

	m->fill_with_obj_file(filename);

	// create the screen
	if(graphical_output)
		screen=new std::thread(simulate_screen, m, &refresh);

	// The von Neuman cycle
	while(1+1==2) {
		p->von_Neuman_step(debug);
		
		if(step_by_step)
			getchar();
	};

	if(graphical_output)
		screen->join();

	return 0;
}
