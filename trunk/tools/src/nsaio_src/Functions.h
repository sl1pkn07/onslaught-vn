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

#ifndef NONS_FUNCTIONS_H
#define NONS_FUNCTIONS_H

#include <vector>
#include <string>
#include <ctime>
#include <boost/filesystem/operations.hpp>

#include "Common.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define _HANDLE_POSSIBLE_ERRORS(x,extra) {ErrorCode possible_error=(x);if (possible_error!=NONS_NO_ERROR){extra return possible_error;}}
#define _CHECK_VARIABLE_NAME(pointertostring)\
{\
	if (!isalpha(*(pointertostring)) && *(pointertostring)!='_')\
		return NONS_INVALID_VARIABLE_NAME;\
	for (wchar_t *a=(pointertostring)+1;*a;a++)\
		if (!isalnum(*a) && *a!='_')\
			return NONS_INVALID_VARIABLE_NAME;\
}

//string functions
wchar_t *copyWString(const wchar_t *str,ulong len=0);
wchar_t *copyWString(const char *str,ulong len=0);
char *copyString(const wchar_t *str,ulong len=0);
char *copyString(const char *str,ulong len=0);
char *addStrings(const char *str1,char *str2);
wchar_t *addStrings(const wchar_t *str1,const wchar_t *str2);
char *addStrings(const char *str1,const char *str2);
void addStringsInplace(wchar_t **str1,const wchar_t *str2);
void addStringsInplace(wchar_t **str1,const char *str2);
void addStringsInplace(char **str1,const wchar_t *str2);
void addStringsInplace(char **str1,const char *str2);
long instr(const wchar_t *str0,const wchar_t *str1);
long instr(const wchar_t *str0,const char *str1);
long instr(const char *str0,const wchar_t *str1);
long instr(const char *str0,const char *str1);
long instrB(const wchar_t *str0,const wchar_t *str1);
long instrB(const char *str0,const char *str1);
void tolower(wchar_t *param);
void tolower(char *param);
bool multicomparison(char character,const char *characters);
bool multicomparison(wchar_t character,const char *characters);
bool multicomparison(char character,const wchar_t *characters);
bool multicomparison(wchar_t character,const wchar_t *characters);
void toforwardslash(wchar_t *param);
void toforwardslash(char *param);
void tobackslash(wchar_t *param);
void tobackslash(char *param);
bool isanumber(char *a);
bool isanumber(wchar_t *a);
std::vector<char *> *getParameterList(const char *string,char delim=' ');

template <typename T1,typename T2,typename T3>
void toforwardslash(std::basic_string<T1,T2,T3> &param){
	for (ulong a=0;a<param.size();a++)
		if (param[a]=='\\')
			param[a]='/';
}

//string parsing
wchar_t *tagName(const wchar_t *string);
char *tagName(const char *string);
wchar_t *tagValue(const wchar_t *string);
char *tagValue(const char *string);

//binary parsing functions
bool getbit(uchar *arr,ulong *byteoffset,uchar *bitoffset);
ulong getbits(uchar *arr,uchar bits,ulong *byteoffset,uchar *bitoffset);
void writeByte(integer8 a,std::string *str,long offset=-1);
void writeWord(integer16 a,std::string *str,long offset=-1);
void writeDWord(integer32 a,std::string *str,long offset=-1);
void writeString(wchar_t *a,std::string *str);
void writeWordBig(integer16 a,std::string *str,long offset=-1);
void writeDWordBig(integer32 a,std::string *str,long offset=-1);

/*
Compresses src[0..srcl-1].
Return value: allocated compressed buffer.
src: input buffer.
srcl: length of the input buffer in bytes.
dstl: the length of the compressed buffer will be written here.
*/
char *compressBuffer_BZ2(char *src,unsigned long srcl,unsigned long *dstl);
/*
Decompresses src[0..srcl-1].
Return value: allocated decompressed buffer.
src: input buffer.
srcl: length of the input buffer in bytes.
dstl: the length of the decompressed buffer will be written here.
*/
char *decompressBuffer_BZ2(char *src,unsigned long srcl,unsigned long *dstl);

//Boost wrappers
bool is_directory(const char *path);
bool is_directory(const wchar_t *path);
bool is_directory(const std::string &path);
bool is_directory(const std::wstring &path);
bool create_directory(const char *path);
bool create_directory(const wchar_t *path);
bool create_directory(const std::string &path);
bool create_directory(const std::wstring &path);
bool file_exists(const char *path);
bool file_exists(const wchar_t *path);
bool file_exists(const std::string &path);
bool file_exists(const std::wstring &path);
boost::uintmax_t file_size(const char *path);
boost::uintmax_t file_size(const wchar_t *path);
boost::uintmax_t file_size(const std::string &path);
boost::uintmax_t file_size(const std::wstring &path);
#endif
