#ifndef TrayIconH
#define TrayIconH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <shellapi.h>

//---------------------------------------------------------------------------
class TrayIcon: public TComponent
{
private:
	bool bShowInTray;
	TIcon *Icon;
	AnsiString asHint;			
    TImageList *ImageList;
    int iImgIndex;
    HWND IconHandle;
    NOTIFYICONDATA IconData;
	TPopupMenu *PopupMenu;
	TTimer *Timer;
	UINT WM_TASKBARCREATE;

	bool __fastcall TrayMessage(DWORD dwMessage);

    void PopMenu();
    void __fastcall Animate(TObject *Sender);
	void __fastcall Notification(TComponent *AComponent, TOperation Operation);

protected:
    virtual void __fastcall NotifyMessage(TMessage &Msg);

public:
    __fastcall TrayIcon(TComponent* Owner);
    __fastcall virtual ~TrayIcon();

	void ShowInTray(bool bShow);
	void ModifyIcon(int iImageListIndex);
	void StartAnimate(unsigned int iInterval);
	void StopAnimate();
	void SetIcon(TImageList *imgList, int id);
	void SetIcon(TIcon *icon);
	void SetHint(AnsiString hint);
	inline void SetImageList(TImageList *imgList)
	{
		ImageList = imgList;
		if (ImageList != NULL)
			ImageList->FreeNotification(this);
	}
	inline void SetPopupMenu(TPopupMenu *popupMenu)
	{
		PopupMenu = popupMenu;
		if (PopupMenu != NULL)
			PopupMenu->FreeNotification(this);
	}

	TNotifyEvent OnLeftBtnDown;
	TNotifyEvent OnRightBtnDown;
	TNotifyEvent OnLeftBtnUp;
	TNotifyEvent OnRightBtnUp;
	TNotifyEvent OnLeftBtnDblClick;
};
//---------------------------------------------------------------------------
#endif

