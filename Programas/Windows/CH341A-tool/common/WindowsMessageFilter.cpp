//---------------------------------------------------------------------------


#pragma hdrstop

#include "WindowsMessageFilter.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)


typedef BOOL (WINAPI *LPFN_CHANGEWINDOWMESSAGEFILTER) (UINT, DWORD);

LPFN_CHANGEWINDOWMESSAGEFILTER fnChangeWindowMessageFilter;
 BOOL ChangeWindowMessageFilter(UINT message, DWORD what)
 {

 fnChangeWindowMessageFilter =      (LPFN_CHANGEWINDOWMESSAGEFILTER)GetProcAddress(GetModuleHandle(TEXT("user32")),"ChangeWindowMessageFilter");

 if (NULL != fnChangeWindowMessageFilter)
  {
   return fnChangeWindowMessageFilter(message,what);
  }
  else return true;
 }