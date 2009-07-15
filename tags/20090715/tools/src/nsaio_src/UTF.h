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

#include "Functions.h"

typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;

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

char checkEnd(wchar_t a);
//Determines the system's endianness.
bool checkNativeEndianness();
/*
Important note: this procedure assumes that the text string is a valid UCS-2
string, so while it does take BOM into account, it doesn't compensate for
streams with an odd length, as all valid UCS-2 strings have an even length.
If the string has an odd length, the last byte will be ignored.
*/
wchar_t *UCS2_to_WChar(const char *buffer,long initialSize,long *finalSize,uchar end=UNDEFINED_ENDIANNESS);
wchar_t *ISO88591_to_WChar(const char *buffer,long initialSize,long *finalSize);
wchar_t *UTF8_to_WChar(const char *buffer,long initialSize,long *finalSize);
wchar_t *UTF8_to_WChar(const char *string);
/*
Historical note: If I understood the code correctly, the old ONScripter perfomed
this conversion in real time in order to output characters to the screen
(FreeType, TrueType fonts, etc. index glyphs using Unicode). Why Ogapee decided
not to include Unicode support, despite the fact that he would have to do the
conversion anyway, is something that eludes me to this day.
*/
wchar_t *SJIS_to_WChar(const char *buffer,long initialSize,long *finalSize);
wchar_t *SJIS_to_WChar(const char *string);
//Determine if a byte is the first of a wide Shift JIS character.
bool isSJISWide(uchar a);
char *WChar_to_UCS2(const wchar_t *buffer,long initialSize,long *finalSize,uchar end=NONS_BIG_ENDIAN);
char *WChar_to_ISO88591(const wchar_t *buffer,long initialSize,long *finalSize);
char *WChar_to_UTF8(const wchar_t *buffer,long initialSize,long *finalSize);
char *WChar_to_UTF8(const wchar_t *string);
char *WChar_to_SJIS(const wchar_t *buffer,long initialSize,long *finalSize);
char *WChar_to_SJIS(const wchar_t *string);
long getUTF8size(const wchar_t *buffer,long size);
long getUTF8size(const wchar_t *string);

#define _READ_BINARY_SJIS_STRING(to,from,offset)\
{\
	char *tempString=readString((char*)(from),&(offset));\
	to =SJIS_to_WChar(tempString);\
	delete[] tempString;\
}
#define _READ_BINARY_UTF8_STRING(to,from,offset)\
{\
	char *tempString=readString((char*)(from),&(offset));\
	to=UTF8_to_WChar(tempString);\
	delete[] tempString;\
	if (!*to){\
		delete[] to;\
		to=0;\
	}\
}
bool iswhitespace(char character);
bool iswhitespace(wchar_t character);
bool iswhitespaceASCIIe(char character);
bool isbreakspace(char character);
bool isbreakspace(wchar_t character);
bool isbreakspaceASCIIe(char character);
#endif
