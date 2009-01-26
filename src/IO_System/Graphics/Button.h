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

#ifndef NONS_BUTTON_H
#define NONS_BUTTON_H

#include "../../Common.h"
#include "ScreenSpace.h"
#include <vector>

struct NONS_Button{
	NONS_Layer *offLayer;
	NONS_Layer *onLayer;
	NONS_Layer *shadowLayer;
	SDL_Rect box;
	NONS_ScreenSpace *screen;
	NONS_Font *font;
	bool status;
	int posx,posy;
	int limitX,limitY;
	NONS_Button();
	NONS_Button(NONS_Font *font);
	~NONS_Button();
	void makeTextButton(wchar_t *text,float center,SDL_Color *on,SDL_Color *off,bool shadow,int limitX,int limitY);
	void makeGraphicButton(SDL_Surface *src,int posx,int posy,int width,int height,int originX,int originY);
	void mergeWithoutUpdate(NONS_VirtualScreen *dst,SDL_Surface *original,bool status,bool force=0);
	void merge(NONS_VirtualScreen *dst,SDL_Surface *original,bool status,bool force=0);
	bool MouseOver(SDL_Event *event);
	bool MouseOver(int x,int y);
private:
	SDL_Rect GetBoundingBox(wchar_t *str,NONS_FontCache *cache,int limitX,int limitY);
	void write(wchar_t *str,float center=0);
	int setLineStart(std::vector<NONS_Glyph *> *arr,long start,SDL_Rect *frame,float center);
	int predictLineLength(std::vector<NONS_Glyph *> *arr,long start,int width);
};
#endif
