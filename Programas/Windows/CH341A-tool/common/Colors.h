//---------------------------------------------------------------------------

#ifndef ColorsH
#define ColorsH
//---------------------------------------------------------------------------

#include <Graphics.hpp>

namespace Color
{
	enum Id
	{
		clCustom = 0,

		clAqua,
		clBlack,
		clBlue,
		clCream,
		clDkGray,
		clFuchsia,
		clGray,
		clGreen,
		clLime,
		clLtGray,
		clMaroon,
		clMedGray,
		clMoneyGreen,
		clNavy,
		clOlive,
		clPurple,
		clRed,
		clSilver,
		clSkyBlue,
		clTeal,
		clWhite,
		clYellow,

		clActiveBorder,
		clActiveCaption,
		clAppWorkSpace,
		clBackground,
		clBtnFace,
		clBtnHighlight,
		clBtnShadow,
		clBtnText,
		clCaptionText,
		//clGradientActiveCaption,		// win 98 or win 2000
		//clGradientInactiveCaption,
		clGrayText,
		clHighlight,
		clHighlightText,
		//clHotlight,
		clInactiveBorder,
		clInactiveCaption,
		clInactiveCaptionText,
		//clInfoBk,
		//clInfoText,
		clMenu,
		clMenuBar,
		clMenuHighlight,
		clMenuText,
		//clScrollbar,
		//cl3DDkShadow,
		//cl3DLight,
		clWindow,
		clWindowFrame,
		clWindowText,

		clLimiter
	};

	//Color(void):
	//	id(clBtnFace)
	//{}

	AnsiString IdToText(Id id);
	int IdToIntTColor(Id id);
	int IntTColorToId(int color);
};

#endif
