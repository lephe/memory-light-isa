#include "processor.h"
using namespace std;

Processor::Processor(Memory* m): m(m) {
	pc=0;
	sp=0;
	a1=0;
	a2=0;
	for (int i=0; i<7; i++)
		r[i]=0;
}

Processor::~Processor()
{}


void Processor::von_Neuman_step(bool debug) {
	// numbers read from the binary code
	int opcode=0;
	int regnum1=0;
	int regnum2=0;
	int shiftval=0;
	int condcode=0;
	int counter=0;
	int size=0;
	uword offset; 
	uint64_t constop=0; 
	int dir=0;
	// each instruction will use some of the following variables:
	// all unsigned, to be cast to signed when required.
	uword uop1;
	uword uop2;
	uword ur=0;
	doubleword fullr;
	bool manage_flags=false; // used to factor out the flag management code
	int instr_pc = pc; // for the debug output
	
	// read 4 bits.
	read_bit_from_pc(opcode);
	read_bit_from_pc(opcode);
	read_bit_from_pc(opcode);
	read_bit_from_pc(opcode);

	switch(opcode) {

	case 0x0: // add2
		read_reg_from_pc(regnum1);
		read_reg_from_pc(regnum2);
		uop1 = r[regnum1];
		uop2 = r[regnum2];
		fullr = ((doubleword) uop1) + ((doubleword) uop2); // for flags
		ur = uop1 + uop2;
		r[regnum1] = ur;
		manage_flags=true;
		break;

	case 0x1: // add2i
		read_reg_from_pc(regnum1);
		read_const_from_pc(constop);
		uop1 = r[regnum1];
		uop2 = constop; 
		fullr = ((doubleword) uop1) + ((doubleword) uop2); // for flags
		ur = uop1 + uop2;
		r[regnum1] = ur;
		manage_flags=true;
		break;

	case 0xa: // jump
		read_addr_from_pc(offset);
		pc += offset;
		m -> set_counter(PC, (uword)pc);
		manage_flags=false;		
		break;

		// begin sabote
		// end sabote
		
	case 0x8: // shift
		read_bit_from_pc(dir);
		read_reg_from_pc(regnum1);
		read_shiftval_from_pc(shiftval);
		uop1 = r[regnum1];
		if(dir==1){ // right shift
			ur = uop1 >> shiftval;
			cflag = ( ((uop1 >> (shiftval-1))&1) == 1);
		}
		else{
			cflag = ( ((uop1 << (shiftval-1)) & (1L<<(WORDSIZE-1))) != 0);
			ur = uop1 << shiftval;
		}
		r[regnum1] = ur;
		zflag = (ur==0);
		// no change to nflag
		manage_flags=false;		
		break;

		// begin sabote
		//end sabote

	case 0xc:
	case 0xd:
		//read two more bits
		read_bit_from_pc(opcode);
		read_bit_from_pc(opcode);
		switch(opcode) {
			
		case 0b110100: // write
			// begin sabote
			//end sabote
			break;
		}
		break; // Do not forget this break! 
		
	case 0xe:
	case 0xf:
		//read 3 more bits
		read_bit_from_pc(opcode);
		read_bit_from_pc(opcode);
		read_bit_from_pc(opcode);
		
		// begin sabote
		// end sabote
		break;
	}
	
	// flag management
	if(manage_flags) {
		zflag = (ur==0);
		cflag = (fullr > ((doubleword) 1)<<WORDSIZE);
		nflag = (0 > (sword) ur);
	}

	if (debug) {
		cout << "after instr: " << opcode 
				 << " at pc=" << hex << setw(8) << setfill('0') << instr_pc
				 << " (newpc=" << hex << setw(8) << setfill('0') << pc
				 << " mpc=" << hex << setw(8) << setfill('0') << m->counter[0] 
				 << " msp=" << hex << setw(8) << setfill('0') << m->counter[1] 
				 << " ma0=" << hex << setw(8) << setfill('0') << m->counter[2] 
				 << " ma1=" << hex << setw(8) << setfill('0') << m->counter[3] << ") ";
			//				 << " newpc=" << hex << setw(9) << setfill('0') << pc;
		cout << " zcn = " << (zflag?1:0) << (cflag?1:0) << (nflag?1:0);
		for (int i=0; i<8; i++)
			cout << " r"<< dec << i << "=" << hex << setw(8) << setfill('0') << r[i];
		cout << endl;
	}
}


// form now on, helper methods. Read and understand...

void Processor::read_bit_from_pc(int& var) {
	var = (var<<1) + m->read_bit(PC); // the read_bit updates the memory's PC
	pc++;                             // this updates the processor's PC
}

void Processor::read_reg_from_pc(int& var) {
	var=0;
	read_bit_from_pc(var);
	read_bit_from_pc(var);
	read_bit_from_pc(var);
}


//unsigned
void Processor::read_const_from_pc(uint64_t& var) {
	var=0;
	int header=0;
	int size;
	read_bit_from_pc(header);
	if(header==0)
		size=1;
	else  {
		read_bit_from_pc(header);
		if(header==2)
			size=8;
		else {
			read_bit_from_pc(header);
			if(header==6)
				size=32;
			else
				size=64;
		}
	}
	// Now we know the size and we can read all the bits of the constant.
	for(int i=0; i<size; i++) {
		var = (var<<1) + m->read_bit(PC);
		pc++;
	}		
}


// Beware, this one is sign-extended
void Processor::read_addr_from_pc(uword& var) {
	var=0;
	int header=0;
	int size;
	var=0;
	read_bit_from_pc(header);
	if(header==0)
		size=8;
	else  {
		read_bit_from_pc(header);
		if(header==2)
			size=16;
		else {
			read_bit_from_pc(header);
			if(header==6)
				size=32;
			else
				size=64;
		}
	}
	// Now we know the size and we can read all the bits of the constant.
	for(int i=0; i<size; i++) {
		var = (var<<1) + m->read_bit(PC);
		pc++;
	}
	// cerr << "before signext " << var << endl;
	// sign extension
	int sign=(var >> (size-1)) & 1;
	for (int i=size; i<WORDSIZE; i++)
		var += sign << i;
	// cerr << "after signext " << var << " " << (int)var << endl;

}




void Processor::read_shiftval_from_pc(int& var) {
	// begin sabote
	//end sabote
}

void Processor::read_cond_from_pc(int& var) {
	var =0;
	read_bit_from_pc(var);
	read_bit_from_pc(var);
	read_bit_from_pc(var);
}


bool Processor::cond_true(int cond) {
	switch(cond) {
	case 0 :
		return (zflag);
		break;
	case 1 :
		return (! zflag);
		break;
		// begin sabote
		// end sabote
	}
	throw "Unexpected condition code";
}


void Processor::read_counter_from_pc(int& var) {
	// begin sabote
	// end sabote
}


void Processor::read_size_from_pc(int& size) {
	// begin sabote
	// end sabote
}
