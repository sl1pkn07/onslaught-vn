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

#ifndef NONS_SCREENSPACE_H
#define NONS_SCREENSPACE_H

#include <vector>
#include "StandardOutput.h"
#include "../../Common.h"
#include "GFX.h"
#include "../../ErrorCodes.h"
#include "Lookback.h"
#include "VirtualScreen.h"

struct NONS_ScreenSpace{
	//The stack of layers that will be blended into the final picture. Layers are
	//blended starting with the highest indexes.
	std::vector<NONS_Layer *> layerStack;
	//The bottom layer. This layer is not merged with the screen. Rather, it
	//replaces its contents.
	NONS_Layer *Background;
	NONS_Layer *leftChar;
	NONS_Layer *rightChar;
	NONS_Layer *centerChar;
	//The NONS_VirtualScreen serving as the screen for all graphical output
	//operations.
	NONS_VirtualScreen *screen;
	SDL_Surface *screenBuffer;
	//A StandardOutput serving as an abstraction layer for text output
	//operations.
	NONS_StandardOutput *output;
	NONS_GFXstore *gfx_store;
	NONS_GFX *monochrome;
	NONS_GFX *negative;
	ulong sprite_priority;
	NONS_Lookback *lookback;
	NONS_Layer *cursor;
	NONS_ScreenSpace(int framesize,NONS_Font *font,NONS_GFXstore *store=0);
	NONS_ScreenSpace(SDL_Rect *window,SDL_Rect *frame,NONS_Font *font,bool shadow,NONS_GFXstore *store);
	~NONS_ScreenSpace();
	ErrorCode BlendAll(ulong effect);
	ErrorCode BlendAll(ulong effect,long timing,wchar_t *rule);
	ErrorCode BlendNoCursor(ulong effect);
	ErrorCode BlendNoCursor(ulong effect,long timing,wchar_t *rule);
	ErrorCode BlendNoText(ulong effect);
	ErrorCode BlendNoText(ulong effect,long timing,wchar_t *rule);
	ErrorCode BlendOnlyBG(ulong effect);
	ErrorCode BlendOnlyBG(ulong effect,long timing,wchar_t *rule);
	void clearText();
	void hideText();
	void showText();
	void resetParameters(SDL_Rect *window,SDL_Rect *frame,NONS_Font *font,bool shadow);
	void clear();
	ErrorCode loadSprite(ulong n,const wchar_t *string,long x,long y,uchar alpha,bool visibility);
	bool advanceAnimations(ulong msecs);
};
#endif
