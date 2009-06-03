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

#ifndef NONS_FUNCTIONS_H
#define NONS_FUNCTIONS_H

#ifndef BARE_FILE
#include "Common.h"
#include "ErrorCodes.h"
#include <SDL/SDL.h>
#include <map>
#endif

#include <vector>
#include <string>
#include <algorithm>
#include <ctime>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define ABS(x) ((x)<0?-(x):(x))

#define _HANDLE_POSSIBLE_ERRORS(x,extra) {ErrorCode possible_error=(x);if (possible_error!=NONS_NO_ERROR){extra return possible_error;}}
#define NONS_NEWSURFACE(w,h,d) SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,(w),(h),(d),rmask,gmask,bmask,amask)
#define CHECK_FLAG(x,y) (((x)&(y))==(y))

//string functions
wchar_t *copyWString(const wchar_t *str,ulong len=0);
wchar_t *copyWString(const char *str,ulong len=0);
char *copyString(const wchar_t *str,ulong len=0);
char *copyString(const char *str,ulong len=0);
wchar_t *addStrings(const wchar_t *str1,const wchar_t *str2);
char *addStrings(const char *str1,const char *str2);
void addStringsInplace(wchar_t **str1,const wchar_t *str2);
void addStringsInplace(wchar_t **str1,const char *str2);
void addStringsInplace(char **str1,const wchar_t *str2);
void addStringsInplace(char **str1,const char *str2);
long instr(const wchar_t *str0,const wchar_t *str1,long max=-1);
long instr(const wchar_t *str0,const char *str1,long max=-1);
long instr(const char *str0,const wchar_t *str1,long max=-1);
long instr(const char *str0,const char *str1,long max=-1);
long instrB(const wchar_t *str0,const wchar_t *str1);
long instrB(const char *str0,const char *str1);
bool multicomparison(char character,const char *characters);
bool multicomparison(wchar_t character,const char *characters);
bool multicomparison(char character,const wchar_t *characters);
bool multicomparison(wchar_t character,const wchar_t *characters);
template <typename T>
void toforwardslash(std::basic_string<T> &s){
	for (std::basic_string<T>::iterator i=s.begin(),end=s.end();i!=end;i++)
		*i=(*i==0x5C)?0x2F:*i;
}

template <typename T>
std::vector<std::basic_string<T> > getParameterList(const std::basic_string<T> &string,bool leave_quotes,char delim=' '){
	std::vector<std::basic_string<T> > res;
	char tempDelim=delim;
	for (std::basic_string<T>::const_iterator i=string.begin();i!=string.end();){
		if (*i=='\"'){
			if (!leave_quotes)
				i++;
			delim='\"';
		}
		std::basic_string<T>::const_iterator end=i;
		if (delim!=tempDelim && leave_quotes)
			end++;
		for (;end!=string.end() && *end!=delim;end++);
		if (delim!=tempDelim && leave_quotes)
			end++;
		res.push_back(std::basic_string<T>(i,end));
		delim=tempDelim;
		for (i=end+1;i!=string.end() && *i==delim;i++);
	}
	return res;
}
/*bool filenames_are_equal(const wchar_t *str0,const wchar_t *str1);
bool filenames_are_equal(const char *str0,const char *str1);*/
template <typename T>
void trim_string(std::basic_string<T> &str){
	std::basic_string<T>::iterator start=str.begin();
	for (;start!=str.end() && iswhitespace(*start);start++);
	if (start==str.end()){
		str.clear();
		return;
	}
	std::basic_string<T>::iterator end=start;
	for (;end!=str.end() && !iswhitespace(*end);end++);
	str=std::wstring(start,end);
}
template <typename T>
bool isValidIdentifier(const std::basic_string<T> &str){
	if (str[0]!='_' && !NONS_isalpha(str[0]))
		return 0;
	const T *s=&str[1];
	for (ulong a=1;a<str.size();a++,s++)
		if (*s!='_' && !NONS_isalnum(*s))
			return 0;
	return 1;
}
std::wstring readline(std::wstring::const_iterator start,std::wstring::const_iterator end,std::wstring::const_iterator *out=0);


//string parsing
wchar_t *tagName(const wchar_t *string);
char *tagName(const char *string);
wchar_t *tagValue(const wchar_t *string);
char *tagValue(const char *string);

//binary parsing functions
bool getbit(uchar *arr,ulong *byteoffset,uchar *bitoffset);
ulong getbits(uchar *arr,uchar bits,ulong *byteoffset,uchar *bitoffset);
Uint8 readByte(char *buffer,ulong &offset);
Sint16 readSignedWord(char *buffer,ulong &offset);
Uint16 readWord(char *buffer,ulong &offset);
Sint32 readSignedDWord(char *buffer,ulong &offset);
Uint32 readDWord(char *buffer,ulong &offset);
std::string readString(char *buffer,ulong &offset);
void writeByte(Uint8 a,std::string &str,long offset=-1);
void writeWord(Uint16 a,std::string &str,long offset=-1);
void writeDWord(Uint32 a,std::string &str,long offset=-1);
void writeString(const std::wstring &a,std::string &str);
template <typename T>
std::vector<Sint32> getIntervals(typename std::map<Sint32,T>::iterator i,typename std::map<Sint32,T>::iterator end){
	std::vector<Sint32> intervals;
	ulong last=i->first;
	intervals.push_back(last++);
	while (++i!=end){
		if (i->first!=last){
			intervals.push_back(last-intervals.back());
			last=i->first;
			intervals.push_back(last++);
		}else
			last++;
	}
	intervals.push_back(last-intervals.back());
	return intervals;
}

#ifndef BARE_FILE
//bitmap processing functions
void manualBlit(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,uchar alpha=255);
void multiplyBlend(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect);
#endif

//other functions
Uint32 secondsSince1970();
#ifndef BARE_FILE
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
#endif
template <typename T1,typename T2>
bool binary_search(const T1 *set,size_t begin,size_t end,const T2 &value,size_t &at_offset,int (*comp_f)(const T2 &,const T1 &)){
	if (begin<=end){
		size_t size=end-begin+1;
		while (1){
			size_t pivot=begin+size/2;
			int cmp=comp_f(value,set[pivot]);
			if (size==1){
				if (!cmp){
					at_offset=pivot;
					return 1;
				}
				break;
			}
			if (cmp<0)
				end=pivot-1;
			else if (cmp>0)
				begin=pivot+1;
			else{
				at_offset=pivot;
				return 1;
			}
			size=end-begin+1;
		}
	}
	return 0;
}

ErrorCode inPlaceDecryption(char *buffer,ulong length,ulong mode);
#endif
