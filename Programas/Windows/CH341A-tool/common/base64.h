#ifndef BASE64H
#define BASE64H

#ifdef __BORLANDC__
#  pragma pack(push, 8)
#endif
	#include <string>
    #include <iostream>
#ifdef __BORLANDC__
#  pragma pack(pop)
#endif

enum BASE64_ALPHABET {
	BASE64_ALPHABET_BASIC = 0,
	BASE64_ALPHABET_FSAFE	// file-safe
};

std::string base64_encode(unsigned char const* , unsigned int len, enum BASE64_ALPHABET alphabet = BASE64_ALPHABET_BASIC);
std::string base64_decode(std::string const& s, enum BASE64_ALPHABET alphabet = BASE64_ALPHABET_BASIC);

#endif // BASE64H
