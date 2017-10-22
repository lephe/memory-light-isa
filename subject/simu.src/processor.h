#include "memory.h" 

class Processor {
 public:
	Processor(Memory* m);
	~Processor();
	void von_Neuman_step(bool debug);

 private:
	void read_bit_from_pc(int& var);
	void read_reg_from_pc(int& var);
	void read_const_from_pc(uint64_t& var);
	void read_addr_from_pc(uword& var);
	void read_shiftval_from_pc(int& var);
	void read_counter_from_pc(int& var);
	void read_size_from_pc(int& var);

	void read_cond_from_pc(int& var);
	bool cond_true(int cond);

	Memory *m;
	uword pc;
	uword sp;
	uword a1;
	uword a2;
	// The registers. Beware, they are defined as unsigned integers:
	// they should be cast to signed when needed
	uword r[8];

	// the flags
	bool zflag;
	bool cflag;
	bool nflag;
};
