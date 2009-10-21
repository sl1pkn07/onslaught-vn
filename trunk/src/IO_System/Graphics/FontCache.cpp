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

#include "FontCache.h"
#include "../../Common.h"
#include "../IOFunctions.h"

//#define BLEND_WITH_SDLBLIT

NONS_Font::NONS_Font(const char *fontname,int size,int style){
	if (size<=0)
		size=20;
	this->font=TTF_OpenFont(fontname,size);
	if(!font){
		this->font=0;
		o_stderr <<"TTF_OpenFont: "<<TTF_GetError()<<"\n";
		return;
	}
	TTF_SetFontStyle(this->font,style);
	this->ascent=TTF_FontAscent(this->font);
	this->lineSkip=TTF_FontLineSkip(this->font);
	this->fontLineSkip=this->lineSkip;
	this->spacing=0;
	this->size=size;
}

NONS_Font::NONS_Font(SDL_RWops *rwop,int size,int style){
	if (size<=0)
		size=20;
	this->font=TTF_OpenFontRW(rwop,1,size);
	if(!font){
		this->font=0;
		o_stderr <<"TTF_OpenFont: "<<TTF_GetError()<<"\n";
		return;
	}
	TTF_SetFontStyle(this->font,style);
	this->ascent=TTF_FontAscent(this->font);
	this->lineSkip=TTF_FontLineSkip(this->font);
	this->fontLineSkip=this->lineSkip;
	this->spacing=0;
	this->size=size;
}

NONS_Font::NONS_Font(){
	this->font=0;
}

NONS_Font::~NONS_Font(){
	if (this->font)
		TTF_CloseFont(this->font);
}

bool NONS_Glyph::equalColors(SDL_Color *a,SDL_Color *b){
	unsigned a0=((a->r)<<16)+((a->g)<<8)+(a->b);
	unsigned b0=((b->r)<<16)+((b->g)<<8)+b->b;
	return (a0==b0);
}

NONS_Glyph::NONS_Glyph(NONS_Font *font,wchar_t character,int ascent,SDL_Color *foreground,bool shadow){
	this->ttf_font=font->getfont();
	this->glyph=TTF_RenderGlyph_Blended(this->ttf_font,character,*foreground);
#ifdef BLEND_WITH_SDLBLIT
	SDL_SetAlpha(glyph,SDL_SRCALPHA,0);
#endif
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
#ifdef BLEND_WITH_SDLBLIT
	SDL_SetAlpha(glyph,(!method)?SDL_SRCALPHA:0,0);
	SDL_BlitSurface(this->glyph,0,dst,&rect);
#else
	manualBlit(this->glyph,0,dst,&rect);
#endif
}

SDL_Color NONS_Glyph::getforeground(){
	return this->foreground;
}

NONS_FontCache::NONS_FontCache(NONS_Font *font,SDL_Color *foreground,bool shadow){
	this->foreground=*foreground;
	this->glyphCache.reserve(128);
	this->shadow=shadow;
	this->font=font;
	this->refreshCache();
}

NONS_FontCache::~NONS_FontCache(){
	for (ulong a=0;a<this->glyphCache.size();a++)
		delete this->glyphCache[a];
}

void NONS_FontCache::refreshCache(){
	for (ulong a=0;a<this->glyphCache.size();a++)
		if (this->glyphCache[a])
			delete this->glyphCache[a];
	this->glyphCache.clear();
	this->glyphCache.reserve(128);
	for (wchar_t a=0;a<128;a++){
		NONS_Glyph *glyph=new NONS_Glyph(this->font,a,this->font->getascent(),&this->foreground,shadow);
		this->glyphCache.push_back(glyph);
	}
}

std::vector<NONS_Glyph *> *NONS_FontCache::getglyphCache(){
	return &(this->glyphCache);
}

NONS_Glyph *NONS_FontCache::getGlyph(wchar_t codePoint){
	switch (codePoint){
		case 0:
		case '\t':
		case '\n':
		case '\r':
			return 0;
		default:
			break;
	}
	for (ulong a=0;a<this->glyphCache.size();a++)
		if (this->glyphCache[a]->getcodePoint()==codePoint)
			return this->glyphCache[a];
	NONS_Glyph *glyph=new NONS_Glyph(this->font,codePoint,this->font->getascent(),&(this->foreground),this->shadow);
	this->glyphCache.push_back(glyph);
	return this->glyphCache.back();
}

NONS_Font *init_font(ulong size,NONS_GeneralArchive *archive){
	NONS_Font *font=new NONS_Font("default.ttf",(size),TTF_STYLE_NORMAL);
	if (!font->valid()){
		delete font;
		ulong l;
		uchar *buffer=archive->getFileBuffer(L"default.ttf",l);
		if (!buffer){
			o_stderr <<"FATAL ERROR: Could not find \"default.ttf\" font file. If your system is\n"
				"case-sensitive, make sure the file name is capitalized correctly.\n";
			exit(0);
			return 0;
		}
		SDL_RWops *rw=SDL_RWFromMem(buffer,l);
		font=new NONS_Font(rw,size,TTF_STYLE_NORMAL);
		SDL_FreeRW(rw);
		delete[] buffer;
	}
	return font;
}
