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

#ifndef NONS_BUTTONLAYER_CPP
#define NONS_BUTTONLAYER_CPP

#include "ButtonLayer.h"
#include "../../Globals.h"
#include "Menu.h"
#include "../../Functions.h"
#include "../IOFunctions.h"
#include <utility>

NONS_ButtonLayer::NONS_ButtonLayer(NONS_Font *font,NONS_ScreenSpace *screen,bool exitable,NONS_Menu *menu){
	this->font=font;
	this->screen=screen;
	this->exitable=exitable;
	this->menu=menu;
	this->loadedGraphic=0;
	this->inputOptions.btnArea=0;
	this->inputOptions.Cursor=0;
	this->inputOptions.Enter=0;
	this->inputOptions.EscapeSpace=0;
	this->inputOptions.Function=0;
	this->inputOptions.Wheel=0;
	this->inputOptions.Insert=0;
	this->inputOptions.PageUpDown=0;
	this->inputOptions.Tab=0;
	this->inputOptions.ZXC=0;
}

NONS_ButtonLayer::NONS_ButtonLayer(SDL_Surface *img,NONS_ScreenSpace *screen){
	this->loadedGraphic=img;
	this->screen=screen;
	this->inputOptions.btnArea=0;
	this->inputOptions.Cursor=0;
	this->inputOptions.Enter=0;
	this->inputOptions.EscapeSpace=0;
	this->inputOptions.Function=0;
	this->inputOptions.Wheel=0;
	this->inputOptions.Insert=0;
	this->inputOptions.PageUpDown=0;
	this->inputOptions.Tab=0;
	this->inputOptions.ZXC=0;
}

NONS_ButtonLayer::~NONS_ButtonLayer(){
	for (ulong a=0;a<this->buttons.size();a++)
		if (this->buttons[a])
			delete this->buttons[a];
	if (this->loadedGraphic && !ImageLoader->unfetchImage(this->loadedGraphic))
		SDL_FreeSurface(this->loadedGraphic);
}

void NONS_ButtonLayer::makeTextButtons(const std::vector<std::wstring> &arr,
		SDL_Color *on,
		SDL_Color *off,
		bool shadow,
		std::wstring *entry,
		std::wstring *mouseover,
		std::wstring *click,
		NONS_Audio *audio,
		NONS_GeneralArchive *archive,
		int width,
		int height){
	if (!this->font)
		return;
	for (ulong a=0;a<this->buttons.size();a++)
		delete this->buttons[a];
	this->buttons.clear();
	this->archive=archive;
	this->audio=audio;
	if (entry)
		this->voiceEntry=*entry;
	if (click)
		this->voiceClick=*click;
	if (mouseover)
		this->voiceMouseOver=*mouseover;
	this->boundingBox.x=0;
	this->boundingBox.y=0;
	this->boundingBox.w=0;
	this->boundingBox.h=0;
	for (ulong a=0;a<arr.size();a++){
		NONS_Button *button=new NONS_Button(this->font);
		this->buttons.push_back(button);
		button->makeTextButton(arr[a],0,on,off,shadow,width,height);
		this->boundingBox.h+=button->box.h;
		if (button->box.w>this->boundingBox.w)
			this->boundingBox.w=button->box.w;
	}
}

