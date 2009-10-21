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

#ifndef NONS_FONTCACHE_H
#define NONS_FONTCACHE_H

#include <vector>
#include <SDL/SDL.h>
#include "SDL_ttf.h"
#include "../../Common.h"
#include "../SAR/Archive.h"

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
	TTF_Font *getfont(){
		return this->font;
	}
	int getsize(){
		return this->size;
	}
	int getstyle(){
		return this->style;
	}
	int getascent(){
		return this->ascent;
	}
	void setStyle(int style){
		TTF_SetFontStyle(this->font,style);
	}
	bool valid(){
		return this->font!=0;
	}
};

struct NONS_Glyph{
	//The font structure.
	NONS_Font *font;
	TTF_Font *ttf_font;
	//Surface storing the rendered glyph.
	SDL_Surface *glyph;
	//Unicode code point for the glyph.
	wchar_t codePoint;
	//SDL_Rect storing glyph size information
	SDL_Rect box;
	//Glyph advance
	int advance;
	//The color the glyph was rendered with.
	SDL_Color foreground;
	//The style the glyph was rendered with.
	int style;
	//Check if two SDL_Colors have the same values.
	bool equalColors(SDL_Color *a,SDL_Color *b);
	NONS_Glyph(NONS_Font *font,wchar_t character,int ascent,SDL_Color *foreground,bool shadow);
	~NONS_Glyph();
	wchar_t getcodePoint();
	SDL_Rect getbox();
	int getadvance();
	void putGlyph(SDL_Surface *dst,int x,int y,SDL_Color *foreground,bool method=0);
	SDL_Color getforeground();
};

class NONS_FontCache{
	bool shadow;
	std::vector<NONS_Glyph *> glyphCache;
public:
	//Foreground color. This is not guaranteed to be the color of the individual
	//glyphs, but it is guaranteed to be the color of the glyphs returned by
	//getGlyph().
	SDL_Color foreground;
	NONS_FontCache(NONS_Font *font,SDL_Color *foreground,bool shadow);
	~NONS_FontCache();
	NONS_Font *font;
	std::vector<NONS_Glyph *> *getglyphCache();
	NONS_Glyph *getGlyph(wchar_t codePoint);
	void refreshCache();
};

#if 0
#define INIT_NONS_FONT(font,size,archive) {\
	(font)=new NONS_Font("default.ttf",(size),TTF_STYLE_NORMAL);\
	if (!(font)->valid()){\
		delete (font);\
		ulong INIT_NONS_FONT_l;\
		uchar *INIT_NONS_FONT_buffer=(archive)->getFileBuffer(L"default.ttf",INIT_NONS_FONT_l);\
		if (!INIT_NONS_FONT_buffer){\
			o_stderr <<"FATAL ERROR: Could not find \"default.ttf\" font file. If your system is\n"\
				"case-sensitive, make sure the file name is capitalized correctly.\n";\
			exit(0);\
		}\
		SDL_RWops *INIT_NONS_FONT_rw=SDL_RWFromMem(INIT_NONS_FONT_buffer,INIT_NONS_FONT_l);\
		(font)=new NONS_Font(INIT_NONS_FONT_rw,(size),TTF_STYLE_NORMAL);\
		SDL_FreeRW(INIT_NONS_FONT_rw);\
		delete[] INIT_NONS_FONT_buffer;\
	}\
}
#else
NONS_Font *init_font(ulong size,NONS_GeneralArchive *archive);
#endif
#endif
