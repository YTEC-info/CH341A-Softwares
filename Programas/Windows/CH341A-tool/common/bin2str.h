#ifndef bin2strH
#define bin2strH

#include <System.hpp>
#include <string>
#include <vector>
#include <stdint.h>

int hexStringToInt (std::string);
std::string intToHexString (int);
int binStringToInt (std::string);
std::string intToBinString (int);
std::string HexStringToBuf(std::string);
std::string BufToHexString(std::string);

/** \brief Clean string (removing "0x" and non-hex characters) and convert to binary buffer
*/
int HexStringCleanToBuf(AnsiString text, AnsiString &msg, std::vector<uint8_t> &data);

#endif

