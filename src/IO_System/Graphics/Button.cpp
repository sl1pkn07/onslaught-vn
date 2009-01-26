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

#ifndef NONS_BUTTON_CPP
#define NONS_BUTTON_CPP

#include "Button.h"
#include "../../Functions.h"
#include "../../UTF.h"
#include "../../Globals.h"
#include <climits>

NONS_Button::NONS_Button(){
	this->offLayer=0;
	this->onLayer=0;
	this->shadowLayer=0;
	this->box.x=0;
	this->box.y=0;
	this->box.h=0;
	this->box.w=0;
	this->font=0;
	this->status=0;
	this->posx=0;
	this->posy=0;
}

NONS_Button::NONS_Button(NONS_Font *font){
	this->offLayer=0;
	this->onLayer=0;
	this->shadowLayer=0;
	this->box.x=0;
	this->box.y=0;
	this->box.h=0;
	this->box.w=0;
	this->font=font;
	this->status=0;
	this->posx=0;
	this->posy=0;
}

NONS_Button::~NONS_Button(){
	if (this->offLayer)
		delete this->offLayer;
	if (this->onLayer)
		delete this->onLayer;
	if (this->shadowLayer)
		delete this->shadowLayer;
}

void NONS_Button::makeTextButton(wchar_t *text,float center,SDL_Color *on,SDL_Color *off,bool shadow,int limitX,int limitY){
	SDL_Color black={0,0,0,0};
	NONS_FontCache *tempCache=new NONS_FontCache(this->font,&black,0);
	this->limitX=limitX;
	this->limitY=limitY;
	this->box=this->GetBoundingBox(text,tempCache,limitX,limitY);
	delete tempCache;
	this->offLayer=new NONS_Layer(&this->box,0);
	this->offLayer->MakeTextLayer(this->font,off,0);
	this->onLayer=new NONS_Layer(&this->box,0);
	this->onLayer->MakeTextLayer(this->font,on,0);
	if (shadow){
		this->shadowLayer=new NONS_Layer(&this->box,0);
		this->shadowLayer->MakeTextLayer(this->font,&black,1);
	}
	this->write(text,center);
}

SDL_Rect NONS_Button::GetBoundingBox(wchar_t *str,NONS_FontCache *cache,int limitX,int limitY){
	std::vector<NONS_Glyph *> outputBuffer;
	long lastSpace=-1;
	int x0=0,y0=0;
	int wordL=0;
	int width=0,minheight=INT_MAX,height=0;
	int lineSkip=this->font->lineSkip;
	int fontLineSkip=this->font->fontLineSkip;
	if (!cache)
		cache=this->offLayer->fontCache;
	SDL_Rect frame={0,0,this->limitX,this->limitY};
	for (wchar_t *str2=str;;str2++){
		NONS_Glyph *glyph=cache->getGlyph(*str2);
		if (*str2=='\n'){
			outputBuffer.push_back(0);
			if (x0+wordL>=frame.w && lastSpace>=0){
				if (isbreakspace(outputBuffer[lastSpace]->getcodePoint()))
					outputBuffer[lastSpace]=0;
				else
					outputBuffer.insert(outputBuffer.begin()+lastSpace+1,0);
					//insertAfter<NONS_Glyph *>(0,&outputBuffer,lastSpace);
				lastSpace=-1;
				x0=0;
				y0+=lineSkip;
			}
			lastSpace=-1;
			//x0=this->x0;
			x0=0;
			y0+=lineSkip;
			wordL=0;
		}else if (isbreakspace(*str2)){
			if (x0+wordL>=frame.w && lastSpace>=0){
				if (isbreakspace(outputBuffer[lastSpace]->getcodePoint()))
					outputBuffer[lastSpace]=0;
				else
					outputBuffer.insert(outputBuffer.begin()+lastSpace+1,0);
					//insertAfter<NONS_Glyph *>(0,&outputBuffer,lastSpace);
				lastSpace=-1;
				x0=0;
				y0+=lineSkip;
			}
			x0+=wordL;
			lastSpace=outputBuffer.size();
			wordL=glyph->getadvance();
			outputBuffer.push_back(glyph);
		}else if (*str2){
			wordL+=glyph->getadvance();
			outputBuffer.push_back(glyph);
		}else{
			if (x0+wordL>=frame.w && lastSpace>=0)
				outputBuffer[lastSpace]=0;
			break;
		}
	}
	x0=0;
	y0=0;
	for (ulong a=0;a<outputBuffer.size();a++){
		if (!outputBuffer[a]){
			if (x0>width)
				width=x0;
			x0=0;
			y0+=lineSkip;
			continue;
		}
		SDL_Rect tempRect=outputBuffer[a]->getbox();
		int temp=tempRect.y+tempRect.h;
		if (height<temp)
			height=temp;
		if (tempRect.y<minheight)
			minheight=tempRect.y;
		int advance=outputBuffer[a]->getadvance();
		if (x0+advance>frame.w){
			if (x0>width)
				width=x0;
			x0=0;
			y0+=lineSkip;
		}
		x0+=advance;
	}
	if (x0>width)
		width=x0;
	SDL_Rect res={0,0,width,y0+fontLineSkip/*-minheight*/};
	return res;
}

