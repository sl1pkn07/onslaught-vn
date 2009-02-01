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

#ifndef NONS_CURSOR_CPP
#define NONS_CURSOR_CPP

#include "Cursor.h"
#include "../../Functions.h"
#include "../../UTF.h"
#include "../../Globals.h"
#include "ScreenSpace.h"
#include <climits>
#include <vector>

NONS_Cursor::NONS_Cursor(){
	this->data=0;
	this->speed=LONG_MAX;
	this->xpos=0;
	this->ypos=0;
	this->absolute=0;
	this->loop=0;
}

NONS_Cursor::NONS_Cursor(wchar_t *name,long length,long speed,long x,long y,long absolute,short loop,METHODS method){
	this->data=ImageLoader->fetchCursor(name,method);
	if (this->data)
		this->data->clip_rect.w/=length;
	this->speed=speed;
	this->xpos=x;
	this->ypos=y;
	this->absolute=!!absolute;
	this->loop=(loop<0 || loop>2)?0:loop;
}

NONS_Cursor::NONS_Cursor(wchar_t *str,long x,long y,long absolute){
	METHODS method=ALPHA_METHOD;
	//Unused:
	//ErrorCode error=NONS_NO_ERROR;
	this->data=0;
	this->xpos=x;
	this->ypos=y;
	this->absolute=!!absolute;
	long length=3,
		speed=100,
		loop=0;
	if (*str==':'){
		str++;
		for (;*str && iswhitespace((char)*str);str++);
		switch (*str){
			case 'a':
				method=ALPHA_METHOD;
				break;
			case 'l':
			case 'r':
				method=CLASSIC_METHOD;
				break;
			case 'c':
				method=NO_ALPHA;
				break;
			default:
				return;
		}
		for (;*str && *str!='/' && *str!=';';str++);
	}
	if (*str=='/'){
		str++;
		long p=instr(str,L",");
		if (p<0)
			return;
		char *copy=copyString(str,p);
		length=atoi(copy);
		str+=p+1;
		delete[] copy;
		if (length<1)
			return;
		p=instr(str,L",");
		if (p<0)
			return;
		copy=copyString(str,p);
		speed=atoi(copy);
		str+=p+1;
		delete[] copy;
		if (length<1)
			return;
		p=instr(str,L";");
		if (p<0)
			return;
		copy=copyString(str,p);
		loop=atoi(copy);
		str+=p;
		delete[] copy;
		if (length<1)
			return;
		for (;*str && *str!=';';str++);
	}
	if (*str==';')
		str++;
	str=copyWString(str);
	this->data=ImageLoader->fetchCursor(str,method);
	if (this->data)
		this->data->clip_rect.w/=length;
	this->speed=speed;
	this->loop=(loop<0 || loop>2)?0:loop;
}

NONS_Cursor::~NONS_Cursor(){
	if (this->data)
		SDL_FreeSurface(this->data);
}

