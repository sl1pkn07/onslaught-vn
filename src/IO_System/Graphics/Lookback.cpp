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

#include "Lookback.h"
#include "Button.h"
#include "../IOFunctions.h"

NONS_Lookback::NONS_Lookback(NONS_StandardOutput *output,uchar r,uchar g,uchar b){
	this->output=output;
	this->foreground.r=r;
	this->foreground.g=g;
	this->foreground.b=b;
	this->up=new NONS_Button();
	this->down=new NONS_Button();
	SDL_Rect temp=this->output->foregroundLayer->clip_rect;
	int thirdofscreen=temp.h/3;
	temp.y=thirdofscreen;
	((NONS_Button *)this->up)->onLayer=new NONS_Layer(&temp,0);
	((NONS_Button *)this->up)->offLayer=new NONS_Layer(&temp,0);
	((NONS_Button *)this->down)->onLayer=new NONS_Layer(&temp,0);
	((NONS_Button *)this->down)->offLayer=new NONS_Layer(&temp,0);
	((NONS_Button *)this->down)->posy=thirdofscreen*2;
	((NONS_Button *)this->up)->box=temp;
	((NONS_Button *)this->up)->box.x=0;
	((NONS_Button *)this->up)->box.y=0;
	((NONS_Button *)this->down)->box=temp;
	((NONS_Button *)this->down)->box.x=0;
	((NONS_Button *)this->down)->box.y=0;
	this->sUpon=0;
	this->sUpoff=0;
	this->sDownon=0;
	this->sDownoff=0;
}

NONS_Lookback::~NONS_Lookback(){
	delete (NONS_Button *)this->up;
	delete (NONS_Button *)this->down;
	if (!!this->sUpon)
		SDL_FreeSurface(this->sUpon);
	if (!!this->sUpoff)
		SDL_FreeSurface(this->sUpoff);
	if (!!this->sDownon)
		SDL_FreeSurface(this->sDownon);
	if (!!this->sDownoff)
		SDL_FreeSurface(this->sDownoff);
}

bool NONS_Lookback::setUpButtons(const std::wstring &upon,const std::wstring &upoff,const std::wstring &downon,const std::wstring &downoff){
	SDL_Surface *temp0=ImageLoader->fetchSprite(upon),
		*temp1=ImageLoader->fetchSprite(upoff),
		*temp2=ImageLoader->fetchSprite(downon),
		*temp3=ImageLoader->fetchSprite(downoff);
	if (!temp0 || !temp1 || !temp2 || !temp3){
		if (!!temp0)
			SDL_FreeSurface(temp0);
		if (!!temp1)
			SDL_FreeSurface(temp1);
		if (!!temp2)
			SDL_FreeSurface(temp2);
		if (!!temp3)
			SDL_FreeSurface(temp3);
		return 0;
	}
	this->sUpon=temp0;
	this->sUpoff=temp1;
	this->sDownon=temp2;
	this->sDownoff=temp3;
	SDL_Rect src={0,0,this->sUpon->w,this->sUpon->h},
		dst={((NONS_Button *)this->up)->onLayer->clip_rect.w-this->sUpon->w,0,0,0};
	manualBlit(this->sUpon,&src,((NONS_Button *)this->up)->onLayer->data,&dst);
	dst.x=((NONS_Button *)this->up)->onLayer->clip_rect.w-this->sUpoff->w;
	manualBlit(this->sUpoff,&src,((NONS_Button *)this->up)->offLayer->data,&dst);
	dst.x=((NONS_Button *)this->down)->onLayer->clip_rect.w-this->sDownon->w;
	dst.y=((NONS_Button *)this->down)->onLayer->clip_rect.h-this->sDownon->h;
	manualBlit(this->sDownon,&src,((NONS_Button *)this->down)->onLayer->data,&dst);
	dst.x=((NONS_Button *)this->down)->offLayer->clip_rect.w-this->sDownoff->w;
	dst.y=((NONS_Button *)this->down)->offLayer->clip_rect.h-this->sDownoff->h;
	manualBlit(this->sDownoff,&src,((NONS_Button *)this->down)->offLayer->data,&dst);
	return 1;
}