void NONS_Button::write(wchar_t *str,float center){
	std::vector<NONS_Glyph *> outputBuffer;
	std::vector<NONS_Glyph *> outputBuffer2;
	std::vector<NONS_Glyph *> outputBuffer3;
	long lastSpace=-1;
	int x0=0,y0=0;
	int wordL=0;
	SDL_Rect frame={0,-this->box.y,this->box.w,this->box.h};
	int lineSkip=this->offLayer->fontCache->font->lineSkip;
	SDL_Rect screenFrame={0,0,this->limitX,this->limitY};
	for (wchar_t *str2=str;;str2++){
		NONS_Glyph *glyph=this->offLayer->fontCache->getGlyph(*str2);
		NONS_Glyph *glyph2=this->onLayer->fontCache->getGlyph(*str2);
		NONS_Glyph *glyph3=0;
		if (this->shadowLayer)
			glyph3=this->shadowLayer->fontCache->getGlyph(*str2);
		else
			glyph3=0;
		if (*str2=='\n'){
			outputBuffer.push_back(0);
			outputBuffer2.push_back(0);
			outputBuffer3.push_back(0);
			if (x0+wordL>=screenFrame.w && lastSpace>=0){
				if (isbreakspace(outputBuffer[lastSpace]->getcodePoint())){
					outputBuffer[lastSpace]=0;
					outputBuffer2[lastSpace]=0;
					outputBuffer3[lastSpace]=0;
				}else{
					outputBuffer.insert(outputBuffer.begin()+lastSpace+1,0);
					outputBuffer2.insert(outputBuffer2.begin()+lastSpace+1,0);
					outputBuffer3.insert(outputBuffer3.begin()+lastSpace+1,0);
					/*insertAfter<NONS_Glyph *>(0,&outputBuffer,lastSpace);
					insertAfter<NONS_Glyph *>(0,&outputBuffer2,lastSpace);
					insertAfter<NONS_Glyph *>(0,&outputBuffer3,lastSpace);*/
				}
				lastSpace=-1;
				y0+=lineSkip;
			}
			lastSpace=-1;
			x0=0;
			y0+=lineSkip;
			wordL=0;
		}else if (isbreakspace(*str2)){
			if (x0+wordL>=screenFrame.w && lastSpace>=0){
				if (isbreakspace(outputBuffer[lastSpace]->getcodePoint())){
					outputBuffer[lastSpace]=0;
					outputBuffer2[lastSpace]=0;
					outputBuffer3[lastSpace]=0;
				}else{
					outputBuffer.insert(outputBuffer.begin()+lastSpace+1,0);
					outputBuffer2.insert(outputBuffer2.begin()+lastSpace+1,0);
					outputBuffer3.insert(outputBuffer3.begin()+lastSpace+1,0);
					/*insertAfter<NONS_Glyph *>(0,&outputBuffer,lastSpace);
					insertAfter<NONS_Glyph *>(0,&outputBuffer2,lastSpace);
					insertAfter<NONS_Glyph *>(0,&outputBuffer3,lastSpace);*/
				}
				lastSpace=-1;
				x0=0;
				y0+=lineSkip;
			}
			x0+=wordL;
			lastSpace=outputBuffer.size();
			wordL=glyph->getadvance();
			outputBuffer.push_back(glyph);
			outputBuffer2.push_back(glyph2);
			outputBuffer3.push_back(glyph3);
		}else if (*str2){
			wordL+=glyph->getadvance();
			outputBuffer.push_back(glyph);
			outputBuffer2.push_back(glyph2);
			outputBuffer3.push_back(glyph3);
		}else{
			if (x0+wordL>=screenFrame.w && lastSpace>=0){
				outputBuffer[lastSpace]=0;
				outputBuffer2[lastSpace]=0;
				outputBuffer3[lastSpace]=0;
			}
			break;
		}
	}
	x0=this->setLineStart(&outputBuffer,0,&screenFrame,center);
	y0=0;
	for (ulong a=0;a<outputBuffer.size();a++){
		if (!outputBuffer[a]){
			x0=this->setLineStart(&outputBuffer,a,&screenFrame,center);
			y0+=lineSkip;
			continue;
		}
		int advance=outputBuffer[a]->getadvance();
		if (x0+advance>screenFrame.w){
			x0=this->setLineStart(&outputBuffer,a,&frame,center);
			y0+=lineSkip;
		}
		outputBuffer[a]->putGlyph(this->offLayer->data,x0,y0,&(this->offLayer->fontCache->foreground),1);
		outputBuffer2[a]->putGlyph(this->onLayer->data,x0,y0,&(this->onLayer->fontCache->foreground),1);
		if (this->shadowLayer)
			outputBuffer3[a]->putGlyph(this->shadowLayer->data,x0,y0,&(this->shadowLayer->fontCache->foreground),1);
		x0+=advance;
	}
}

