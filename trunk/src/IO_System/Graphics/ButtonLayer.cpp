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

NONS_ButtonLayer::NONS_ButtonLayer(NONS_Font *font,NONS_ScreenSpace *screen,bool exitable,void *menu){
	this->font=font;
	this->screen=screen;
	this->exitable=exitable;
	this->menu=menu;
	this->loadedGraphic=0;
}

NONS_ButtonLayer::NONS_ButtonLayer(SDL_Surface *img,NONS_ScreenSpace *screen){
	this->loadedGraphic=img;
	this->screen=screen;
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
	SDL_Surface *screenCopy=SDL_CreateRGBSurface(
		SDL_HWSURFACE|SDL_SRCALPHA,
		this->screen->screen->inRect.w,
		this->screen->screen->inRect.h,
		32,
		rmask,
		gmask,
		bmask,
		amask);
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
	//SDL_UpdateRect(this->screen->screen,0,0,0,0);
	this->screen->screen->updateWholeScreen();
	NONS_Menu *tempMenu=(NONS_Menu *)this->menu;
	while (1){
		queue->WaitForEvent(10);
		SDL_Event event=queue->pop();
		switch (event.type){
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE:
						if (this->exitable){
							InputObserver.detach(queue);
							//SDL_BlitSurface(screenCopy,0,this->screen->screen,0);
							this->screen->screen->blitToScreen(screenCopy,0,0);
							//SDL_UpdateRect(this->screen->screen,0,0,0,0);
							this->screen->screen->updateWholeScreen();
							SDL_FreeSurface(screenCopy);
							return -1;
						}else if (tempMenu){
							//SDL_BlitSurface(screenCopy,0,this->screen->screen,0);
							this->screen->screen->blitToScreen(screenCopy,0,0);
							int ret=tempMenu->callMenu();
							if (ret<0){
								SDL_FreeSurface(screenCopy);
								InputObserver.detach(queue);
								return -3;
							}
							while (!queue->data.empty())
								queue->pop();
							//SDL_BlitSurface(screenCopy,0,this->screen->screen,0);
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
							//SDL_UpdateRect(this->screen->screen,0,0,0,0);
							this->screen->screen->updateWholeScreen();
						}
						break;
					case SDLK_UP:
					case SDLK_PAGEUP:
						{
							this->screen->BlendNoText(0);
							//manualBlit(this->screen->screenBuffer,0,this->screen->screen,0);
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
							//SDL_UpdateRect(this->screen->screen,0,0,0,0);
							this->screen->screen->updateWholeScreen();
						}
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
						//SDL_UpdateRect(this->screen->screen,0,0,0,0);
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
	SDL_Surface *screenCopy=SDL_CreateRGBSurface(
		SDL_HWSURFACE|SDL_SRCALPHA,
		this->screen->screen->virtualScreen->w,
		this->screen->screen->virtualScreen->h,
		32,
		rmask,
		gmask,
		bmask,
		amask);
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
	//SDL_UpdateRect(this->screen->screen,0,0,0,0);
	this->screen->screen->updateWholeScreen();
	long expire=expiration;
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
						if (event.button.button!=SDL_BUTTON_LEFT && event.button.button!=SDL_BUTTON_RIGHT)
							break;
						InputObserver.detach(queue);
						/*LOCKSCREEN;
						manualBlit(screenCopy,0,this->screen->screen->virtualScreen,0);
						UNLOCKSCREEN;*/
						SDL_FreeSurface(screenCopy);
						this->screen->screen->updateWholeScreen();
						if (event.button.button==SDL_BUTTON_RIGHT)
							return -2;
						if (mouseOver<0)
							return -1;
						else
							return mouseOver;
					}
					break;
			}
		}
		SDL_Delay(10);
		expire-=10;
	}
	InputObserver.detach(queue);
	/*LOCKSCREEN;
	manualBlit(screenCopy,0,this->screen->screen->virtualScreen,0);
	UNLOCKSCREEN;*/
	SDL_FreeSurface(screenCopy);
	return -2;
}
#endif