int NONS_ButtonLayer::getUserInput(int x,int y){
	if (!this->buttons.size())
		return -1;
	for (ulong a=0;a<this->buttons.size();a++){
		NONS_Button *cB=this->buttons[a];
		cB->posx=x;
		cB->posy=y;
		y+=cB->box.y+cB->box.h;
	}
	if (y>this->screen->output->y0+this->screen->output->h)
		return -2;
	if (this->voiceEntry.size()){
		if (this->audio->bufferIsLoaded(this->voiceEntry))
			this->audio->playSoundAsync(&this->voiceEntry,0,0,7,0);
		else{
			ulong l;
			char *buffer=(char *)this->archive->getFileBuffer(this->voiceEntry,l);
			if (this->audio->playSoundAsync(&this->voiceClick,buffer,l,7,0)!=NONS_NO_ERROR)
				delete[] buffer;
		}
	}
	if (this->voiceMouseOver.size()){
		if (this->audio->bufferIsLoaded(this->voiceMouseOver)){
			ulong l;
			char *buffer=(char *)this->archive->getFileBuffer(this->voiceMouseOver,l);
			this->audio->loadAsyncBuffer(this->voiceMouseOver,buffer,l,7);
		}
	}
	NONS_EventQueue *queue=InputObserver.attach();
	SDL_Surface *screenCopy=makeSurface(this->screen->screen->inRect.w,this->screen->screen->inRect.h,32);
	LOCKSCREEN;
	manualBlit(this->screen->screen->virtualScreen,0,screenCopy,0);
	UNLOCKSCREEN;
	int mouseOver=-1;
	getCorrectedMousePosition(this->screen->screen,&x,&y);
	for (ulong a=0;a<this->buttons.size();a++){
		NONS_Button *b=this->buttons[a];
		if (b){
			if (b->MouseOver(x,y) && mouseOver<0){
				mouseOver=a;
				b->mergeWithoutUpdate(this->screen->screen,screenCopy,1,1);
			}else
				this->buttons[a]->mergeWithoutUpdate(this->screen->screen,screenCopy,0,1);
		}
	}
	this->screen->screen->updateWholeScreen();
	while (1){
		queue->WaitForEvent(10);
		SDL_Event event=queue->pop();
		//Handle entering to lookback.
		if (event.type==SDL_KEYDOWN && (event.key.keysym.sym==SDLK_UP || event.key.keysym.sym==SDLK_PAGEUP) ||
				event.type==SDL_MOUSEBUTTONDOWN && (event.button.button==SDL_BUTTON_WHEELUP || event.button.button==SDL_BUTTON_WHEELDOWN)){
			this->screen->BlendNoText(0);
			this->screen->screen->blitToScreen(this->screen->screenBuffer,0,0);
			LOCKSCREEN;
			multiplyBlend(
				this->screen->output->shadeLayer->data,
				&(this->screen->output->shadeLayer->clip_rect),
				this->screen->screen->virtualScreen,
				0);
			this->screen->lookback->display(this->screen->screen);
			while (!queue->data.empty())
				queue->pop();
			manualBlit(screenCopy,0,this->screen->screen->virtualScreen,0);
			UNLOCKSCREEN;
			getCorrectedMousePosition(this->screen->screen,&x,&y);
			for (ulong a=0;a<this->buttons.size();a++){
				NONS_Button *b=this->buttons[a];
				if (b){
					if (b->MouseOver(x,y)){
						mouseOver=a;
						b->mergeWithoutUpdate(this->screen->screen,screenCopy,1,1);
					}else
						this->buttons[a]->mergeWithoutUpdate(this->screen->screen,screenCopy,0,1);
				}
			}
			this->screen->screen->updateWholeScreen();
			continue;
		}
		switch (event.type){
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE:
						if (this->exitable){
							InputObserver.detach(queue);
							this->screen->screen->blitToScreen(screenCopy,0,0);
							this->screen->screen->updateWholeScreen();
							SDL_FreeSurface(screenCopy);
							return -1;
						}else if (this->menu){
							this->screen->screen->blitToScreen(screenCopy,0,0);
							int ret=this->menu->callMenu();
							if (ret<0){
								SDL_FreeSurface(screenCopy);
								InputObserver.detach(queue);
								return -3;
							}
							while (!queue->data.empty())
								queue->pop();
							this->screen->screen->blitToScreen(screenCopy,0,0);
							getCorrectedMousePosition(this->screen->screen,&x,&y);
							for (ulong a=0;a<this->buttons.size();a++){
								NONS_Button *b=this->buttons[a];
								if (b){
									if (b->MouseOver(x,y)){
										mouseOver=a;
										b->mergeWithoutUpdate(this->screen->screen,screenCopy,1,1);
									}else
										this->buttons[a]->mergeWithoutUpdate(this->screen->screen,screenCopy,0,1);
								}
							}
							this->screen->screen->updateWholeScreen();
						}
						break;
					//Will never happen:
					/*
					case SDLK_UP:
					case SDLK_PAGEUP:
						{
						}
					*/
					default:
						break;
				}
			case SDL_MOUSEMOTION:
				{
					if (mouseOver>=0 && this->buttons[mouseOver]->MouseOver(&event))
						break;
					int tempMO=-1;
					for (ulong a=0;a<this->buttons.size() && tempMO==-1;a++)
						if (this->buttons[a]->MouseOver(&event))
							tempMO=a;
					if (tempMO<0){
						if (mouseOver>=0)
							this->buttons[mouseOver]->merge(this->screen->screen,screenCopy,0);
						mouseOver=-1;
						break;
					}else{
						if (mouseOver>=0)
							this->buttons[mouseOver]->merge(this->screen->screen,screenCopy,0);
						mouseOver=tempMO;
						this->buttons[mouseOver]->merge(this->screen->screen,screenCopy,1);
						if (this->voiceMouseOver.size()){
							if (this->audio->bufferIsLoaded(this->voiceMouseOver))
								this->audio->playSoundAsync(&this->voiceMouseOver,0,0,7,0);
							else{
								ulong l;
								char *buffer=(char *)this->archive->getFileBuffer(this->voiceMouseOver,l);
								if (this->audio->playSoundAsync(&this->voiceMouseOver,buffer,l,7,0)!=NONS_NO_ERROR)
									delete[] buffer;
							}
						}
						//o_stdout <<"ButtonLayer::getUserInput(): "<<mouseOver<<std::endl;
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				{
					if (mouseOver<0)
						break;
					else{
						if (this->voiceClick.size()){
							if (this->audio->bufferIsLoaded(this->voiceClick))
								this->audio->playSoundAsync(&this->voiceClick,0,0,7,0);
							else{
								ulong l;
								char *buffer=(char *)this->archive->getFileBuffer(this->voiceClick,l);
								if (this->audio->playSoundAsync(&this->voiceClick,buffer,l,7,0)!=NONS_NO_ERROR)
									delete[] buffer;
							}
						}
						InputObserver.detach(queue);
						LOCKSCREEN;
						manualBlit(screenCopy,0,this->screen->screen->virtualScreen,0);
						UNLOCKSCREEN;
						this->screen->screen->updateWholeScreen();
						SDL_FreeSurface(screenCopy);
						return mouseOver;
					}
				}
				break;
		}
	}
}

void NONS_ButtonLayer::addImageButton(ulong index,int posx,int posy,int width,int height,int originX,int originY){
	if (this->buttons.size()<index+1)
		this->buttons.resize(index+1,0);
	NONS_Button *b=new NONS_Button();
	b->makeGraphicButton(this->loadedGraphic,posx,posy,width,height,originX,originY);
	this->buttons[index]=b;
}

ulong NONS_ButtonLayer::countActualButtons(){
	ulong res=0;
	for (ulong a=0;a<this->buttons.size();a++)
		if (this->buttons[a])
			res++;
	return res;
}

int NONS_ButtonLayer::getUserInput(ulong expiration){
	if (!this->countActualButtons()){
		this->addImageButton(0,0,0,this->loadedGraphic->w,this->loadedGraphic->h,0,0);
		//return LONG_MIN;
	}
	NONS_EventQueue *queue=InputObserver.attach();
	LOCKSCREEN;
	SDL_Surface *screenCopy=makeSurface(this->screen->screen->virtualScreen->w,this->screen->screen->virtualScreen->h,32);
	UNLOCKSCREEN;
	LOCKSCREEN;
	manualBlit(this->screen->screen->virtualScreen,0,screenCopy,0);
	UNLOCKSCREEN;
	int mouseOver=-1;
	int x,y;
	getCorrectedMousePosition(this->screen->screen,&x,&y);
	for (ulong a=0;a<this->buttons.size();a++){
		NONS_Button *b=this->buttons[a];
		if (b){
			if (b->MouseOver(x,y) && mouseOver<0){
				mouseOver=a;
				b->mergeWithoutUpdate(this->screen->screen,screenCopy,1,1);
			}else
				this->buttons[a]->mergeWithoutUpdate(this->screen->screen,screenCopy,0,1);
		}
	}
	this->screen->screen->updateWholeScreen();
	long expire=expiration;


	std::map<SDLKey,std::pair<int,bool *> > key_bool_map;
	key_bool_map[SDLK_ESCAPE]=  std::make_pair(-10,&this->inputOptions.EscapeSpace);
	key_bool_map[SDLK_SPACE]=   std::make_pair(-11,&this->inputOptions.EscapeSpace);
	key_bool_map[SDLK_PAGEUP]=  std::make_pair(-12,&this->inputOptions.PageUpDown);
	key_bool_map[SDLK_PAGEDOWN]=std::make_pair(-13,&this->inputOptions.PageUpDown);
	key_bool_map[SDLK_RETURN]=  std::make_pair(-19,&this->inputOptions.Enter);
	key_bool_map[SDLK_TAB]=     std::make_pair(-20,&this->inputOptions.Tab);
	key_bool_map[SDLK_F1]=      std::make_pair(-21,&this->inputOptions.Function);
	key_bool_map[SDLK_F2]=      std::make_pair(-22,&this->inputOptions.Function);
	key_bool_map[SDLK_F3]=      std::make_pair(-23,&this->inputOptions.Function);
	key_bool_map[SDLK_F4]=      std::make_pair(-24,&this->inputOptions.Function);
	key_bool_map[SDLK_F5]=      std::make_pair(-25,&this->inputOptions.Function);
	key_bool_map[SDLK_F6]=      std::make_pair(-26,&this->inputOptions.Function);
	key_bool_map[SDLK_F7]=      std::make_pair(-27,&this->inputOptions.Function);
	key_bool_map[SDLK_F8]=      std::make_pair(-28,&this->inputOptions.Function);
	key_bool_map[SDLK_F9]=      std::make_pair(-29,&this->inputOptions.Function);
	key_bool_map[SDLK_F10]=     std::make_pair(-30,&this->inputOptions.Function);
	key_bool_map[SDLK_F11]=     std::make_pair(-31,&this->inputOptions.Function);
	key_bool_map[SDLK_F12]=     std::make_pair(-32,&this->inputOptions.Function);
	key_bool_map[SDLK_UP]=      std::make_pair(-40,&this->inputOptions.Cursor);
	key_bool_map[SDLK_RIGHT]=   std::make_pair(-41,&this->inputOptions.Cursor);
	key_bool_map[SDLK_DOWN]=    std::make_pair(-42,&this->inputOptions.Cursor);
	key_bool_map[SDLK_LEFT]=    std::make_pair(-43,&this->inputOptions.Cursor);
	key_bool_map[SDLK_INSERT]=  std::make_pair(-50,&this->inputOptions.Insert);
	key_bool_map[SDLK_z]=       std::make_pair(-51,&this->inputOptions.ZXC);
	key_bool_map[SDLK_x]=       std::make_pair(-52,&this->inputOptions.ZXC);
	key_bool_map[SDLK_c]=       std::make_pair(-53,&this->inputOptions.ZXC);

	//Is this the same as while (1)? I have no idea, but don't touch it, just in case.
	while (expiration && expire>0 || !expiration){
		while (!queue->data.empty()){
			SDL_Event event=queue->pop();
			switch (event.type){
				case SDL_MOUSEMOTION:
					{
						if (mouseOver>=0 && this->buttons[mouseOver]->MouseOver(&event))
							break;
						int tempMO=-1;
						for (ulong a=0;a<this->buttons.size() && tempMO==-1;a++)
							if (this->buttons[a] && this->buttons[a]->MouseOver(&event))
								tempMO=a;
						if (tempMO<0){
							if (mouseOver>=0)
								this->buttons[mouseOver]->merge(this->screen->screen,screenCopy,0);
							mouseOver=-1;
							break;
						}else{
							if (mouseOver>=0)
								this->buttons[mouseOver]->merge(this->screen->screen,screenCopy,0);
							mouseOver=tempMO;
							this->buttons[mouseOver]->merge(this->screen->screen,screenCopy,1);
						}
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					{
						int button;
						switch (event.button.button){
							case SDL_BUTTON_LEFT:
								button=1;
								break;
							case SDL_BUTTON_RIGHT:
								button=2;
								break;
							case SDL_BUTTON_WHEELUP:
								button=3;
								break;
							case SDL_BUTTON_WHEELDOWN:
								button=4;
								break;
							default:
								button=-1;
						}
						if (button<0)
							break;
						InputObserver.detach(queue);
						SDL_FreeSurface(screenCopy);
						this->screen->screen->updateWholeScreen();
						if (button==1){
							if (mouseOver<0)
								return -1;
							return mouseOver;
						}else
							return -button;
					}
					break;
				case SDL_KEYDOWN:
					{
						SDLKey key=event.key.keysym.sym;
						std::map<SDLKey,std::pair<int,bool *> >::iterator i=key_bool_map.find(key);
						int ret=0;
						if (i!=key_bool_map.end()){
							if (*(i->second.second))
								ret=i->second.first-1;
							else if (key==SDLK_ESCAPE)
								ret=-2;
						}
						if (ret){
							InputObserver.detach(queue);
							SDL_FreeSurface(screenCopy);
							this->screen->screen->updateWholeScreen();
							return ret;
						}
					}
			}
		}
		SDL_Delay(10);
		expire-=10;
	}
	InputObserver.detach(queue);
	SDL_FreeSurface(screenCopy);
	return (this->inputOptions.Wheel)?-5:-2;
}
#endif
