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
#endif

#include <vector>
#include <string>
#include <ctime>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define _HANDLE_POSSIBLE_ERRORS(x,extra) {ErrorCode possible_error=(x);if (possible_error!=NONS_NO_ERROR){extra return possible_error;}}
#define _CHECK_ID_NAME(pointertostring)\
{\
	if (!isalpha(*(pointertostring)) && *(pointertostring)!='_')\
		return NONS_INVALID_ID_NAME;\
	for (wchar_t *a=(pointertostring)+1;*a;a++)\
		if (!isalnum(*a) && *a!='_')\
			return NONS_INVALID_ID_NAME;\
}

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
bool isanumber(const char *a);
bool isanumber(const wchar_t *a);
std::vector<char *> *getParameterList(const char *string,char delim=' ');
std::vector<wchar_t *> *getParameterList(const wchar_t *string,wchar_t delim=' ');


//string parsing
wchar_t *tagName(const wchar_t *string);
char *tagName(const char *string);
wchar_t *tagValue(const wchar_t *string);
char *tagValue(const char *string);

//binary parsing functions
bool getbit(uchar *arr,ulong *byteoffset,uchar *bitoffset);
ulong getbits(uchar *arr,uchar bits,ulong *byteoffset,uchar *bitoffset);
Uint8 readByte(char *buffer,long *offset);
Sint16 readSignedWord(char *buffer,long *offset);
Uint16 readWord(char *buffer,long *offset);
Sint32 readSignedDWord(char *buffer,long *offset);
Uint32 readDWord(char *buffer,long *offset);
char *readString(char *buffer,long *offset);
void writeByte(Uint8 a,std::string *str,long offset=-1);
void writeWord(Uint16 a,std::string *str,long offset=-1);
void writeDWord(Uint32 a,std::string *str,long offset=-1);
void writeString(const wchar_t *a,std::string *str);

#ifndef BARE_FILE
//bitmap processing functions
void manualBlit(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,uchar alpha=255);
void multiplyBlend(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect);
#endif

//other functions
Uint32 secondsSince1900();
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
#endif