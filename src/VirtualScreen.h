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

#ifndef NONS_VIRTUALSCREEN_H
#define NONS_VIRTUALSCREEN_H

#include "Common.h"
#include <SDL/SDL.h>
#include <string>

extern SDL_mutex *screenMutex;

#if 0
#define LOCKSCREEN {\
	SDL_LockMutex(screenMutex);\
	char LOCKSCREEN_buf[1000];\
	sprintf(LOCKSCREEN_buf,"%u - %s (%d) from 0x%08X LOCK.\n",SDL_GetTicks(),__FILE__,__LINE__,SDL_ThreadID());\
	o_stderr <<LOCKSCREEN_buf;\
}

#define UNLOCKSCREEN {\
	char UNLOCKSCREEN_buf[1000];\
	sprintf(UNLOCKSCREEN_buf,"%u - %s (%d) from 0x%08X UNLOCK.\n",SDL_GetTicks(),__FILE__,__LINE__,SDL_ThreadID());\
	o_stderr <<UNLOCKSCREEN_buf;\
	SDL_UnlockMutex(screenMutex);\
}
#else
#define LOCKSCREEN SDL_LockMutex(screenMutex)
#define UNLOCKSCREEN SDL_UnlockMutex(screenMutex)
#endif

struct NONS_VirtualScreen{
	SDL_Surface *realScreen;
	SDL_Surface *virtualScreen;
	SDL_Rect inRect;
	SDL_Rect outRect;
	ulong x_multiplier;
	ulong y_multiplier;
	ulong x_divisor;
	ulong y_divisor;
	void (*normalInterpolation)(SDL_Surface *,SDL_Rect *,SDL_Surface *,SDL_Rect *,ulong,ulong);
	bool fullscreen;
	NONS_VirtualScreen(ulong w,ulong h);
	NONS_VirtualScreen(ulong iw,ulong ih,ulong ow,ulong oh);
	~NONS_VirtualScreen();
	void blitToScreen(SDL_Surface *src,SDL_Rect *srcrect,SDL_Rect *dstrect);
	//Note: Call with the screen unlocked or you'll enter a deadlock.
	void updateScreen(ulong x,ulong y,ulong w,ulong h,bool fast=0);
	void updateWholeScreen(bool fast=0);
	//If 0, to window; if 1, to fullscreen; if 2, toggle.
	bool toggleFullscreen(uchar mode=2);
	long convertX(long x);
	long convertY(long y);
	long unconvertX(long x);
	long unconvertY(long y);
	ulong convertW(ulong w);
	ulong convertH(ulong h);
	void updateWithoutLock(bool fast=0);
	std::string takeScreenshot(const std::string &filename="");
};

void nearestNeighborInterpolation(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor);
void bilinearInterpolation(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor);
void bilinearInterpolation2(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor);
#endif
