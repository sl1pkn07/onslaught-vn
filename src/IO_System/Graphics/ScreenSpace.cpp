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

#ifndef NONS_SCREENSPACE_CPP
#define NONS_SCREENSPACE_CPP

#include "ScreenSpace.h"
#include "../../Functions.h"
#include "../../Globals.h"

NONS_ScreenSpace::NONS_ScreenSpace(int framesize,NONS_Font *font){
	this->screen=new NONS_VirtualScreen(CLOptions.virtualWidth,CLOptions.virtualHeight,CLOptions.realWidth,CLOptions.realHeight);
	SDL_Rect size={0,0,CLOptions.virtualWidth,CLOptions.virtualHeight};
	SDL_Rect frame={framesize,framesize,CLOptions.virtualWidth-framesize*2,CLOptions.virtualHeight-framesize*2};
	this->output=new NONS_StandardOutput(font,&size,&frame);
	this->output->visible=0;
	this->layerStack.resize(1000,0);
	this->Background=new NONS_Layer(&size,0xFF000000);
	this->leftChar=0;
	this->rightChar=0;
	this->centerChar=0;
	this->screenBuffer=SDL_CreateRGBSurface(
		SDL_HWSURFACE|SDL_SRCALPHA,
		this->screen->virtualScreen->w,
		this->screen->virtualScreen->h,
		32,
		rmask,gmask,bmask,amask);
	//if (!store)
		this->gfx_store=new NONS_GFXstore();
	/*else
		this->gfx_store=store;*/
	this->monochrome=0;
	this->negative=0;
	this->sprite_priority=25;
	SDL_Color *temp=&this->output->foregroundLayer->fontCache->foreground;
	this->lookback=new NONS_Lookback(this->output,temp->r,temp->g,temp->b);
	this->cursor=0;
	this->char_baseline=this->screenBuffer->h-1;
}

NONS_ScreenSpace::NONS_ScreenSpace(SDL_Rect *window,SDL_Rect *frame,NONS_Font *font,bool shadow){
	this->screen=new NONS_VirtualScreen(CLOptions.virtualWidth,CLOptions.virtualHeight,CLOptions.realWidth,CLOptions.realHeight);
	this->output=new NONS_StandardOutput(font,window,frame);
	this->output->visible=0;
	this->layerStack.resize(1000,0);
	SDL_Rect size={0,0,CLOptions.virtualWidth,CLOptions.virtualHeight};
	this->Background=new NONS_Layer(&size,0xFF000000);
	this->leftChar=0;
	this->rightChar=0;
	this->centerChar=0;
	this->screenBuffer=SDL_CreateRGBSurface(
		SDL_HWSURFACE|SDL_SRCALPHA,
		this->screen->virtualScreen->w,
		this->screen->virtualScreen->h,
		32,
		rmask,gmask,bmask,amask);
	this->gfx_store=new NONS_GFXstore();
	this->sprite_priority=25;
	SDL_Color *temp=&this->output->foregroundLayer->fontCache->foreground;
	this->lookback=new NONS_Lookback(this->output,temp->r,temp->g,temp->b);
	this->cursor=0;
	this->char_baseline=this->screenBuffer->h-1;
}

NONS_ScreenSpace::~NONS_ScreenSpace(){
	delete this->output;
	for (ulong a=0;a<this->layerStack.size();a++)
		if (this->layerStack[a])
			delete this->layerStack[a];
	delete this->leftChar;
	delete this->rightChar;
	delete this->centerChar;
	delete this->Background;
	delete this->screen;
	//delete this->this->cursor;
	SDL_FreeSurface(this->screenBuffer);
	delete this->gfx_store;
	if (this->monochrome)
		delete this->monochrome;
	if (this->negative)
		delete this->negative;
	delete this->lookback;
}

