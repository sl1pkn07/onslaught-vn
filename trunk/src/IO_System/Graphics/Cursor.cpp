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

NONS_Cursor::NONS_Cursor(NONS_ScreenSpace *screen){
	this->data=0;
	this->xpos=0;
	this->ypos=0;
	this->absolute=0;
	this->screen=screen;
}

NONS_Cursor::NONS_Cursor(const std::wstring &str,long x,long y,long absolute,NONS_ScreenSpace *screen){
	this->data=0;
	this->xpos=x;
	this->ypos=y;
	this->absolute=!!absolute;
	this->screen=screen;
	this->data=new NONS_Layer(&str);
}

NONS_Cursor::~NONS_Cursor(){
	if (this->data)
		delete this->data;
}

int NONS_Cursor::animate(NONS_Menu *menu,ulong expiration){
	if (CURRENTLYSKIPPING)
		return 0;
	this->screen->cursor=this->data;
	this->data->position.x=this->xpos+(!this->absolute)?this->screen->output->x:0;
	this->data->position.y=this->ypos+(!this->absolute)?this->screen->output->y:0;
	bool done=0;
	NONS_EventQueue *queue=InputObserver.attach();
	const long delayadvance=25;
	ulong expire=expiration?expiration:ULONG_MAX;
	int ret=0;
	std::vector<SDL_Rect> rects;
	if (this->data->animated())
		this->screen->BlendAll(1);
	while (!done && !CURRENTLYSKIPPING && expire>0){
		for (ulong a=0;!done && !CURRENTLYSKIPPING && expire>0;a+=delayadvance){
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
								if (!this->callMenu(menu,queue)){
									ret=-1;
									goto animate_000;
								}
								break;
							case SDLK_UP:
							case SDLK_PAGEUP:
								{
									this->callLookback(queue);
									break;
								}
							case SDLK_MENU:
								break;
							default:
								goto animate_000;
						}
						break;
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button==SDL_BUTTON_RIGHT){
							if (!this->callMenu(menu,queue)){
								ret=-1;
								goto animate_000;
							}
						}else if (event.button.button==SDL_BUTTON_WHEELUP)
							this->callLookback(queue);
						else
							done=1;
						break;
					default:
						break;
				}
			}
			ulong t0=SDL_GetTicks();
			if (!!this->screen && this->screen->advanceAnimations(delayadvance,rects))
				this->screen->BlendOptimized(rects);
			ulong t1=SDL_GetTicks()-t0;
			long delay=delayadvance-t1;
			if (delay>0)
				SDL_Delay(delay);
			expire-=delayadvance;
		}
	}
animate_000:
	if (ret!=-1){
		this->screen->BlendNoCursor(1);
		this->screen->cursor=0;
	}
	InputObserver.detach(queue);
	return ret;
}

bool NONS_Cursor::callMenu(NONS_Menu *menu,NONS_EventQueue *queue){
	if (menu && menu->rightClickMode==1 && menu->buttons){
		//this->screen->BlendNoText(1);
		if (menu->callMenu()==-1)
			return 0;
		while (!queue->data.empty())
			queue->pop();
		if (this->data->animated())
			this->screen->BlendAll(1);
		else
			this->screen->BlendNoCursor(1);
	}
	return 1;
}

void NONS_Cursor::callLookback(NONS_EventQueue *queue){
	screen->BlendNoText(0);
	LOCKSCREEN;
	manualBlit(screen->screenBuffer,0,screen->screen->virtualScreen,0);
	multiplyBlend(screen->output->shadeLayer->data,0,screen->screen->virtualScreen,&(screen->output->shadeLayer->clip_rect));
	UNLOCKSCREEN;
	screen->lookback->display(screen->screen);
	while (!queue->data.empty())
		queue->pop();
	if (this->data->animated())
		screen->BlendAll(1);
	else
		this->screen->BlendNoCursor(1);
}
#endif
