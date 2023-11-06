#include <vcl.h>
#pragma hdrstop

#include "TrayIcon.h"
//#include "Log.h"
#pragma package(smart_init)

#define WM_TRAYICON WM_USER + 1

__fastcall TrayIcon::TrayIcon(TComponent* Owner)
        : TComponent(Owner)
{
	memset(&IconData, 0, sizeof(IconData));
	IconData.cbSize = sizeof(IconData);
	IconData.uVersion = NOTIFYICON_VERSION;	// NOTIFYICON_VERSION Use the Windows 2000 behavior.
											// Use this value for applications designed for Windows 2000 and later.

	IconHandle = AllocateHWnd(NotifyMessage);
	Timer = new TTimer(NULL);
	Timer->OnTimer = Animate;
	Icon = new TIcon();
	//get the message id which is registered by shell
	WM_TASKBARCREATE = RegisterWindowMessage("TaskbarCreated");
	iImgIndex = 0;
	bShowInTray = false;
}
//---------------------------------------------------------------------------

__fastcall TrayIcon::~TrayIcon()
{
	ImageList    = NULL;
	PopupMenu    = NULL;

	TrayMessage(NIM_DELETE);
	DeallocateHWnd(IconHandle);
	delete Timer;

	delete Icon;
	Icon         = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TrayIcon::NotifyMessage(TMessage &Msg)
{
    if (Msg.Msg == WM_CLOSE)
    {
        TrayMessage(NIM_DELETE);
    }
	else if (Msg.Msg == WM_TRAYICON)
    {
        switch (Msg.LParam)
        {
        case WM_LBUTTONDOWN:
			if (OnLeftBtnDown)
                OnLeftBtnDown(this);
			break;
		case WM_LBUTTONUP:
			if (OnLeftBtnUp)
				OnLeftBtnUp(this);
			break;
		case WM_RBUTTONDOWN:
			if (OnRightBtnDown)
				OnRightBtnDown(this);
			break;
		case WM_RBUTTONUP:
			if (OnRightBtnUp)
				OnRightBtnUp(this);
			PopMenu();
			break;
		case WM_LBUTTONDBLCLK:
			if (OnLeftBtnDblClick)
				OnLeftBtnDblClick(this);
			break;
		}
	}
	else if (Msg.Msg == WM_TASKBARCREATE)
	{
		if (bShowInTray)
		{
			TrayMessage(NIM_ADD);
			TrayMessage(NIM_SETVERSION);
		}
	}
	if (Msg.Result != 1)
		Msg.Result = DefWindowProc(IconHandle, Msg.Msg, Msg.WParam,Msg.LParam);
}
//---------------------------------------------------------------------------

void __fastcall TrayIcon::Notification(TComponent *AComponent, TOperation Operation)
{
	// We don't care about controls being added.
	if (Operation != opRemove)
		return;
	if (AComponent == ImageList)
		ImageList = NULL;
	else if (AComponent == PopupMenu)
    	PopupMenu = NULL;
	return;
}
//---------------------------------------------------------------------------

void TrayIcon::ModifyIcon(int iImageListIndex)
{
	ImageList->GetIcon(iImageListIndex, Icon);
    TrayMessage(NIM_MODIFY);
}
//---------------------------------------------------------------------------

void TrayIcon::StartAnimate(unsigned int iInterval)
{
	if (ImageList && !Timer->Enabled)
		Timer->Enabled = true;
	if (iInterval == 0)
		Timer->Interval = 1000;
	else
		Timer->Interval = iInterval;
}
//---------------------------------------------------------------------------

void TrayIcon::StopAnimate()
{
	Timer->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TrayIcon::Animate(TObject *Sender)
{
	if (ImageList && ImageList->Count)
	{
		if (iImgIndex < ImageList->Count - 1)
			iImgIndex++;
		else
			iImgIndex = 0;
		ModifyIcon(iImgIndex);
	}
}
//---------------------------------------------------------------------------

void TrayIcon::SetIcon(TImageList *imgList, int id)
{
	imgList->GetIcon(id, Icon);
	if (bShowInTray)
		TrayMessage(NIM_MODIFY);
}

void TrayIcon::SetIcon(TIcon *icon)
{
	Icon->Assign(icon);
	if (bShowInTray)
		TrayMessage(NIM_MODIFY);
}

void TrayIcon::SetHint(AnsiString hint)
{
	if (asHint != hint)
	{
		asHint = hint;
		if (bShowInTray)
			TrayMessage(NIM_MODIFY);
	}
}
//---------------------------------------------------------------------------

void TrayIcon::PopMenu()
{
    if (PopupMenu != NULL)
    {
        POINT MousePos;
        if (GetCursorPos(&MousePos))
        {
			SetForegroundWindow(IconHandle);
            PopupMenu->Popup(MousePos.x,MousePos.y);
            PostMessage(IconHandle, WM_NULL, 0,0);
        }
    }
}
//---------------------------------------------------------------------------

bool __fastcall TrayIcon::TrayMessage(DWORD dwMessage)
{
	//LOG("TrayIcon::TrayMessage _WIN32_IE = %08X\n", _WIN32_IE);
	assert(Icon);
    IconData.hIcon = Icon->Handle;
    IconData.hWnd = IconHandle;
    IconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	strncpy(IconData.szTip, asHint.c_str(), sizeof(IconData.szTip));
	IconData.szTip[sizeof(IconData.szTip)-1] = '\0';
    IconData.uCallbackMessage = WM_TRAYICON;
    return Shell_NotifyIcon(dwMessage, &IconData);
}
//---------------------------------------------------------------------------

void TrayIcon::ShowInTray(bool bShow)
{
	if (bShowInTray == bShow)
		return;
	if (bShow)
	{
		TrayMessage(NIM_ADD);
		TrayMessage(NIM_SETVERSION);		
	}
	else
	{
		TrayMessage(NIM_DELETE);
	}
	bShowInTray = bShow;
}
//---------------------------------------------------------------------------

