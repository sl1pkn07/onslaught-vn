/*
* Copyright (c) 2008, 2009, Helios (helios.vmg@gmail.com)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, 
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of the author may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*     * Products derived from this software may not be called "ONSlaught" nor
*       may "ONSlaught" appear in their names without specific prior written
*       permission from the author. 
*
* THIS SOFTWARE IS PROVIDED BY HELIOS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL HELIOS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef NONS_UTF_H
#define NONS_UTF_H

#ifndef TOOLS_BARE_FILE
#include "Common.h"
#include "Functions.h"
#else
#include <fstream>
#include <cstring>

typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;
#endif
#include <string>
#include <algorithm>
#include <sstream>

#define STR_WHITESPACE "\x09\x0A\x0B\x0C\x0D\x20\x85\xA0"
#define WCS_WHITESPACE L"\x0009\x000A\x000B\x000C\x000D\x0020\x0085\x00A0\x1680\x180E\x2002\x2003\x2004\x2005\x2006\x2008\x2009\x200A\x200B\x200C\x200D\x205F\x3000"
#define WCS_NON_NEWLINE_WHITESPACE L"\x0009\x000B\x000C\x0020\x0085\x00A0\x1680\x180E\x2002\x2003\x2004\x2005\x2006\x2008\x2009\x200A\x200B\x200C\x200D\x205F\x3000"

#define BOM16B 0xFEFF
#define BOM16BA 0xFE
#define BOM16BB 0xFF
#define BOM16L 0xFFFE
#define BOM16LA BOM16BB
#define BOM16LB BOM16BA
#define BOM8A 0xEF
#define BOM8B 0xBB
#define BOM8C 0xBF
#define NONS_BIG_ENDIAN 0
#define NONS_LITTLE_ENDIAN 1
#define UNDEFINED_ENDIANNESS 2

inline bool NONS_isdigit(unsigned character){ return character>=0x0030 && character<=0x0039; }
inline bool NONS_isupper(unsigned character){ return character>=0x0041 && character<=0x005A; }
inline bool NONS_islower(unsigned character){ return character>=0x0061 && character<=0x007A; }
inline bool NONS_isalpha(unsigned character){ return character>=0x0041 && character<=0x005A || character>=0x0061 && character<=0x007A; }
inline bool NONS_isalnum(unsigned character){ return NONS_isalpha(character) || NONS_isdigit(character); }
inline unsigned NONS_toupper(unsigned character){ return NONS_islower(character)?character&223:character; }
inline unsigned NONS_tolower(unsigned character){ return NONS_isupper(character)?character|32:character; }
inline bool NONS_ishexa(unsigned character){ return NONS_isdigit(character) || NONS_toupper(character)>=0x0041 && NONS_toupper(character)<=0x0046; }
//1 if the character matches the regex [A-Za-z_] (the first character in a C-style identifier)
inline bool NONS_isid1char(unsigned character){ return NONS_isalpha(character) || character==0x005F; }
//1 if the character matches the regex [A-Za-z_0-9] (the second and beyond character in a C-style identifier)
inline bool NONS_isidnchar(unsigned character){ return NONS_isid1char(character) || NONS_isdigit(character); }

template <typename T1,typename T2>
int lexcmp(const T1 *a,const T2 *b){
	for (;*a || *b;a++,b++){
		unsigned c=*a,
		d=*b;
		if (c<d)
			return -1;
		if (c>d)
			return 1;
	}
	return 0;
}

template <typename T1,typename T2>
int lexcmp_CI(const T1 *a,const T2 *b){
	for (;*a || *b;a++,b++){
		unsigned c=NONS_toupper(*a),
		d=NONS_toupper(*b);
		if (c<d)
			return -1;
		if (c>d)
			return 1;
	}
	return 0;
}

template <typename T1,typename T2>
int lexcmp_CI_bounded(const T1 *a,size_t sizeA,const T2 *b,size_t sizeB){
	for (size_t c=0;c<sizeA && c<sizeB;a++,b++,c++){
		unsigned d=NONS_toupper(*a),
		e=NONS_toupper(*b);
		if (d<e)
			return -1;
		if (d>e)
			return 1;
	}
	if (sizeA<sizeB)
		return -1;
	if (sizeA>sizeB)
		return 1;
	return 0;
}

std::wstring UniFromISO88591(const std::string &str);
std::wstring UniFromUTF8(const std::string &str);
/*
Important note: this procedure assumes that the text string is a valid UCS-2
string, so while it does take BOM into account, it doesn't compensate for
streams with an odd length, as all valid UCS-2 strings have an even length.
*/
std::wstring UniFromUCS2(const std::string &str,char end=UNDEFINED_ENDIANNESS);
std::wstring UniFromSJIS(const std::string &str);
std::string UniToISO88591(const std::wstring &str);
std::string UniToUTF8(const std::wstring &str,bool addBOM=0);
std::string UniToUCS2(const std::wstring &str,char end=UNDEFINED_ENDIANNESS);
std::string UniToSJIS(const std::wstring &str);
template <typename T>
inline void toupper(std::basic_string<T> &str){
	std::transform<
		std::basic_string<T>::iterator,
		std::basic_string<T>::iterator,
		unsigned(*)(unsigned)>(str.begin(),str.end(),str.begin(),NONS_toupper);
}
template <typename T>
inline void tolower(std::basic_string<T> &str){
	std::transform<
		std::basic_string<T>::iterator,
		std::basic_string<T>::iterator,
		unsigned(*)(unsigned)>(str.begin(),str.end(),str.begin(),NONS_tolower);
}
template <typename T>
inline std::basic_string<T> toupperCopy(std::basic_string<T> str){
	toupper(str);
	return str;
}
template <typename T>
inline std::basic_string<T> tolowerCopy(std::basic_string<T> str){
	tolower(str);
	return str;
}