void NONS_ScreenSpace::BlendOptimized(std::vector<SDL_Rect> &rects){
	if (!rects.size())
		return;
////////////////////////////////////////////////////////////////////////////////
#define BLEND_OPTIM(p,function) {\
	if ((p) && (p)->data && (p)->visible){\
		SDL_Rect src={\
			refresh_area.x-(p)->position.x+(p)->clip_rect.x,\
			refresh_area.y-(p)->position.y+(p)->clip_rect.y,\
			refresh_area.w>(p)->clip_rect.w?(p)->clip_rect.w:refresh_area.w,\
			refresh_area.h>(p)->clip_rect.h?(p)->clip_rect.h:refresh_area.h\
		};\
		if (src.x<(p)->clip_rect.x)\
			src.x=(p)->clip_rect.x;\
		if (src.y<(p)->clip_rect.y)\
			src.y=(p)->clip_rect.y;\
		SDL_Rect dst=refresh_area;\
		if (dst.x<(p)->position.x)\
			dst.x=(p)->position.x;\
		if (dst.y<(p)->position.y)\
			dst.y=(p)->position.y;\
		function((p)->data,&src,this->screenBuffer,&dst);\
	}\
}
////////////////////////////////////////////////////////////////////////////////
	ulong minx=rects[0].x,
		maxx=minx+rects[0].w,
		miny=rects[0].y,
		maxy=miny+rects[0].h;
	for (ulong a=1;a<rects.size();a++){
		ulong x0=rects[a].x,
			x1=x0+rects[a].w,
			y0=rects[a].y,
			y1=y0+rects[a].h;
		if (x0<minx)
			minx=x0;
		if (x1>maxx)
			maxx=x1;
		if (y0<miny)
			miny=y0;
		if (y1>maxy)
			maxy=y1;
	}
	SDL_Rect refresh_area={minx,miny,maxx-minx,maxy-miny};
	if (!(refresh_area.w*refresh_area.h))
		return;
	SDL_FillRect(this->screenBuffer,&refresh_area,amask);
	BLEND_OPTIM(this->Background,manualBlit);
	for (ulong a=this->layerStack.size()-1;a>this->sprite_priority;a--){
		NONS_Layer *p=this->layerStack[a];
		BLEND_OPTIM(p,manualBlit);
	}
	BLEND_OPTIM(this->leftChar,manualBlit);
	BLEND_OPTIM(this->rightChar,manualBlit);
	BLEND_OPTIM(this->centerChar,manualBlit);
	for (long a=this->sprite_priority;a>=0;a--){
		NONS_Layer *p=this->layerStack[a];
		BLEND_OPTIM(p,manualBlit);
	}
	if (this->monochrome)
		this->monochrome->call(0,this->screenBuffer,0);
	if (this->negative)
		this->negative->call(0,this->screenBuffer,0);
	if (this->output->visible){
		if (!this->output->shadeLayer->useDataAsDefaultShade){
			BLEND_OPTIM(this->output->shadeLayer,multiplyBlend);
		}else{
			BLEND_OPTIM(this->output->shadeLayer,manualBlit);
		}
		BLEND_OPTIM(this->output->shadowLayer,manualBlit);
		BLEND_OPTIM(this->output->foregroundLayer,manualBlit);
	}
	BLEND_OPTIM(this->cursor,manualBlit);
	LOCKSCREEN;
	manualBlit(this->screenBuffer,&refresh_area,this->screen->virtualScreen,&refresh_area);
	UNLOCKSCREEN;
	this->screen->updateScreen(refresh_area.x,refresh_area.y,refresh_area.w,refresh_area.h);
}