int NONS_Cursor::animate(NONS_ScreenSpace *screen,NONS_Menu *menu,ulong expiration){
	if (CURRENTLYSKIPPING)
		return 0;
	long w=0,h=0;
	bool anim=!!this->data;
	if (anim){
		w=this->data->clip_rect.w;
		h=this->data->clip_rect.h;
	}
	SDL_Surface *copyDst;
	if (anim)
		copyDst=SDL_CreateRGBSurface(
			SDL_HWSURFACE|SDL_SRCALPHA,
			w,h,32,
			rmask,
			gmask,
			bmask,
			amask);
	SDL_Rect srcRect={0,0,w,h};
	SDL_Rect dstRect;
	if (anim){
		dstRect.x=!this->absolute?
			screen->output->x+this->data->clip_rect.x:
			this->data->clip_rect.x;
		dstRect.y=!this->absolute?
			screen->output->y+this->data->clip_rect.y:
			this->data->clip_rect.y,
		dstRect.w=w;
		dstRect.h=h;
		LOCKSCREEN;
		manualBlit(screen->screen->virtualScreen,&dstRect,copyDst,0);
		UNLOCKSCREEN;
	}
	//Unused:
	//char i=0;
	bool done=0;
	long advance=w;
	NONS_EventQueue *queue=InputObserver.attach();
	const long delayadvance=10;
	ulong expire=expiration?expiration:ULONG_MAX;
	int ret=0;
	while (!done && !CURRENTLYSKIPPING && expire>0){
		if (anim){
			LOCKSCREEN;
			manualBlit(copyDst,0,screen->screen->virtualScreen,&dstRect);
			manualBlit(this->data,&srcRect,screen->screen->virtualScreen,&dstRect);
			UNLOCKSCREEN;
			if (srcRect.x+advance<0 || srcRect.x+advance*2>this->data->w){
				switch (this->loop){
					case 0:
						srcRect.x=0;
						break;
					case 1:
						advance=0;
						break;
					case 2:
						advance=-advance;
				}
			}
			srcRect.x+=advance;
			//SDL_UpdateRect(screen->screen,dstRect.x,dstRect.y,dstRect.w,dstRect.h);
			screen->screen->updateScreen(dstRect.x,dstRect.y,dstRect.w,dstRect.h);
		}
		for (ulong a=0;!done && (a<this->speed || !this->data) && !CURRENTLYSKIPPING && expire>0;a+=delayadvance){
			/*if (!anim)
				NONS_WaitForEvent(queue,25);*/
			while (!queue->data.empty()){
				SDL_Event event=queue->pop();
				switch (event.type){
					case SDL_QUIT:
						break;
					case SDL_KEYDOWN:
						if (!menu)
							break;
						switch (event.key.keysym.sym){
							case SDLK_ESCAPE:
								if (menu && menu->rightClickMode==1 && menu->buttons){
									if (anim){
										LOCKSCREEN;
										manualBlit(copyDst,0,screen->screen->virtualScreen,&dstRect);
										UNLOCKSCREEN;
									}
									if (menu->callMenu()==-1){
										ret=-1;
										goto animate_000;
									}
									while (!queue->data.empty())
										queue->pop();
									//SDL_BlitSurface(copyDst,0,screen->screen,0);
									screen->BlendAll(0);
									LOCKSCREEN;
									manualBlit(screen->screenBuffer,0,screen->screen->virtualScreen,0);
									if (anim)
										manualBlit(this->data,&srcRect,screen->screen->virtualScreen,&dstRect);
									UNLOCKSCREEN;
									//SDL_UpdateRect(screen->screen,0,0,0,0);
									screen->screen->updateWholeScreen();
								}
								break;
							case SDLK_UP:
							case SDLK_PAGEUP:
								{
									screen->BlendNoText(0);
									LOCKSCREEN;
									manualBlit(screen->screenBuffer,0,screen->screen->virtualScreen,0);
									multiplyBlend(screen->output->shadeLayer->data,0,screen->screen->virtualScreen,&(screen->output->shadeLayer->clip_rect));
									UNLOCKSCREEN;
									screen->lookback->callLookback(screen->screen);
									while (!queue->data.empty())
										queue->pop();
									screen->BlendAll(0);
									LOCKSCREEN;
									manualBlit(screen->screenBuffer,0,screen->screen->virtualScreen,0);
									if (anim)
										manualBlit(this->data,&srcRect,screen->screen->virtualScreen,&dstRect);
									UNLOCKSCREEN;
									screen->screen->updateWholeScreen();
									break;
								}
							case SDLK_MENU:
								break;
							default:
								goto animate_000;
						}
						break;
					case SDL_MOUSEBUTTONDOWN:
						done=1;
						break;
					default:
						break;
						//v_stdout <<"animate(): Place holder."<<std::endl;
				}
			}
			//if (anim)
			SDL_Delay(delayadvance);
			expire-=delayadvance;
		}
	}
animate_000:
	if (anim){
		if (ret!=-1){
			LOCKSCREEN;
			manualBlit(copyDst,0,screen->screen->virtualScreen,&dstRect);
			UNLOCKSCREEN;
		}
		SDL_FreeSurface(copyDst);
		if (ret!=-1)
			//SDL_UpdateRect(screen->screen,dstRect.x,dstRect.y,dstRect.w,dstRect.h);
			screen->screen->updateScreen(dstRect.x,dstRect.y,dstRect.w,dstRect.h);
	}
	InputObserver.detach(queue);
	return ret;
}
#endif
