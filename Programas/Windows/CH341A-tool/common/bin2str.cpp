#pragma hdrstop

#include "bin2str.h"
#include <SysUtils.hpp>
#include <stdlib.h>
#include <stdio.h>


/**
 * int pow ( int, int )
 * ---------------------------------------------------------------
 * Calculates int 1 to the power of int 2.
 *
 * Example:
 * int MyInt1 = 10;
 * int MyInt2 = 2;
 * int MyResult = pow(MyInt1, MyInt2);
 * cout << MyResult << endl;
 *
 * ( will result in 100 , being printed )
 *
 * */
int pow(int n, int i)
{
    if (i == 0)
        return 1;
    else if (i == 1)
        return n;
    else
    {
        int partial = pow(n, i / 2);
        if (i % 2 == 0)
            return partial * partial;
        else
            return partial * partial * n;
    }
}



/**
 * int hexStringToInt ( string )
 * ---------------------------------------------------------------
 * Converts a string representing a hexadecimal value to a an
 * integer
 *
 * Example:
 * string HexString = "0A";
 * int MyInt = hexStringToInt(HexString);
 * cout << MyInt << endl;
 *
 * ( will result in 10 , being printed )
 *
 * */
int hexStringToInt (std::string hexString)
{
    unsigned int i;
    int charvalue;
    int returnvalue = 0;

    for (i = 0; i < hexString.length(); i++)
    {
        if ( hexString[i] >= 'a' && hexString[i] <= 'f' )
        {
            charvalue = (int) hexString[i] - 87;
        }
        if ( hexString[i] >= 'A' && hexString[i] <= 'F' )
        {
            charvalue = (int) hexString[i] - 55;
        }
        if ( hexString[i] >= '0' && hexString[i] <= '9' )
        {
            charvalue = (int) hexString[i] - 48;
        }
        returnvalue += charvalue * ( pow(16, (int)(hexString.length() - (i + 1)) ));
    }
    return returnvalue;
}



/**
 * string intToHexString ( int )
 * ---------------------------------------------------------------
 * Converts an integer to a string representing the hexadecimal value
 * of the integer. The result will be have a 0 prepended to make the
 * string length even.
 *
 * Example:
 * int MyInt = 10;
 * string MyHex = intToHexString(MyInt);
 * cout << MyHex << endl;
 *
 * ( will result in "0A" , being printed )
 *
 * */
std::string intToHexString (int integer)
{
    std::string hexString = "";
    /*
    for (int i=2*sizeof(int) - 1; i>=0; i--) {
    	if ( ( (integer >> i*4) & 0xF ) != 0 )  {
    		hexString += "0123456789ABCDEF"[((integer >> i*4) & 0xF)];
    		
    	}
    }
    if ( hexString.length() % 2 != 0 ){
    	hexString = "0" + hexString;
    }
    */
    //Changed Conversion from intToHex from above to below...
    char buf[3];
    sprintf(buf,"%X",integer);
    hexString = (std::string)buf;
    //Prepend a 0 when not even
    if ( hexString.length() % 2 != 0 )
    {
        hexString = "0" + hexString;
    }

    return hexString;
}



/**
 * int binStringToInt ( string )
 * ---------------------------------------------------------------
 * Converts a string representing a binary value to a an
 * integer
 *
 * Example:
 * string BinString = "01111";
 * int MyInt = binStringToInt(BinString);
 * cout << MyInt << endl;
 *
 * ( will result in 15 , being printed )
 *
 * */
int binStringToInt (std::string binString)
{

    unsigned int pos;
    int integerReturn = 0;
    int base = 2;

    for (pos = 0; pos < binString.length(); pos++)
    {
        if ( binString.substr(pos, 1) == "1" )
        {
            integerReturn += pow(base, (binString.length() - (pos + 1)) );
        }
    }
    return integerReturn;
}



/**
 * string intToBinString ( int )
 * ---------------------------------------------------------------
 * Converts an integer to a string representing the binary value
 * of the integer. padded to make the string a multiple of 8 characters long
 *
 * Example:
 * int MyInt = 16;
 * string MyBin = intToBinString(MyInt);
 * cout << MyBin << endl;
 *
 * ( will result in "00010000" , being printed )
 *
 * */
std::string intToBinString (int integer)
{
    std::string stringReturn;

    if (integer <= 0)
    {
        return "0";
    }

    while ( integer > 0 )
    {
        if ( integer % 2 == 1 )
        {
            stringReturn = "1" + stringReturn;
            --integer;
        }
        else
        {
            stringReturn = "0" + stringReturn;
        }
        integer = integer / 2;
    }
    //Changed 8 to 7 because the characters are in septets
    while ( stringReturn.length() % 7 != 0 )
    {
        stringReturn = "0" + stringReturn;
    }
    return stringReturn;
}



std::string HexStringToBuf(std::string in)
{
    if(in.size() % 2)
        return "";
    std::string out;
    for (unsigned int i=0; i<in.size(); )
    {
        out += (char)hexStringToInt(in.substr(i, 2));
        i += 2;
    }
    return out;
}

std::string BufToHexString(std::string in)
{
    char *buf = new char[in.size()*2 + 1];
    if (!buf)
        return "";
    buf[0] = 0;
    for (unsigned int i=0; i<in.size(); i++)
    {
        sprintf(buf+(i*2), "%02X", (unsigned char)(in[i]));
    }
    std::string tst = buf;
    delete [] buf;
    return tst;
}

int HexStringCleanToBuf(AnsiString text, AnsiString &msg, std::vector<uint8_t> &data)
{
	data.clear();
	msg = "";
	
	text = text.Trim().LowerCase();
	AnsiString text2 = StringReplace(text, "0x", "", TReplaceFlags() << rfReplaceAll);
	text2 = StringReplace(text2, "\n", "", TReplaceFlags() << rfReplaceAll);
	text2 = StringReplace(text2, "\r", "", TReplaceFlags() << rfReplaceAll);
	const char* allowed = "0123456789abcdef";

	text = "";
	for (int i=1; i<=text2.Length(); i++)
	{
		char c = text2[i];
		if (strchr(allowed, c))
		{
        	text += c;
		}
	}

	if (text.Length() % 2)
	{
		msg = "Number of hex characters in data is uneven!";
		return -1;
	}
	std::string str = HexStringToBuf(text.c_str());
	data.assign(str.begin(), str.end());
	return 0;
}

