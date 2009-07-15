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

#ifndef NONS_LOOKBACK_H
#define NONS_LOOKBACK_H

#include "../../Common.h"
#include "StandardOutput.h"
#include <SDL/SDL.h>

struct NONS_Lookback{
	SDL_Color foreground;
	NONS_StandardOutput *output;
	void *up;
	void *down;
	SDL_Surface *sUpon;
	SDL_Surface *sUpoff;
	SDL_Surface *sDownon;
	SDL_Surface *sDownoff;
	NONS_Lookback(NONS_StandardOutput *output,uchar r,uchar g,uchar b);
	~NONS_Lookback();
	bool setUpButtons(const std::wstring &upon,const std::wstring &upoff,const std::wstring &downon,const std::wstring &downoff);
	void display(NONS_VirtualScreen *dst);
	void reset(NONS_StandardOutput *output);
private:
	bool changePage(int dir,long &currentPage,SDL_Surface *copyDst,NONS_VirtualScreen *dst,SDL_Surface *preBlit,uchar &visibility,int &mouseOver);
};
#endif
