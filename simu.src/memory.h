#ifndef memory_hpp
#define memory_hpp
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include <cstdlib>
#include <stdint.h>
#include <stdio.h>

#include "types.h"

const uint64_t MEMSIZE=1<<24; // please keep this a multiple of 64
const int PC=0;
const int SP=1;
const int A0=2;
const int A1=3;

class Memory {
 public:

	/** ctr should be one of PC, SP, A0, A1 */
	int read_bit(int ctr);

	/** ctr should be one of PC, SP, A0, A1 */
	void write_bit(int ctr, int bit);

	/** ctr should be one of PC, SP, A0, A1 */
	void set_counter(int ctr, uword val);

	/** method called to initialize the memory */
	void fill_with_obj_file(std::string filename);

	Memory();
	~Memory();
	
	// should be private but I am too lazy
	int counter[4]; 
	uint64_t m[MEMSIZE/64];

};


#endif
