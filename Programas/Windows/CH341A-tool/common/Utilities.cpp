//---------------------------------------------------------------------------


#pragma hdrstop

#include "Utilities.h"
#include <stdio.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

//i.e. GetFileVer(Application->ExeName)
AnsiString GetFileVer(AnsiString FileName)
{
    AnsiString asVer="";
    VS_FIXEDFILEINFO *pVsInfo;
    unsigned int iFileInfoSize = sizeof( VS_FIXEDFILEINFO );
    
    int iVerInfoSize = GetFileVersionInfoSize(FileName.c_str(), NULL);
    if (iVerInfoSize!= 0)
    {
        char *pBuf = new char[iVerInfoSize];
        if (GetFileVersionInfo(FileName.c_str(),0, iVerInfoSize, pBuf ) )
        {
            if (VerQueryValue(pBuf, "\\",(void **)&pVsInfo,&iFileInfoSize))
            {
				asVer.sprintf("%d.%02d.%02d.%02d",
					HIWORD(pVsInfo->dwFileVersionMS), LOWORD(pVsInfo->dwFileVersionMS),
					HIWORD(pVsInfo->dwFileVersionLS), LOWORD(pVsInfo->dwFileVersionLS));
			}
        }
        delete [] pBuf;
    }
    return asVer;
}

void GetFileVer(AnsiString FileName, unsigned int &FileVersionMS, unsigned int &FileVersionLS)
{
	FileVersionMS = 0;
	FileVersionLS = 0;
    VS_FIXEDFILEINFO *pVsInfo;
    unsigned int iFileInfoSize = sizeof( VS_FIXEDFILEINFO );

    int iVerInfoSize = GetFileVersionInfoSize(FileName.c_str(), NULL);
    if (iVerInfoSize!= 0)
    {
        char *pBuf = new char[iVerInfoSize];
        if (GetFileVersionInfo(FileName.c_str(),0, iVerInfoSize, pBuf ) )
        {
            if (VerQueryValue(pBuf, "\\",(void **)&pVsInfo,&iFileInfoSize))
			{
				FileVersionMS = pVsInfo->dwFileVersionMS;
				FileVersionLS = pVsInfo->dwFileVersionLS;
			}
        }
        delete [] pBuf;
	}
}

char* urlencode(char* buf, size_t blen, const char* s)
{
    unsigned int i, j = 0;
    for (i=0; s[i]!='\0' && j+1<blen; ++i) {
        if ((s[i]>='A'&&s[i]<='Z') ||
            (s[i]>='a'&&s[i]<='z') ||
            (s[i]>='0'&&s[i]<='9') ||
            NULL!=strchr("-_.", s[i]))
            buf[j++] = s[i];
        else {
            if (j+4<=blen) {
                snprintf(buf+j, 4, "%%%02X", (unsigned char)s[i]);
                j += 3;
            }
            else
                break;
        }
    }
    buf[j] = '\0';
    return buf;
}

int GetFileWriteTime(AnsiString name, FILETIME *ftWrite)
{
	int status = -1;
	memset(ftWrite, 0, sizeof(*ftWrite));
	HANDLE hFile = CreateFile(name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	// Retrieve the file times for the file.
	if (hFile != INVALID_HANDLE_VALUE)
	{
		if (GetFileTime(hFile, NULL, NULL, ftWrite))
		{
			status = 0;
		}
		CloseHandle(hFile);
	}
	return status;
}

