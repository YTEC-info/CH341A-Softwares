//---------------------------------------------------------------------------

#ifndef UtilitiesH
#define UtilitiesH
//---------------------------------------------------------------------------

#include <System.hpp>

/** Get version (windows/resource based) of specified file
*/
AnsiString GetFileVer(AnsiString FileName);

void GetFileVer(AnsiString FileName, unsigned int &FileVersionMS, unsigned int &FileVersionLS);

int GetFileWriteTime(AnsiString name, FILETIME *ftWrite);

char* urlencode(char* buf, size_t blen, const char* s);


#endif
