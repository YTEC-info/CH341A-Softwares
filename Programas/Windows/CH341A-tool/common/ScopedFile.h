/** \file
	Small utility class to close file out of scope
*/

#ifndef ScopedFileH
#define ScopedFileH

#include <stdio.h>

class ScopedFile {
private:
	FILE* fp;
public:
	explicit ScopedFile(FILE* fp): fp(fp) {
	}
	~ScopedFile(void) {
		if (fp)
			fclose(fp);
	}
	operator FILE*() {
		return fp;
	}
};

#endif