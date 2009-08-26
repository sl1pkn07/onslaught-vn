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

#ifndef NONS_STANDARDOUTPUT_H
#define NONS_STANDARDOUTPUT_H

#include "../InputHandler.h"
#include "Layer.h"
#include "../../Common.h"
#include "GFX.h"
#include "VirtualScreen.h"
#include <string>

struct NONS_StandardOutput{
	//Current distance (px) from the upper left corner of the cursor to the left edge of the screen.
	int x;
	//Current distance (px) from the upper left corner of the cursor to the top edge of the screen.
	int y;
	//Distance (px) from the upper left corner of the frame to the left edge of the screen.
	int x0;
	//Distance (px) from the upper left corner of the frame to the top edge of the screen.
	int y0;
	//Width of the frame.
	int w;
	//Height of the frame.
	int h;
	NONS_Layer *foregroundLayer,
		*shadowLayer,
		*shadeLayer;
	long shadowPosX,
		shadowPosY;
	ulong display_speed;
	int extraAdvance;
	bool visible;
	NONS_GFX *transition;
	std::vector<std::wstring> log;
	std::wstring currentBuffer;
	ulong indentationLevel;
	bool indent_next;
	long maxLogPages;

	NONS_StandardOutput(NONS_Layer *fgLayer,NONS_Layer *shadowLayer,NONS_Layer *shadeLayer);
	NONS_StandardOutput(NONS_Font *font,SDL_Rect *size,SDL_Rect *frame,bool shadow=1);
	void Clear(bool eraseBuffer=1);
	~NONS_StandardOutput();
	void setPosition(int x,int y);
	//if return==1, there should be a call to the page cursor and a screen clear before
	//calling print().
	bool prepareForPrinting(const std::wstring str);
	bool print(ulong start,ulong end,NONS_VirtualScreen *dst,ulong *printedChars=0);
	void endPrinting();
	void ephemeralOut(std::wstring *str,NONS_VirtualScreen *dst,bool update,bool writeToLayers,SDL_Color *col);
	float getCenterPolicy(char which);
	void setCenterPolicy(char which,float val);
	void setCenterPolicy(char which,long val);
	bool NewLine();
private:
	int predictLineLength(std::wstring *arr,long start,int width);
	int predictTextHeight(std::wstring *arr);
	int setLineStart(std::wstring *arr,ulong start,SDL_Rect *frame,float center);
	int setTextStart(std::wstring *arr,SDL_Rect *frame,float center);
	ulong getIndentationSize();
	float horizontalCenterPolicy;
	float verticalCenterPolicy;
	long lastStart;
	std::wstring cachedText;
	std::wstring prebufferedText;
	bool resumePrinting;
	bool printingStarted;
	ulong resumePrintingWhere;
};

std::wstring removeTags(const std::wstring &str);
#endif
