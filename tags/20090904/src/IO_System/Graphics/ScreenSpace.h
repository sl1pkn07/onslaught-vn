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
	std::vector<NONS_Layer *> layerStack;
	NONS_Layer *Background,
		*leftChar,
		*rightChar,
		*centerChar,
		**characters[3],
		*cursor;
	NONS_VirtualScreen *screen;
	SDL_Surface *screenBuffer;
	NONS_StandardOutput *output;
	NONS_GFXstore *gfx_store;
	NONS_GFX *monochrome,
		*negative;
	ulong sprite_priority;
	NONS_Lookback *lookback;
	ulong char_baseline;
	bool blendSprites;
	std::vector<ulong> charactersBlendOrder;

	NONS_ScreenSpace(int framesize,NONS_Font *font);
	NONS_ScreenSpace(SDL_Rect *window,SDL_Rect *frame,NONS_Font *font,bool shadow);
	~NONS_ScreenSpace();

	ErrorCode BlendAll(ulong effect);
	ErrorCode BlendAll(ulong effect,long timing,const std::wstring *rule);
	void BlendOptimized(std::vector<SDL_Rect> &rects);
	ErrorCode BlendNoCursor(ulong effect);
	ErrorCode BlendNoCursor(ulong effect,long timing,const std::wstring *rule);
	ErrorCode BlendNoText(ulong effect);
	ErrorCode BlendNoText(ulong effect,long timing,const std::wstring *rule);
	ErrorCode BlendOnlyBG(ulong effect);
	ErrorCode BlendOnlyBG(ulong effect,long timing,const std::wstring *rule);
	void clearText();
	void hideText();
	void showText();
	void resetParameters(SDL_Rect *window,SDL_Rect *frame,NONS_Font *font,bool shadow);
	void clear();
	ErrorCode loadSprite(ulong n,const std::wstring &string,long x,long y,uchar alpha,bool visibility);
	bool advanceAnimations(ulong msecs,std::vector<SDL_Rect> &rects);
};
#endif
