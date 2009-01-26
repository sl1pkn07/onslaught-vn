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

#ifndef NONS_MENU_H
#define NONS_MENU_H
#include "../../Common.h"
#include "ButtonLayer.h"
#include "Layer.h"
#include <SDL/SDL.h>
#include <vector>

struct NONS_Menu{
	std::vector<wchar_t *> strings;
	std::vector<wchar_t *> commands;
	SDL_Color off;
	SDL_Color on;
	SDL_Color nofile;
	bool shadow;
	NONS_ButtonLayer *buttons;
	NONS_ButtonLayer *files;
	short slots;
	void *interpreter;
	int x,y;
	NONS_Layer *shade;
	NONS_Font *font;
	NONS_Font *defaultFont;
	long fontsize,spacing,lineskip;
	SDL_Color shadeColor;
	wchar_t *stringSave;
	wchar_t *stringLoad;
	wchar_t *stringSlot;
	wchar_t *voiceEntry;
	wchar_t *voiceCancel;
	wchar_t *voiceMO;
	wchar_t *voiceClick;
	wchar_t *voiceYes;
	wchar_t *voiceNo;
	NONS_Audio *audio;
	NONS_GeneralArchive *archive;
	uchar rightClickMode;

	NONS_Menu(void *interpreter);
	NONS_Menu(std::vector<wchar_t *> *options,void *interpreter);
	~NONS_Menu();
	int callMenu();
	void reset();
	void resetStrings(std::vector<wchar_t *> *options);
	int save();
	int load();
	int windowerase();
	int skip();
	int call(wchar_t *string);
private:
	int write(wchar_t *txt,int y);
};
#endif
