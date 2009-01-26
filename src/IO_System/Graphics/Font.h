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

#ifndef NONS_FONT_H
#define NONS_FONT_H

#include "SDL_ttf.h"
#include "../../Common.h"

#define INIT_NONS_FONT(font,size,archive) {\
	(font)=new NONS_Font("default.ttf",(size),TTF_STYLE_NORMAL);\
	if (!(font)->valid()){\
		delete (font);\
		ulong INIT_NONS_FONT_l;\
		uchar *INIT_NONS_FONT_buffer=(archive)->getFileBuffer("default.ttf",&INIT_NONS_FONT_l);\
		if (!INIT_NONS_FONT_buffer){\
			v_stderr <<"FATAL ERROR: Could not find \"default.ttf\" font file. If your system is\n"\
				"case-sensitive, make sure the file name is capitalized correctly."<<std::endl;\
			exit(0);\
		}\
		SDL_RWops *INIT_NONS_FONT_rw=SDL_RWFromMem(INIT_NONS_FONT_buffer,INIT_NONS_FONT_l);\
		(font)=new NONS_Font(INIT_NONS_FONT_rw,(size),TTF_STYLE_NORMAL);\
		SDL_FreeRW(INIT_NONS_FONT_rw);\
		delete[] INIT_NONS_FONT_buffer;\
	}\
}

class NONS_Font{
	TTF_Font *font;
	int size;
	int style;
	int ascent;
	/*uchar *fontbuffer;
	SDL_RWops *fontRWop;*/
public:
	int lineSkip;
	int fontLineSkip;
	int spacing;
	NONS_Font();
	NONS_Font(const char *fontname,int size,int style);
	NONS_Font(SDL_RWops *rwop,int size,int style);
	~NONS_Font();
	TTF_Font *getfont();
	int getsize();
	int getstyle();
	int getascent();
	void setStyle(int style);
	bool valid();
};
#endif
