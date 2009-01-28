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

#ifndef NONS_IMAGELOADER_H
#define NONS_IMAGELOADER_H

#include "../../../Common.h"
#include "../GeneralArchive.h"
#include "FileLog.h"
#include "Image.h"
#include <vector>

struct NONS_ImageLoader{
	NONS_GeneralArchive *archive;
	std::vector<NONS_Image *> imageCache;
	//<0: infinite (until memory is exhausted)
	long maxCacheSize;
	NONS_FileLog filelog;
	NONS_ImageLoader(NONS_GeneralArchive *archive,long maxCacheSize=-1);
	~NONS_ImageLoader();
	ulong getCacheSize();
	SDL_Surface *fetchImage(wchar_t *name,SDL_Rect *screen,int method);
	SDL_Surface *fetchCursor(wchar_t *name,int method);
	SDL_Surface *fetchSprite(wchar_t *string,wchar_t *name,int method);
	bool unfetchImage(SDL_Surface *which);
	NONS_Image *elementFromSurface(SDL_Surface *srf);
	long freeOldest(long howMany=1);
	ulong clearCache();
	void printCurrent();
};
#endif
