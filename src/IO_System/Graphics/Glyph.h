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
*     * Helios' name may not be used to endorse or promote products derived from
*       this software without specific prior written permission.
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

#ifndef NONS_GLYPH_H
#define NONS_GLYPH_H

#include <SDL/SDL.h>
#include "SDL_ttf.h"
#include "Font.h"
#include "../../Common.h"

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
#endif
