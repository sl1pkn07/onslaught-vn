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

#ifndef NONS_LAYER_CPP
#define NONS_LAYER_CPP

#include "Layer.h"
#include "../../Globals.h"

NONS_Layer::NONS_Layer(SDL_Rect *size,unsigned rgba){
	this->data=makeSurface(size->w,size->h,32);
	SDL_FillRect(this->data,0,rgba);
	this->defaultShade=rgba;
	this->fontCache=0;
	this->visible=1;
	this->useDataAsDefaultShade=0;
	this->alpha=0xFF;
	this->clip_rect=this->data->clip_rect;
	this->clip_rect.x=size->x;
	this->clip_rect.y=size->y;
	this->position.x=0;
	this->position.y=0;
	this->position.w=0;
	this->position.h=0;
}

NONS_Layer::NONS_Layer(SDL_Surface *img,unsigned rgba){
	this->data=img;
	this->defaultShade=rgba;
	this->fontCache=0;
	this->visible=1;
	this->useDataAsDefaultShade=0;
	this->alpha=0xFF;
	this->clip_rect=this->data->clip_rect;
	this->position.x=0;
	this->position.y=0;
	this->position.w=0;
	this->position.h=0;
}

NONS_Layer::NONS_Layer(const std::wstring *string){
	this->defaultShade=0;
	this->fontCache=0;
	this->visible=1;
	this->useDataAsDefaultShade=0;
	this->alpha=0xFF;
	this->data=0;
	this->load(string);
	this->position.x=0;
	this->position.y=0;
	this->position.w=0;
	this->position.h=0;
}

NONS_Layer::~NONS_Layer(){
	this->unload();
	if (this->fontCache)
		delete this->fontCache;
}

void NONS_Layer::MakeTextLayer(NONS_Font *font,SDL_Color *foreground,bool shadow){
	this->fontCache=new NONS_FontCache(font,foreground,shadow);
}

void NONS_Layer::Clear(){
	if (!this->useDataAsDefaultShade){
		this->load((const std::wstring *)0);
		SDL_FillRect(this->data,0,this->defaultShade);
	}
}

void NONS_Layer::setShade(uchar r,uchar g,uchar b){
	if (!this->data)
		this->data=makeSurface(this->clip_rect.w,this->clip_rect.h,32);
	SDL_PixelFormat *format=this->data->format;
	unsigned r0=r,
		g0=g,
		b0=b,
		rgb=(0xFF<<(format->Ashift))|(r0<<(format->Rshift))|(g0<<(format->Gshift))|(b0<<(format->Bshift));
	this->defaultShade=rgb;
}

void NONS_Layer::usePicAsDefaultShade(SDL_Surface *pic){
	if (this->data)
		SDL_FreeSurface(this->data);
	this->data=pic;
	this->useDataAsDefaultShade=1;
}

bool NONS_Layer::load(const std::wstring *string){
	if (!string){
		int w=this->data->w,
			h=this->data->h;
		if (this->unload(1)){
			this->data=makeSurface(w,h,32);
			this->clip_rect=this->data->clip_rect;
		}
		return 1;
	}
	this->unload();
	this->data=ImageLoader->fetchSprite(*string,&this->optimized_updates);
	if (!this->data){
		this->data=makeSurface(1,1,24);
		SDL_FillRect(this->data,0,this->defaultShade);
		this->clip_rect=this->data->clip_rect;
		return 0;
	}
	this->animation.parse(*string);
	this->clip_rect=this->data->clip_rect;
	/*if (this->animation.animation_length>1){
		ulong t0=SDL_GetTicks();
		SDL_Rect rect=this->getUpdateRect(0,1);
		ulong t1=SDL_GetTicks();
		std::cout <<"completed in "<<t1-t0<<" msec."<<std::endl;
	}*/
	return 1;
}

bool NONS_Layer::load(SDL_Surface *src){
	this->unload();
	this->data=makeSurface(src->w,src->h,32);
	//SDL_FillRect(this->data,0,gmask|amask);
	manualBlit(src,0,this->data,0);
	return 1;
}

bool NONS_Layer::unload(bool youCantTouchThis){
	if (!this || !this->data)
		return 1;
	if (ImageLoader->unfetchImage(this->data)){
		this->data=0;
		this->optimized_updates.clear();
		return 1;
	}else if (!youCantTouchThis){
		SDL_FreeSurface(this->data);
		this->data=0;
		this->optimized_updates.clear();
		return 1;
	}
	return 0;
}

bool NONS_Layer::advanceAnimation(ulong msec){
	long frame=this->animation.advanceAnimation(msec);
	if (frame<0)
		return 0;
	this->clip_rect.x=frame*this->clip_rect.w;
	return 1;
}

void NONS_Layer::centerAround(int x){
	this->position.x=x-this->clip_rect.w/2;
}

void NONS_Layer::useBaseline(int y){
	this->position.y=y-this->clip_rect.h+1;
}
#endif