inline std::ostream &operator<<(std::ostream &stream,std::wstring &str){
	return stream <<UniToUTF8(str);
}

char checkEnd(wchar_t a);
//Determines the system's endianness.
char checkNativeEndianness();

bool isValidUTF8(const char *buffer,ulong size);
bool isValidSJIS(const char *buffer,ulong size);

bool iswhitespace(char character);
bool iswhitespace(wchar_t character);
bool iswhitespaceASCIIe(char character);
bool isbreakspace(char character);
bool isbreakspace(wchar_t character);
bool isbreakspaceASCIIe(char character);
void NONS_tolower(wchar_t *param);
void NONS_tolower(char *param);

template <typename T>
struct stdStringCmpCI{
	bool operator()(const std::basic_string<T> &s1,const std::basic_string<T> &s2) const{
		return lexcmp_CI_bounded(s1.c_str(),s1.size(),s2.c_str(),s2.size())<0;
	}
};

template <typename T1,typename T2>
int stdStrCmpCI(const std::basic_string<T1> &s1,const T2 *s2){
	std::basic_string<T2> temp=s2;
	return lexcmp_CI_bounded(s1.c_str(),s1.size(),temp.c_str(),temp.size());
}

template <typename T1,typename T2>
int stdStrCmpCI(const std::basic_string<T1> &s1,const std::basic_string<T2> &s2){
	return lexcmp_CI_bounded(s1.c_str(),s1.size(),s2.c_str(),s2.size());
}

//1 if the s1 begins with s2 at off
template <typename T>
bool firstcharsCI(const std::basic_string<T> &s1,size_t off,const std::basic_string<T> &s2){
	if (s1.size()-off<s2.size())
		return 0;
	for (ulong a=0;a<s2.size();a++)
		if (NONS_tolower(s1[off+a])!=NONS_tolower(s2[a]))
			return 0;
	return 1;
}

template <typename T>
bool firstcharsCI(const std::basic_string<T> &s1,size_t off,const T *s2){
	ulong l=0;
	while (s2[l])
		l++;
	if (s1.size()-off<l)
		return 0;
	for (ulong a=0;a<l;a++)
		if (NONS_tolower(s1[off+a])!=NONS_tolower(s2[a]))
			return 0;
	return 1;
}

template <typename T>
void trim_string(std::basic_string<T> &str){
	ulong first=0,
		second,
		size=str.size();
	for (;first<size && iswhitespace(str[first]);first++);
	if (first==size){
		str.clear();
		return;
	}
	second=first;
	for (;second<size && !iswhitespace(str[second]);second++);
	str=str.substr(first,second-first);
}
#endif
