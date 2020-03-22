/*
* Copyright (c) 2008-2010, Helios (helios.vmg@gmail.com)
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

#include "../src/Common.h"

extern const int rmask;
extern const int gmask;
extern const int bmask;
extern const int amask;

#define USE_HARDWARE_SURFACES 0

typedef long manualBlitAlpha_t;
NONS_DECLSPEC void manualBlit(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,manualBlitAlpha_t alpha=255);

inline SDL_Surface *makeSurface(ulong w,ulong h,ulong bits,Uint32 r=rmask,Uint32 g=gmask,Uint32 b=bmask,Uint32 a=amask){
	return SDL_CreateRGBSurface(USE_HARDWARE_SURFACES|SDL_SRCALPHA,(int)w,(int)h,bits,r,g,b,a);
}

inline SDL_Surface *copySurface(SDL_Surface *src,bool fast=1){
	SDL_Surface *res=makeSurface(src->w,src->h,src->format->BitsPerPixel,src->format->Rmask,src->format->Gmask,src->format->Bmask,src->format->Amask);
	if (fast)
		manualBlit(src,0,res,0);
	else
		SDL_BlitSurface(src,0,res,0);
	res->clip_rect=src->clip_rect;
	return res;
}
