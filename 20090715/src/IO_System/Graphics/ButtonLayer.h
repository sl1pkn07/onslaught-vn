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

#ifndef NONS_BUTTONLAYER_H
#define NONS_BUTTONLAYER_H

#include "../../Common.h"
#include <vector>
#include "Button.h"
#include "ScreenSpace.h"
#include "../Audio/Audio.h"
#include "../SAR/GeneralArchive.h"

struct NONS_ButtonLayer{
	std::vector<NONS_Button *> buttons;
	NONS_Font *font;
	NONS_ScreenSpace *screen;
	std::wstring voiceEntry;
	std::wstring voiceMouseOver;
	std::wstring voiceClick;
	NONS_Audio *audio;
	NONS_GeneralArchive *archive;
	SDL_Rect boundingBox;
	bool exitable;
	void *menu;
	SDL_Surface *loadedGraphic;
	NONS_ButtonLayer(SDL_Surface *img,NONS_ScreenSpace *screen);
	NONS_ButtonLayer(NONS_Font *font,NONS_ScreenSpace *screen,bool exitable,void *menu);
	~NONS_ButtonLayer();
	void makeTextButtons(const std::vector<std::wstring> &arr,
		SDL_Color *on,
		SDL_Color *off,
		bool shadow,
		std::wstring *entry,
		std::wstring *mouseover,
		std::wstring *click,
		NONS_Audio *audio,
		NONS_GeneralArchive *archive,
		int width,
		int height);
	void addImageButton(ulong index,int posx,int posy,int width,int height,int originX,int originY);
	int getUserInput(int x,int y);
	int getUserInput(ulong expiration=0);
	ulong countActualButtons();
};
#endif
