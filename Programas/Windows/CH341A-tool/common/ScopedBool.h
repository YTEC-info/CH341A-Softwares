/** \file
    Small utility class to set bool variable in scope / clear out of scope
*/

#ifndef ScopedBoolH
#define ScopedBoolH

#include <assert.h>

class ScopedBool {
private:
	bool *var;
public:
	ScopedBool(bool *var): var(var) {
		assert(var);
		*var = true;
	}
	ScopedBool(volatile bool *var): var((bool*)var) {
		assert(var);
		*var = true;
	}
	~ScopedBool(void) {
        *var = false;
    }
};

#endif