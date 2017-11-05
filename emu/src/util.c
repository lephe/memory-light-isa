#include <util.h>

/* sign_extend() -- sign-extend from a variable-width storage format
   For more info, see the mighty Bit Twiddling Hacks reference:
   http://graphics.stanford.edu/~seander/bithacks.html#VariableSignExtend */
int64_t sign_extend(uint64_t x, uint n)
{
	uint64_t m = 1ul << (n - 1);
	return (x ^ m) - m;
}
