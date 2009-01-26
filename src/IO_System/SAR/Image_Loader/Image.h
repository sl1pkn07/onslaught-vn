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

#ifndef NONS_IMAGE_H
#define NONS_IMAGE_H

#include "../../../Common.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

struct NONS_Image{
	SDL_Surface *image;
	wchar_t *name;
	wchar_t *string;
	/*
	How many fetches have passed since this image was last fetched. Zero if it's
	curretly being used. This variable is used to limit the size of the cache.
	All images are loaded as 32-bit surfaces, so for a 640x480 surface, each
	cache element uses ~1.17 MiB, not counting the name. Therefore, limiting the
	size of the cache can be important on certain systems.
	*/
	ulong age;
	ulong refCount;
	NONS_Image();
	~NONS_Image();
	SDL_Surface *LoadLayerImage(wchar_t *name,uchar *buffer,ulong bufferSize,SDL_Rect *screen,int method);
	SDL_Surface *LoadSpriteImage(wchar_t *string,wchar_t *name,uchar *buffer,ulong bufferSize,int method);
	SDL_Surface *LoadCursorImage(uchar *buffer,ulong bufferSize,int method);
};
#endif
