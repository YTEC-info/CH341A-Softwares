//---------------------------------------------------------------------------


#pragma hdrstop

#include "Utils.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

AnsiString ExtractNumberFromUri(AnsiString uri)
{
	AnsiString res = "";
	int start = uri.Pos("sip:");
	if (start == 1)
		start += 4;
	int end = uri.Pos("@");
	if (end <= start)
		return "";
	res = uri.SubString(start, end-start);
	return res;
}

AnsiString CleanNumber(AnsiString asNumber)
{
	AnsiString nr = "";
	for(int i=1; i<=asNumber.Length(); i++)
	{
		if((asNumber[i] >= '0' && asNumber[i] <= '9') || asNumber[i]=='*' ||
			asNumber[i]=='#' || asNumber[i]=='+')
		{
			nr += asNumber[i];
		}
	}
	return nr;
}