void NONS_Lookback::reset(NONS_StandardOutput *output){
	delete (NONS_Button *)this->up;
	delete (NONS_Button *)this->down;
	this->output=output;
	SDL_Rect temp=this->output->foregroundLayer->clip_rect;
	int thirdofscreen=temp.h/3;
	temp.h=thirdofscreen;
	this->up=new NONS_Button();
	this->down=new NONS_Button();
	((NONS_Button *)this->up)->onLayer=new NONS_Layer(&temp,0);
	((NONS_Button *)this->up)->offLayer=new NONS_Layer(&temp,0);
	((NONS_Button *)this->down)->onLayer=new NONS_Layer(&temp,0);
	((NONS_Button *)this->down)->offLayer=new NONS_Layer(&temp,0);
	((NONS_Button *)this->down)->posy=thirdofscreen*2;
	((NONS_Button *)this->up)->box=temp;
	((NONS_Button *)this->up)->box.x=0;
	((NONS_Button *)this->up)->box.y=0;
	((NONS_Button *)this->down)->box=temp;
	((NONS_Button *)this->down)->box.x=0;
	((NONS_Button *)this->down)->box.y=0;
	((NONS_Button *)this->up)->posx+=temp.x;
	((NONS_Button *)this->up)->posy+=temp.y;
	((NONS_Button *)this->down)->posx+=temp.x;
	((NONS_Button *)this->down)->posy+=temp.y;
	if (!this->sUpon)
		return;
	SDL_Rect src={0,0,this->sUpon->w,this->sUpon->h},
		dst={((NONS_Button *)this->up)->onLayer->clip_rect.w-this->sUpon->w,0,0,0};
	manualBlit(this->sUpon,&src,((NONS_Button *)this->up)->onLayer->data,&dst);
	dst.x=((NONS_Button *)this->up)->onLayer->clip_rect.w-this->sUpoff->w;
	manualBlit(this->sUpoff,&src,((NONS_Button *)this->up)->offLayer->data,&dst);
	//((NONS_Button *)this->up)->posx=((NONS_Button *)this->up)->onLayer->clip_rect.x;
	//((NONS_Button *)this->up)->posy=((NONS_Button *)this->up)->onLayer->clip_rect.y;
	dst.x=((NONS_Button *)this->down)->onLayer->clip_rect.w-this->sDownon->w;
	dst.y=((NONS_Button *)this->down)->onLayer->clip_rect.h-this->sDownon->h;
	manualBlit(this->sDownon,&src,((NONS_Button *)this->down)->onLayer->data,&dst);
	dst.x=((NONS_Button *)this->down)->offLayer->clip_rect.w-this->sDownoff->w;
	dst.y=((NONS_Button *)this->down)->offLayer->clip_rect.h-this->sDownoff->h;
	manualBlit(this->sDownoff,&src,((NONS_Button *)this->down)->offLayer->data,&dst);
	//((NONS_Button *)this->down)->posx=((NONS_Button *)this->down)->onLayer->clip_rect.x;
	//((NONS_Button *)this->down)->posy=((NONS_Button *)this->down)->onLayer->clip_rect.y/*+thirdofscreen*2*/;
}

