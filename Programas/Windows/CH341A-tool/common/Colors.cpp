//---------------------------------------------------------------------------


#pragma hdrstop

#include "Colors.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace {
    struct Entry {
		Color::Id id;
		AnsiString name;
		int color;
	} entries[Color::clLimiter] =
	{
		{ Color::clCustom, "<custom color>", 0 },

		{ Color::clAqua, "Aqua", clAqua },
		{ Color::clBlack, 	 "Black", clBlack },
		{ Color::clBlue, 	 "Blue", clBlue },
		{ Color::clCream, 	 "Cream", clCream },
		{ Color::clDkGray, 	 "Dark Grey", clDkGray },
		{ Color::clFuchsia, 	 "Fuchsia", clFuchsia },
		{ Color::clGray, 	 "Gray", clGray },
		{ Color::clGreen, 	 "Green", clGreen },
		{ Color::clLime, 	 "Lime Green", clLime },
		{ Color::clLtGray, 	 "Light Gray", clLtGray },
		{ Color::clMaroon, 	 "Maroon", clMaroon },
		{ Color::clMedGray, 	 "Medium Gray", clMedGray },
		{ Color::clMoneyGreen, 	 "Mint Green", clMoneyGreen },
		{ Color::clNavy, 	 "Navy Blue", clNavy },
		{ Color::clOlive, 	 "Olive Green", clOlive },
		{ Color::clPurple, 	 "Purple", clPurple },
		{ Color::clRed, 	 "Red", clRed },
		{ Color::clSilver, 	 "Silver", clSilver },
		{ Color::clSkyBlue, 	 "Sky Blue", clSkyBlue },
		{ Color::clTeal, 	 "Teal", clTeal },
		{ Color::clWhite, 	 "White", clWhite },
		{ Color::clYellow, 	 "Yellow", clYellow },

		{ Color::clActiveBorder, "clActiveBorder", clActiveBorder },
		{ Color::clActiveCaption, "clActiveCaption", clActiveCaption },
		{ Color::clAppWorkSpace, "clAppWorkSpace", clAppWorkSpace },
		{ Color::clBackground, "clBackground", clBackground },
		{ Color::clBtnFace, "clBtnFace", clBtnFace },
		{ Color::clBtnHighlight, "clBtnHighlight", clBtnHighlight },
		{ Color::clBtnShadow, "clBtnShadow", clBtnShadow },
		{ Color::clBtnText, "clBtnText", clBtnText },
		{ Color::clCaptionText, "clCaptionText", clCaptionText },
		//{ Color::clGradientActiveCaption, "clGradientActiveCaption", clGradientActiveCaption },
		//{ Color::clGradientInactiveCaption, "clGradientInactiveCaption", clGradientInactiveCaption },
		{ Color::clGrayText, "clGrayText", clGrayText },
		{ Color::clHighlight, "clHighlight", clHighlight },
		{ Color::clHighlightText, "clHighlightText", clHighlightText },
		//{ Color::clHotlight, "clHotlight", clHotlight },
		{ Color::clInactiveBorder, "clInactiveBorder", clInactiveBorder },
		{ Color::clInactiveCaption, "clInactiveCaption", clInactiveCaption },
		{ Color::clInactiveCaptionText, "clInactiveCaptionText", clInactiveCaptionText },
		//{ Color::clInfoBk, "clInfoBk", clInfoBk },
		//{ Color::clInfoText, "clInfoText", clInfoText },
		{ Color::clMenu, "clMenu", clMenu },
		{ Color::clMenuBar, "clMenuBar", clMenuBar },
		{ Color::clMenuHighlight, "clMenuHighlight", clMenuHighlight },
		{ Color::clMenuText, "clMenuText", clMenuText },
		//{ Color::clScrollbar, "clScrollbar", clScrollbar },
		//{ Color::cl3DDkShadow, "cl3DDkShadow", cl3DDkShadow },
		//{ Color::cl3DLight, "cl3DLight", cl3DLight },
		{ Color::clWindow, "clWindow", clWindow },
		{ Color::clWindowFrame, "clWindowFrame", clWindowFrame },
		{ Color::clWindowText, "clWindowText", clWindowText }
	};
}

AnsiString Color::IdToText(Id id)
{
	for (unsigned int i=0; i<sizeof(entries)/sizeof(entries[0]); i++)
	{
		Entry &entry = entries[i];
		if (entry.id == id)
		{
			return entry.name;
        }
	}
	return "???";
}

int Color::IdToIntTColor(Id id)
{
	for (unsigned int i=0; i<sizeof(entries)/sizeof(entries[0]); i++)
	{
		Entry &entry = entries[i];
		if (entry.id == id)
		{
			return entry.color;
        }
	}
	return 0;
}

int Color::IntTColorToId(int color)
{
	for (unsigned int i=0; i<sizeof(entries)/sizeof(entries[0]); i++)
	{
		Entry &entry = entries[i];
		if (entry.color == color)
		{
			return entry.id;
        }
	}
	return Color::clCustom;
}

