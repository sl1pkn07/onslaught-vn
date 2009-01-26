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

#ifndef NONS_SCREENSPACE_CPP
#define NONS_SCREENSPACE_CPP

#include "ScreenSpace.h"
#include "../../Functions.h"
#include "../../Globals.h"

NONS_ScreenSpace::NONS_ScreenSpace(int framesize,NONS_Font *font,NONS_GFXstore *store){
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
	if (!store)
		this->gfx_store=new NONS_GFXstore();
	else
		this->gfx_store=store;
	this->monochrome=0;
	this->negative=0;
	this->sprite_priority=this->layerStack.size()-1;
	SDL_Color *temp=&this->output->foregroundLayer->fontCache->foreground;
	this->lookback=new NONS_Lookback(this->output,temp->r,temp->g,temp->b,CLOptions.virtualWidth,CLOptions.virtualHeight);
}

NONS_ScreenSpace::NONS_ScreenSpace(SDL_Rect *window,SDL_Rect *frame,NONS_Font *font,bool shadow,NONS_GFXstore *store){
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
	if (!store)
		this->gfx_store=new NONS_GFXstore();
	else
		this->gfx_store=store;
	SDL_Color *temp=&this->output->foregroundLayer->fontCache->foreground;
	this->lookback=new NONS_Lookback(this->output,temp->r,temp->g,temp->b,CLOptions.virtualWidth,CLOptions.virtualHeight);
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
	SDL_FreeSurface(this->screenBuffer);
	delete this->gfx_store;
	if (this->monochrome)
		delete this->monochrome;
	if (this->negative)
		delete this->negative;
	delete this->lookback;
}

ErrorCode NONS_ScreenSpace::BlendAll(ulong effect){
	this->BlendNoText(0);
	if (this->output->visible){
		multiplyBlend(this->output->shadeLayer->data,&(this->output->shadeLayer->clip_rect),this->screenBuffer,&(this->screenBuffer->clip_rect));
		manualBlit(this->output->shadowLayer->data,0,this->screenBuffer,0,this->output->shadowLayer->alpha);
		manualBlit(this->output->foregroundLayer->data,0,this->screenBuffer,0,this->output->foregroundLayer->alpha);
	}
	if (effect){
		NONS_GFX *e=this->gfx_store->retrieve(effect);
		if (!e)
			return NONS_UNDEFINED_EFFECT;
		return e->call(this->screenBuffer,0,this->screen);
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScreenSpace::BlendAll(ulong effect,long timing,wchar_t *rule){
	this->BlendAll(0);
	return NONS_GFX::callEffect(effect,timing,rule,this->screenBuffer,0,this->screen);
}

ErrorCode NONS_ScreenSpace::BlendNoText(ulong effect){
	this->BlendOnlyBG(0);
	for (long a=this->layerStack.size()-1;a>this->sprite_priority;a--)
		if (this->layerStack[a] && this->layerStack[a]->visible && this->layerStack[a]->data)
			manualBlit(this->layerStack[a]->data,0,this->screenBuffer,&(this->layerStack[a]->clip_rect),this->layerStack[a]->alpha);
	if (this->leftChar && this->leftChar->data)
		manualBlit(this->leftChar->data,0,this->screenBuffer,&(this->leftChar->clip_rect),this->leftChar->alpha);
	if (this->rightChar && this->rightChar->data)
		manualBlit(this->rightChar->data,0,this->screenBuffer,&(this->rightChar->clip_rect),this->rightChar->alpha);
	if (this->centerChar && this->centerChar->data)
		manualBlit(this->centerChar->data,0,this->screenBuffer,&(this->centerChar->clip_rect),this->centerChar->alpha);
	for (long a=this->sprite_priority;a>=0;a--)
		if (this->layerStack[a] && this->layerStack[a]->visible && this->layerStack[a]->data)
			manualBlit(this->layerStack[a]->data,0,this->screenBuffer,&(this->layerStack[a]->clip_rect),this->layerStack[a]->alpha);
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

ErrorCode NONS_ScreenSpace::BlendNoText(ulong effect,long timing,wchar_t *rule){
	this->BlendNoText(0);
	return NONS_GFX::callEffect(effect,timing,rule,this->screenBuffer,0,this->screen);
}

ErrorCode NONS_ScreenSpace::BlendOnlyBG(ulong effect){
	SDL_FillRect(this->screenBuffer,0,amask);
	manualBlit(this->Background->data,0,this->screenBuffer,0);
	if (effect){
		NONS_GFX *e=this->gfx_store->retrieve(effect);
		if (!e)
			return NONS_UNDEFINED_EFFECT;
		return e->call(this->screenBuffer,0,this->screen);
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScreenSpace::BlendOnlyBG(ulong effect,long timing,wchar_t *rule){
	this->BlendOnlyBG(0);
	return NONS_GFX::callEffect(effect,timing,rule,this->screenBuffer,0,this->screen);
}

void NONS_ScreenSpace::clearText(){
	this->output->Clear();
	this->BlendAll(1);
	//SDL_UpdateRect(this->screen,0,0,0,0);
}

void NONS_ScreenSpace::hideText(){
	if (!this->output->visible)
		return;
	this->output->visible=0;
	this->BlendAll(0);
	this->output->transition->call(this->screenBuffer,0,this->screen);
}

void NONS_ScreenSpace::showText(){
	if (this->output->visible)
		return;
	this->output->visible=1;
	this->BlendAll(0);
	this->output->transition->call(this->screenBuffer,0,this->screen);
}

/*std::vector<NONS_Glyph *> *NONS_ScreenSpace::NONSOut(wchar_t *str,float center){
	std::vector<NONS_Glyph *> *ret=this->output->Out(str,this->screen);
	return ret;
}*/

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
	this->lookback->output=this->output;
}

ErrorCode NONS_ScreenSpace::loadSprite(ulong n,wchar_t *string,wchar_t *name,long x,long y,uchar alpha,METHODS method,bool visibility){
	if (!name)
		return NONS_EMPTY_STRING;
	if (n>this->layerStack.size())
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	SDL_Surface *s=ImageLoader->fetchSprite(string,name,method);
	if (!s)
		return NONS_UNDEFINED_ERROR;
	if (!this->layerStack[n])
		this->layerStack[n]=new NONS_Layer(s,0);
	else{
		if (this->layerStack[n]->data)
			this->layerStack[n]->unload();
		this->layerStack[n]->data=s;
		this->layerStack[n]->clip_rect=this->layerStack[n]->data->clip_rect;
	}
	this->layerStack[n]->clip_rect.x=x;
	this->layerStack[n]->clip_rect.y=y;
	this->layerStack[n]->visible=visibility;
	this->layerStack[n]->alpha=alpha;
	return NONS_NO_ERROR;
}

/*std::vector<NONS_Glyph *> *NONS_ScreenSpace::NONSOut(std::vector<NONS_Glyph *> *str){
	return this->output->Out(str,0,this->screen);
}*/

void NONS_ScreenSpace::clear(){
	this->Background->unload();
	this->leftChar->unload();
	this->rightChar->unload();
	this->centerChar->unload();
	for (ulong a=0;a<this->layerStack.size();a++)
		if (this->layerStack[a])
			this->layerStack[a]->unload();
	this->clearText();
	this->BlendAll(1);
}
#endif
