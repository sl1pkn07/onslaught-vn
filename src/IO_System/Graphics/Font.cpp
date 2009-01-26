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

#ifndef NONS_FONT_CPP
#define NONS_FONT_CPP

#include "Font.h"
#include "../../Globals.h"

NONS_Font::NONS_Font(const char *fontname,int size,int style){
	if (size<=0)
		size=20;
	this->font=TTF_OpenFont(fontname,size);
	if(!font){
		this->font=0;
		v_stderr <<"TTF_OpenFont: "<<TTF_GetError()<<std::endl;
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
		v_stderr <<"TTF_OpenFont: "<<TTF_GetError()<<std::endl;
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
	/*if (this->fontbuffer)
		delete[] this->fontbuffer;
	if (this->fontRWop)
		SDL_FreeRW(this->fontRWop);*/
}

TTF_Font *NONS_Font::getfont(){
	return this->font;
}

int NONS_Font::getsize(){
	return this->size;
}

int NONS_Font::getstyle(){
	return this->style;
}

int NONS_Font::getascent(){
	return this->ascent;
}

void NONS_Font::setStyle(int style){
	TTF_SetFontStyle(this->font,style);
}

bool NONS_Font::valid(){
	return this->font!=0;
}
#endif
