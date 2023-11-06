//---------------------------------------------------------------------------


#pragma hdrstop

#include "OS.h"
#include <windows.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

bool IsWin7OrLater(void) {
	static bool once = false;
	static bool res = false;

	if (once)
		return res;
#if 0
	DWORD version = GetVersion();
    DWORD major = (DWORD) (LOBYTE(LOWORD(version)));
    DWORD minor = (DWORD) (HIBYTE(LOWORD(version)));

	res = (major > 6) || ((major == 6) && (minor >= 1));
#endif
#if 0
    OSVERSIONINFO osvi;
    BOOL bIsWindowsXPorLater;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);
	res = (osvi.dwMajorVersion > 6) || ((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion >= 1));
#endif
#if 1
	OSVERSIONINFOEX ver;
	DWORDLONG condMask = 0;
	ZeroMemory(&ver, sizeof(OSVERSIONINFOEX));
	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	ver.dwMajorVersion = 6;
	ver.dwMinorVersion = 1;
	VER_SET_CONDITION(condMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(condMask, VER_MINORVERSION, VER_GREATER_EQUAL);
	res = VerifyVersionInfo(&ver, VER_MAJORVERSION | VER_MINORVERSION, condMask);
#endif
	once = true;
	return res;
}

bool IsWinVistaOrLater(void) {
	static bool once = false;
	static bool res = false;

	if (once)
		return res;

	OSVERSIONINFOEX ver;
	DWORDLONG condMask = 0;
	ZeroMemory(&ver, sizeof(OSVERSIONINFOEX));
	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	ver.dwMajorVersion = 6;
	ver.dwMinorVersion = 0;
	VER_SET_CONDITION(condMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(condMask, VER_MINORVERSION, VER_GREATER_EQUAL);
	res = VerifyVersionInfo(&ver, VER_MAJORVERSION | VER_MINORVERSION, condMask);

	once = true;
	return res;
}