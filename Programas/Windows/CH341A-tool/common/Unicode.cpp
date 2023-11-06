//---------------------------------------------------------------------------


#pragma hdrstop

#include "Unicode.h"
#include <windows.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

static wchar_t *CodePageToUnicode(int codePage, const char *src)
{
    if (!src) return 0;
    int srcLen = strlen(src);
    if (!srcLen)
    {
        wchar_t *w = new wchar_t[1];
        w[0] = 0;
        return w;
    }

	int requiredSize = MultiByteToWideChar(codePage, 0, src, srcLen, 0, 0);
	if (!requiredSize)
	{
		return 0;
	}

	wchar_t *w = new wchar_t[requiredSize+1];
	w[requiredSize] = 0;

	int retval = MultiByteToWideChar(codePage, 0, src,srcLen,w,requiredSize);
	if (!retval)
	{
		delete [] w;
		return 0;
	}

	return w;
}

static char *UnicodeToCodePage(int codePage, const wchar_t *src)
{
	if (!src) return 0;
	int srcLen = wcslen(src);
	if (!srcLen)
	{
		char *x = new char[1];
		x[0] = '\0';
		return x;
	}

	int requiredSize = WideCharToMultiByte(codePage, 0, src, srcLen, 0, 0, 0, 0);
	if (!requiredSize)
	{
		return 0;
	}

	char *x = new char[requiredSize+1];
	x[requiredSize] = 0;

	int retval = WideCharToMultiByte(codePage, 0, src, srcLen, x, requiredSize, 0, 0);
    if (!retval)
    {
        delete [] x;
        return 0;
    }

    return x;
}


AnsiString Utf8ToAnsi(AnsiString utf8)
{
	AnsiString ret;
    // 65001 = utf-8
	wchar_t *wText = CodePageToUnicode(65001, utf8.c_str());
	if (wText)
	{
		char *out = UnicodeToCodePage(CP_ACP /* system page */, wText);
		if (out)
		{
			ret = out;
			delete[] out;
        }
		delete[] wText;
	}
	return ret;
}

