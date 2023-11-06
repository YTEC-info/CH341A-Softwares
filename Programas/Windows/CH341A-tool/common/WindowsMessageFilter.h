//---------------------------------------------------------------------------

#ifndef WindowsMessageFilterH
#define WindowsMessageFilterH
//---------------------------------------------------------------------------

#include <windows.h>

/// Adds the message to the filter. This has the effect of allowing the message to be received.
#define MSGFLT_ADD 1

/// Removes the message from the filter. This has the effect of blocking the message.
#define MSGFLT_REMOVE 2

BOOL ChangeWindowMessageFilter(UINT message, DWORD what);


#endif
