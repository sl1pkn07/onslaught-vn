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

#ifndef TOOLS_BARE_FILE
#include "ErrorCodes.h"
#include <map>
#ifndef TOOLS_NSAIO
#include "Common.h"
#include <SDL/SDL.h>
#include <iomanip>
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
#define CHECK_FLAG(x,y) (((x)&(y))==(y))

//string functions
bool multicomparison(char character,const char *characters);
bool multicomparison(wchar_t character,const char *characters);
bool multicomparison(char character,const wchar_t *characters);
bool multicomparison(wchar_t character,const wchar_t *characters);
template <typename T>
void toforwardslash(std::basic_string<T> &s){
	for (ulong a=0,size=s.size();a<size;a++)
		s[a]=(s[a]==0x5C)?0x2F:s[a];
}
template <typename T>
void tobackslash(std::basic_string<T> &s){
	for (ulong a=0,size=s.size();a<size;a++)
		s[a]=(s[a]==0x2F)?0x5C:s[a];
}

template <typename T>
std::vector<std::basic_string<T> > getParameterList(const std::basic_string<T> &string,bool leave_quotes,char delim=' '){
	std::vector<std::basic_string<T> > res;
	char tempDelim=delim;
	for (ulong a=0,size=string.size();a<size;){
		if (string[a]=='\"'){
			if (!leave_quotes)
				a++;
			delim='\"';
		}
		ulong end=a;
		if (delim!=tempDelim && leave_quotes)
			end++;
		for (;end<size && string[end]!=delim;end++);
		if (delim!=tempDelim && leave_quotes)
			end++;
		res.push_back(std::basic_string<T>(string,a,end-a));
		delim=tempDelim;
		for (a=end+1;a<size && string[a]==delim;a++);
	}
	return res;
}
/*bool filenames_are_equal(const wchar_t *str0,const wchar_t *str1);
bool filenames_are_equal(const char *str0,const char *str1);*/
template <typename T>
bool isValidIdentifier(const std::basic_string<T> &str){
	if (!NONS_isid1char(str[0]))
		return 0;
	const T *s=&str[1];
	for (ulong a=1,size=str.size();a<size;a++,s++)
		if (!NONS_isidnchar(*s))
			return 0;
	return 1;
}
template <typename T>
bool isValidLabel(const std::basic_string<T> &str){
	const T *s=&str[0];
	for (ulong a=0,size=str.size();a<size;a++,s++)
		if (!NONS_isidnchar(*s))
			return 0;
	return 1;
}
template <typename T>
inline T HEX2DEC(T x){
	return x<='9'?x-'0':(x<='F'?x-'A'+10:x-'a'+10);
}
template <typename T>
long atoi(const std::basic_string<T> &str){
	std::basic_stringstream<T> stream(str);
	long res;
	return !(stream >>res)?0:res;
}
template <typename T>
std::basic_string<T> itoa(long n,unsigned w=0){
	std::basic_stringstream<T> stream;
	if (w){
		stream.fill('0');
		stream.width(w);
	}
	stream <<n;
	return stream.str();
}
//1 if the s1 begins with s2 at off
template <typename T>
bool firstchars(const std::basic_string<T> &s1,size_t off,const std::basic_string<T> &s2){
	if (s1.size()-off<s2.size())
		return 0;
	for (ulong a=0;a<s2.size();a++)
		if (s1[off+a]!=s2[a])
			return 0;
	return 1;
}

template <typename T>
bool firstchars(const std::basic_string<T> &s1,size_t off,const T *s2){
	ulong l=0;
	while (s2[l])
		l++;
	if (s1.size()-off<l)
		return 0;
	for (ulong a=0;a<l;a++)
		if (s1[off+a]!=s2[a])
			return 0;
	return 1;
}

template <typename T>
std::basic_string<T> string_replace(
		const std::basic_string<T> &src,
		const std::basic_string<T> &what,
		const std::basic_string<T> &with){
	if (!what.size())
		return src;
	std::basic_string<T> res;
	for (ulong a=0,end=src.size();a<end;){
		ulong found=src.find(what,a);
		if (found!=src.npos){
			res.append(src,a,found-a);
			res.append(with);
			a=found+what.size();
		}else{
			res.append(src,a,found);
			a=found;
		}
	}
	return res;
}