ErrorCode NONS_ScreenSpace::BlendAll(ulong effect){
	this->BlendNoCursor(0);
	if (this->cursor && this->cursor->data)
		manualBlit(this->cursor->data,&this->cursor->clip_rect,this->screenBuffer,&this->cursor->position);
	if (effect){
		NONS_GFX *e=this->gfx_store->retrieve(effect);
		if (!e)
			return NONS_UNDEFINED_EFFECT;
		return e->call(this->screenBuffer,0,this->screen);
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScreenSpace::BlendAll(ulong effect,long timing,const std::wstring *rule){
	this->BlendAll(0);
	return NONS_GFX::callEffect(effect,timing,rule,this->screenBuffer,0,this->screen);
}

ErrorCode NONS_ScreenSpace::BlendNoCursor(ulong effect){
	this->BlendNoText(0);
	if (this->output->visible){
		if (!this->output->shadeLayer->useDataAsDefaultShade)
			multiplyBlend(this->output->shadeLayer->data,0,this->screenBuffer,&(this->output->shadeLayer->clip_rect));
		else
			manualBlit(this->output->shadeLayer->data,0,this->screenBuffer,&(this->output->shadeLayer->clip_rect));
		if (this->output->shadowLayer)
			manualBlit(this->output->shadowLayer->data,0,this->screenBuffer,&(this->output->shadowLayer->clip_rect),this->output->shadowLayer->alpha);
		manualBlit(this->output->foregroundLayer->data,0,this->screenBuffer,&(this->output->foregroundLayer->clip_rect),this->output->foregroundLayer->alpha);
	}
	if (effect){
		NONS_GFX *e=this->gfx_store->retrieve(effect);
		if (!e)
			return NONS_UNDEFINED_EFFECT;
		return e->call(this->screenBuffer,0,this->screen);
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScreenSpace::BlendNoCursor(ulong effect,long timing,const std::wstring *rule){
	this->BlendNoCursor(0);
	return NONS_GFX::callEffect(effect,timing,rule,this->screenBuffer,0,this->screen);
}

ErrorCode NONS_ScreenSpace::BlendNoText(ulong effect){
	this->BlendOnlyBG(0);
	for (ulong a=this->layerStack.size()-1;a>this->sprite_priority;a--)
		if (this->layerStack[a] && this->layerStack[a]->visible && this->layerStack[a]->data)
			manualBlit(
				this->layerStack[a]->data,
				&this->layerStack[a]->clip_rect,
				this->screenBuffer,
				&this->layerStack[a]->position,
				this->layerStack[a]->alpha);
	if (this->leftChar && this->leftChar->data)
		manualBlit(
			this->leftChar->data,
			&this->leftChar->clip_rect,
			this->screenBuffer,
			&this->leftChar->position,
			this->leftChar->alpha);
	if (this->rightChar && this->rightChar->data)
		manualBlit(
			this->rightChar->data,
			&this->rightChar->clip_rect,
			this->screenBuffer,
			&this->rightChar->position,
			this->rightChar->alpha);
	if (this->centerChar && this->centerChar->data)
		manualBlit(
			this->centerChar->data,
			&this->centerChar->clip_rect,
			this->screenBuffer,
			&this->centerChar->position,
			this->centerChar->alpha);
	for (long a=this->sprite_priority;a>=0;a--)
		if (this->layerStack[a] && this->layerStack[a]->visible && this->layerStack[a]->data)
			manualBlit(
				this->layerStack[a]->data,
				&this->layerStack[a]->clip_rect,
				this->screenBuffer,
				&this->layerStack[a]->position,
				this->layerStack[a]->alpha);
	if (this->monochrome)
		this->monochrome->call(0,this->screenBuffer,0);
	if (this->negative)
		this->negative->call(0,this->screenBuffer,0);
	if (effect){
		NONS_GFX *e=this->gfx_store->retrieve(effect);
		if (!e)
			return NONS_UNDEFINED_EFFECT;
		return e->call(this->screenBuffer,0,this->screen);
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScreenSpace::BlendNoText(ulong effect,long timing,const std::wstring *rule){
	this->BlendNoText(0);
	return NONS_GFX::callEffect(effect,timing,rule,this->screenBuffer,0,this->screen);
}

ErrorCode NONS_ScreenSpace::BlendOnlyBG(ulong effect){
	SDL_FillRect(this->screenBuffer,0,amask);
	if (!!this->Background && !!this->Background->data)
		manualBlit(this->Background->data,&this->Background->clip_rect,this->screenBuffer,&this->Background->position);
	if (effect){
		NONS_GFX *e=this->gfx_store->retrieve(effect);
		if (!e)
			return NONS_UNDEFINED_EFFECT;
		return e->call(this->screenBuffer,0,this->screen);
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScreenSpace::BlendOnlyBG(ulong effect,long timing,const std::wstring *rule){
	this->BlendOnlyBG(0);
	return NONS_GFX::callEffect(effect,timing,rule,this->screenBuffer,0,this->screen);
}

void NONS_ScreenSpace::clearText(){
	this->output->Clear();
	this->BlendNoCursor(1);
	//SDL_UpdateRect(this->screen,0,0,0,0);
}

void NONS_ScreenSpace::hideText(){
	if (!this->output->visible)
		return;
	this->output->visible=0;
	this->BlendNoCursor(0);
	this->output->transition->call(this->screenBuffer,0,this->screen);
}

void NONS_ScreenSpace::showText(){
	if (this->output->visible)
		return;
	this->output->visible=1;
	this->BlendNoCursor(0);
	this->output->transition->call(this->screenBuffer,0,this->screen);
}

void NONS_ScreenSpace::resetParameters(SDL_Rect *window,SDL_Rect *frame,NONS_Font *font,bool shadow){
	NONS_GFX *temp;
	bool a=this->output->transition->stored;
	if (a)
		temp=this->output->transition;
	else
		temp=new NONS_GFX(*(this->output->transition));
	delete this->output;
	this->output=new NONS_StandardOutput(font,window,frame,shadow);
	delete this->output->transition;
	this->output->transition=temp;
	this->lookback->reset(this->output);
}

ErrorCode NONS_ScreenSpace::loadSprite(ulong n,const std::wstring &string,long x,long y,uchar alpha,bool visibility){
	if (!string[0])
		return NONS_EMPTY_STRING;
	if (n>this->layerStack.size())
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	if (!this->layerStack[n])
		this->layerStack[n]=new NONS_Layer(&string);
	else
		this->layerStack[n]->load(&string);
	if (!this->layerStack[n]->data)
		return NONS_UNDEFINED_ERROR;
	this->layerStack[n]->position.x=x;
	this->layerStack[n]->position.y=y;
	this->layerStack[n]->visible=visibility;
	this->layerStack[n]->alpha=alpha;
	return NONS_NO_ERROR;
}

void NONS_ScreenSpace::clear(){
#define CHECK_AND_DELETE(p) (p)->unload();  //if (!!(p)){ /*delete (p); (p)=0;*/}
	CHECK_AND_DELETE(this->Background);
	CHECK_AND_DELETE(this->leftChar);
	CHECK_AND_DELETE(this->rightChar);
	CHECK_AND_DELETE(this->centerChar);
	for (ulong a=0;a<this->layerStack.size();a++){
		if (this->layerStack[a]){
			CHECK_AND_DELETE(this->layerStack[a]);
		}
	}
	this->clearText();
	this->BlendNoCursor(1);
}

bool NONS_ScreenSpace::advanceAnimations(ulong msecs,std::vector<SDL_Rect> &rects){
	rects.clear();
	bool requireRefresh=0;
	std::vector<NONS_Layer *> arr;
	arr.reserve(5+this->layerStack.size());
	arr.push_back(this->Background);
	arr.push_back(this->leftChar);
	arr.push_back(this->rightChar);
	arr.push_back(this->centerChar);
	for (ulong a=0;a<this->layerStack.size();a++)
		arr.push_back(this->layerStack[a]);
	arr.push_back(this->cursor);
	for (ulong a=0;a<arr.size();a++){
		NONS_Layer *p=arr[a];
		if (p && p->data){
			ulong first=p->animation.getCurrentAnimationFrame();
			bool b=p->advanceAnimation(msecs);
			requireRefresh|=b;
			if (b){
				ulong second=p->animation.getCurrentAnimationFrame();
				SDL_Rect push=p->optimized_updates[std::pair<ulong,ulong>(first,second)];
				push.x+=p->position.x;
				push.y+=p->position.y;
				rects.push_back(push);
			}
		}
	}
	return requireRefresh;
}
#endif