int NONS_Button::setLineStart(std::vector<NONS_Glyph *> *arr,long start,SDL_Rect *frame,float center){
	while (!(*arr)[start])
		start++;
	int width=this->predictLineLength(arr,start,frame->w);
	float factor=(center<=.5)?center:1.0-center;
	int pixelcenter=float(frame->w)*factor;
	return (width/2>pixelcenter)?(frame->w-width)*(center>.5):frame->w*center-width/2;
}

int NONS_Button::predictLineLength(std::vector<NONS_Glyph *> *arr,long start,int width){
	int res=0;
	for (ulong a=start;a<arr->size() && (*arr)[a] && res+(*arr)[a]->getadvance()<=width;a++)
		res+=(*arr)[a]->getadvance();
	return res;
}

void NONS_Button::makeGraphicButton(SDL_Surface *src,int posx,int posy,int width,int height,int originX,int originY){
	SDL_Rect dst={0,0,width,height},
		srcRect={originX,originY,width,height};
	this->onLayer=new NONS_Layer(&dst,0);
	manualBlit(src,&srcRect,this->onLayer->data,&dst);
	this->posx=posx;
	this->posy=posy;
	this->box.w=width;
	this->box.h=height;
}

void NONS_Button::mergeWithoutUpdate(NONS_VirtualScreen *dst,SDL_Surface *original,bool status,bool force){
	if (!force && this->status==status)
		return;
	SDL_Rect rect=this->box;
	rect.x=this->posx;
	rect.y=this->posy;
	LOCKSCREEN;
	manualBlit(original,&rect,dst->virtualScreen,&rect);
	if (this->shadowLayer){
		rect.x++;
		rect.y++;
		manualBlit(this->shadowLayer->data,0,dst->virtualScreen,&rect);
		rect.x--;
		rect.y--;
	}
	this->status=status;
	if (this->status)
		manualBlit(this->onLayer->data,0,dst->virtualScreen,&rect);
	else if (this->offLayer)
		manualBlit(this->offLayer->data,0,dst->virtualScreen,&rect);
	UNLOCKSCREEN;
}

void NONS_Button::merge(NONS_VirtualScreen *dst,SDL_Surface *original,bool status,bool force){
	if (!force && this->status==status)
		return;
	SDL_Rect rect=this->box;
	rect.x=this->posx;
	rect.y=this->posy;
	this->mergeWithoutUpdate(dst,original,status,force);
	dst->updateScreen(
		rect.x,
		rect.y,
		(rect.w+rect.x>dst->virtualScreen->w)?(dst->virtualScreen->w-rect.x):(rect.w),
		(rect.h+rect.y>dst->virtualScreen->h)?(dst->virtualScreen->h-rect.y):(rect.h));
}

bool NONS_Button::MouseOver(SDL_Event *event){
	if (event->type!=SDL_MOUSEMOTION)
		return 0;
	int x=event->motion.x,y=event->motion.y;
	int startx=this->posx+this->box.x,starty=this->posy+this->box.y;
	return (x>=startx && x<=startx+this->box.w && y>=starty && y<=starty+this->box.h);
}

bool NONS_Button::MouseOver(int x,int y){
	int startx=this->posx+this->box.x,starty=this->posy+this->box.y;
	return (x>=startx && x<=startx+this->box.w && y>=starty && y<=starty+this->box.h);
}
#endif