template <typename T>
inline std::basic_string<T> string_replace(
		const std::basic_string<T> &src,
		const T *what,
		const T *with){
	if (!what || !*what)
		return src;
	std::basic_string<T> res;
	ulong l=0,l2=0;
	while (what[l])
		l++;
	if (with)
		while (with[l2])
			l2++;
	for (ulong a=0,end=src.size();a<end;){
		ulong found=src.find(what,a,l);
		if (found!=src.npos){
			res.append(src,a,found-a);
			if (with)
				res.append(with,l2);
			a=found+l;
		}else{
			res.append(src,a,found);
			a=found;
		}
	}
	return res;
}


//string parsing
template <typename T>
std::basic_string<T> tagName(const std::basic_string<T> &string,size_t off){
	if (string[off]!='<')
		return std::basic_string<T>();
	ulong a=string.find('>',off+1),
		b=string.find('=',off+1);
	a=std::min(a,b);
	if (a==string.npos)
		return std::basic_string<T>();
	std::basic_string<T> temp(string,off+1,a-(off+1));
	trim_string(temp);
	return temp;
}
template <typename T>
std::basic_string<T> tagValue(const std::basic_string<T> &string,size_t off){
	if (string[off]!='<')
		return std::basic_string<T>();
	ulong a=string.find('>',off+1),
		b=string.find('=',off+1);
	a=std::min(a,b);
	if (a==string.npos || string[a]=='>')
		return std::basic_string<T>();
	a++;
	ulong c=string.find('>',a);
	std::basic_string<T> temp(string,a,a-c);
	trim_string(temp);
	return temp;
}

//binary parsing functions
bool getbit(uchar *arr,ulong *byteoffset,uchar *bitoffset);
ulong getbits(uchar *arr,uchar bits,ulong *byteoffset,uchar *bitoffset);
Uint8 readByte(char *buffer,ulong &offset);
Sint16 readSignedWord(char *buffer,ulong &offset);
Uint16 readWord(char *buffer,ulong &offset);
Sint32 readSignedDWord(char *buffer,ulong &offset);
Uint32 readDWord(char *buffer,ulong &offset);
std::string readString(char *buffer,ulong &offset);
void writeByte(Uint8 a,std::string &str,ulong offset=ULONG_MAX);
void writeWord(Uint16 a,std::string &str,ulong offset=ULONG_MAX);
void writeDWord(Uint32 a,std::string &str,ulong offset=ULONG_MAX);
void writeWordBig(Uint16 a,std::string &str,ulong offset=ULONG_MAX);
void writeDWordBig(Uint32 a,std::string &str,ulong offset=ULONG_MAX);
void writeString(const std::wstring &a,std::string &str);
template <typename T>
std::vector<Sint32> getIntervals(typename std::map<Sint32,T>::iterator i,typename std::map<Sint32,T>::iterator end){
	std::vector<Sint32> intervals;
	Sint32 last=i->first;
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

extern const int rmask;
extern const int gmask;
extern const int bmask;
extern const int amask;

inline SDL_Surface *makeSurface(ulong w,ulong h,ulong bits,Uint32 r=rmask,Uint32 g=gmask,Uint32 b=bmask,Uint32 a=amask){
	return SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,w,h,bits,r,g,b,a);
}

#if !defined(TOOLS_BARE_FILE) && !defined(TOOLS_NSAIO)
//bitmap processing functions
typedef long manualBlitAlpha_t;
void manualBlit(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,manualBlitAlpha_t alpha=255);
void multiplyBlend(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect);
void FlipSurfaceH(SDL_Surface *src,SDL_Surface *dst);
void FlipSurfaceV(SDL_Surface *src,SDL_Surface *dst);
void FlipSurfaceHV(SDL_Surface *src,SDL_Surface *dst);
SDL_Surface *horizontalShear(SDL_Surface *src,float amount);
SDL_Surface *verticalShear(SDL_Surface *src,float amount);
SDL_Surface *applyTransformationMatrix(SDL_Surface *src,float matrix[4]);
#endif

//other functions
Uint32 secondsSince1970();
#ifndef TOOLS_BARE_FILE
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
		while (begin<=end){
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
#endif

ErrorCode inPlaceDecryption(char *buffer,ulong length,ulong mode);
#endif