void NONS_Lookback::display(NONS_VirtualScreen *dst){
	if (!this->output->log.size())
		return;
	NONS_EventQueue queue;
	SDL_Surface *copyDst=makeSurface(dst->virtualScreen->w,dst->virtualScreen->h,32);
	SDL_Surface *preBlit=makeSurface(dst->virtualScreen->w,dst->virtualScreen->h,32);
	manualBlit(dst->virtualScreen,0,copyDst,0);
	long end=this->output->log.size(),
		currentPage=end-1;
	this->output->ephemeralOut(&this->output->log[currentPage],dst,0,0,&this->foreground);
	manualBlit(dst->virtualScreen,0,preBlit,0);
	int mouseOver=-1;
	int x,y;
	uchar visibility=(!!currentPage)<<1;
	if (visibility){
		getCorrectedMousePosition(dst,&x,&y);
		if (((NONS_Button *)this->up)->MouseOver(x,y)){
			mouseOver=0;
			((NONS_Button *)this->up)->mergeWithoutUpdate(dst,preBlit,1,1);
		}else{
			mouseOver=-1;
			((NONS_Button *)this->up)->mergeWithoutUpdate(dst,preBlit,0,1);
		}
	}else
		mouseOver=-1;
	dst->updateWholeScreen();
	while (1){
		queue.WaitForEvent(10);
		while (!queue.empty()){
			SDL_Event event=queue.pop();
			switch (event.type){
				case SDL_MOUSEMOTION:
					{
						if (visibility){
							if (visibility&2 && ((NONS_Button *)this->up)->MouseOver(&event)){
								if (visibility&1)
									((NONS_Button *)this->down)->merge(dst,preBlit,0);
								mouseOver=0;
								((NONS_Button *)this->up)->merge(dst,preBlit,1);
							}else{
								if (visibility&2)
									((NONS_Button *)this->up)->merge(dst,preBlit,0);
								if (visibility&1 && ((NONS_Button *)this->down)->MouseOver(&event)){
									mouseOver=1;
									((NONS_Button *)this->down)->merge(dst,preBlit,1);
								}else{
									mouseOver=-1;
									if (visibility&1)
										((NONS_Button *)this->down)->merge(dst,preBlit,0);
								}
							}
						}
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					{
						if (event.button.button==SDL_BUTTON_LEFT){
							if (mouseOver<0 || !visibility)
								break;
							manualBlit(copyDst,0,dst->virtualScreen,0);
							int dir;
							if (!mouseOver)
								dir=-1;
							else
								dir=1;
							if (!this->changePage(dir,currentPage,copyDst,dst,preBlit,visibility,mouseOver))
								goto callLookback_000;
						}else if (event.button.button==SDL_BUTTON_WHEELUP || event.button.button==SDL_BUTTON_WHEELDOWN){
							int dir;
							if (event.button.button==SDL_BUTTON_WHEELUP)
								dir=-1;
							else
								dir=1;
							if (!this->changePage(dir,currentPage,copyDst,dst,preBlit,visibility,mouseOver))
								goto callLookback_000;
						}
					}
					break;
				case SDL_KEYDOWN:
					{
						int dir=0;
						switch (event.key.keysym.sym){
							case SDLK_UP:
							case SDLK_PAGEUP:
								dir=-1;
								break;
							case SDLK_DOWN:
							case SDLK_PAGEDOWN:
								dir=1;
								break;
							case SDLK_ESCAPE:
								dir=end-currentPage;
							default:
								break;
						}
						if (!this->changePage(dir,currentPage,copyDst,dst,preBlit,visibility,mouseOver))
							goto callLookback_000;
					}
					break;
			}
		}
		SDL_Delay(10);
	}
callLookback_000:
	SDL_FreeSurface(copyDst);
	SDL_FreeSurface(preBlit);
}

bool NONS_Lookback::changePage(int dir,long &currentPage,SDL_Surface *copyDst,NONS_VirtualScreen *dst,SDL_Surface *preBlit,uchar &visibility,int &mouseOver){
	long end=this->output->log.size();
	if (!dir || -dir>currentPage)
		return 1;
	manualBlit(copyDst,0,dst->virtualScreen,0);
	currentPage+=dir;
	if (currentPage==end)
		return 0;
	this->output->ephemeralOut(&this->output->log[currentPage],dst,0,0,&this->foreground);
	manualBlit(dst->virtualScreen,0,preBlit,0);
	visibility=(!!currentPage<<1)|(currentPage!=end-1);
	int x,y;
	getCorrectedMousePosition(dst,&x,&y);
	if (visibility){
		if (visibility&2 && ((NONS_Button *)this->up)->MouseOver(x,y)){
			((NONS_Button *)this->down)->mergeWithoutUpdate(dst,preBlit,0,1);
			mouseOver=0;
			((NONS_Button *)this->up)->mergeWithoutUpdate(dst,preBlit,1,1);
		}else{
			if (visibility&2)
				((NONS_Button *)this->up)->mergeWithoutUpdate(dst,preBlit,0,1);
			if (visibility&1 && ((NONS_Button *)this->down)->MouseOver(x,y)){
				mouseOver=1;
				((NONS_Button *)this->down)->mergeWithoutUpdate(dst,preBlit,1,1);
			}else{
				mouseOver=-1;
				if (visibility&1)
					((NONS_Button *)this->down)->mergeWithoutUpdate(dst,preBlit,0,1);
			}
		}
	}
	//SDL_UpdateRect(dst,0,0,0,0);
	dst->updateWholeScreen();
	return 1;
}
