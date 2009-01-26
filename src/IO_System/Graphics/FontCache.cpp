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

#ifndef NONS_FONTCACHE_CPP
#define NONS_FONTCACHE_CPP

#include "FontCache.h"
#include "../../Common.h"
#include "../../Globals.h"

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
	for (wchar_t a=0;a<this->glyphCache.size();a++)
		if (this->glyphCache[a])
			delete this->glyphCache[a];
	this->glyphCache.clear();
	this->glyphCache.reserve(128);
	for (wchar_t a=0;a<128;a++){
		NONS_Glyph *glyph=new NONS_Glyph(this->font,a,this->font->getascent(),&(this->foreground),shadow);
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
	return this->glyphCache[this->glyphCache.size()-1];
}
#endif
