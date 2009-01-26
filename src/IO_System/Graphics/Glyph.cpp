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

#ifndef NONS_GLYPH_CPP
#define NONS_GLYPH_CPP

#include "Glyph.h"
#include "../../Functions.h"
#include "../../Globals.h"

bool NONS_Glyph::equalColors(SDL_Color *a,SDL_Color *b){
	unsigned a0=((a->r)<<16)+((a->g)<<8)+(a->b);
	unsigned b0=((b->r)<<16)+((b->g)<<8)+b->b;
	return (a0==b0);
}

NONS_Glyph::NONS_Glyph(NONS_Font *font,wchar_t character,int ascent,SDL_Color *foreground,bool shadow){
	this->ttf_font=font->getfont();
	this->glyph=TTF_RenderGlyph_Blended(this->ttf_font,character,*foreground);
	SDL_SetAlpha(glyph,SDL_SRCALPHA,0);
	int x0,y1;
	TTF_GlyphMetrics(this->ttf_font,character,&x0,0,0,&y1,&this->advance);
	this->box=this->glyph->clip_rect;
	this->box.x+=x0;
	this->box.y+=-y1+ascent;
	this->codePoint=character;
	this->foreground=*foreground;
	this->font=font;
	this->style=TTF_GetFontStyle(this->ttf_font);
}

NONS_Glyph::~NONS_Glyph(){
	SDL_FreeSurface(this->glyph);
}

wchar_t NONS_Glyph::getcodePoint(){
	return this->codePoint;
}

SDL_Rect NONS_Glyph::getbox(){
	return this->box;
}

int NONS_Glyph::getadvance(){
	return this->advance+this->font->spacing;
}

void NONS_Glyph::putGlyph(SDL_Surface *dst,int x,int y,SDL_Color *foreground,bool method){
	if (foreground && !this->equalColors(foreground,&this->foreground) || this->style!=TTF_GetFontStyle(this->ttf_font)){
		SDL_FreeSurface(this->glyph);
		this->glyph=TTF_RenderGlyph_Blended(this->ttf_font,this->codePoint,*foreground);
		this->foreground=*foreground;
	}
	SDL_Rect rect=this->box;
	rect.x+=x;
	rect.y+=y;
	SDL_SetAlpha(glyph,(!method)?SDL_SRCALPHA:0,0);
	/*SDL_FillRect(dst,&rect,0xFF0000FF);
	SDL_Rect rect2=rect;
	rect2.x++;
	rect2.y++;
	rect2.w-=2;
	rect2.h-=2;
	SDL_FillRect(dst,&rect2,0xFF000000);*/
	SDL_BlitSurface(this->glyph,0,dst,&rect);
	//manualBlit(this->glyph,0,dst,&rect);
}

SDL_Color NONS_Glyph::getforeground(){
	return this->foreground;
}
#endif
