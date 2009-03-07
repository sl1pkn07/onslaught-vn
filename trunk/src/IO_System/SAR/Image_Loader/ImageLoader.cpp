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

#ifndef NONS_IMAGELOADER_CPP
#define NONS_IMAGELOADER_CPP

#include "ImageLoader.h"
#include "../../../Functions.h"
#include "../../../Globals.h"

NONS_ImageLoader::NONS_ImageLoader(NONS_GeneralArchive *archive,long maxCacheSize){
	this->archive=archive;
	this->maxCacheSize=maxCacheSize;
	if (maxCacheSize<0)
		this->imageCache.reserve(50);
	else
		this->imageCache.reserve(maxCacheSize);
}

NONS_ImageLoader::~NONS_ImageLoader(){
	this->clearCache();
}

ulong NONS_ImageLoader::getCacheSize(){
	ulong res=0;
	for (ulong a=0;a<this->imageCache.size();a++)
		if (this->imageCache[a] && this->imageCache[a]->age)
			res++;
	return res;
}

SDL_Surface *NONS_ImageLoader::fetchImage(const wchar_t *name,SDL_Rect *screen,int method){
	if (!name || !screen)
		return 0;
	wchar_t *tempname=copyWString(name);
	tolower(tempname);
	toforwardslash(tempname);
	for (ulong a=0;a<this->imageCache.size();a++){
		if (this->imageCache[a] && !wcscmp(this->imageCache[a]->name,name)){
			this->imageCache[a]->age=0;
			this->imageCache[a]->refCount++;
			SDL_Surface *res=this->imageCache[a]->image;
			for (a=0;a<this->imageCache.size();a++)
				if (this->imageCache[a] && this->imageCache[a]->age)
					this->imageCache[a]->age++;
			delete[] tempname;
			return res;
		}
	}
	delete[] tempname;
	long l;
	char *buffer=(char *)this->archive->getFileBuffer(name,(ulong *)&l);
	NONS_Image *img=new NONS_Image();
	SDL_Surface *res=img->LoadLayerImage(name,(uchar *)buffer,l,screen,method);
	delete[] buffer;
	this->filelog.addString(name);
	if (!res)
		return 0;
	ulong append;
	for (append=0;append<this->imageCache.size() && !!this->imageCache[append];append++);
	if (append>=this->imageCache.size())
		this->imageCache.push_back(img);
	else
		this->imageCache[append]=img;
	img->age=0;
	img->refCount++;
	for (ulong a=0;a<this->imageCache.size();a++)
		if (this->imageCache[a] && this->imageCache[a]->age)
			this->imageCache[a]->age++;
	if (this->maxCacheSize>=0){
		ulong cachesize=this->getCacheSize();
		if (cachesize>this->maxCacheSize)
			this->freeOldest(cachesize-this->maxCacheSize);
	}
	return res;
}

SDL_Surface *NONS_ImageLoader::fetchCursor(const wchar_t *name,int method){
	if (!name)
		return 0;
	long l;
	char *buffer=(char *)this->archive->getFileBuffer(name,(ulong *)&l);
	NONS_Image *img=new NONS_Image();
	SDL_Surface *res=img->LoadCursorImage((uchar *)buffer,l,method);
	delete[] buffer;
	this->filelog.addString(name);
	return res;
}

SDL_Surface *NONS_ImageLoader::fetchSprite(const wchar_t *string,const wchar_t *name,int method){
	if (!name)
		return 0;
	wchar_t *tempname=copyWString(name);
	tolower(tempname);
	toforwardslash(tempname);
	for (ulong a=0;a<this->imageCache.size();a++){
		if (this->imageCache[a] && !wcscmp(this->imageCache[a]->name,tempname)){
			this->imageCache[a]->age=0;
			this->imageCache[a]->refCount++;
			SDL_Surface *res=this->imageCache[a]->image;
			for (a=0;a<this->imageCache.size();a++)
				if (this->imageCache[a] && this->imageCache[a]->age)
					this->imageCache[a]->age++;
			delete[] tempname;
			return res;
		}
	}
	delete[] tempname;
	long l;
	char *buffer=(char *)this->archive->getFileBuffer(name,(ulong *)&l);
	NONS_Image *img=new NONS_Image();
	SDL_Surface *res=img->LoadSpriteImage(string,name,(uchar *)buffer,l,method);
	delete[] buffer;
	this->filelog.addString(name);
	if (!res)
		return 0;
	ulong append;
	for (append=0;append<this->imageCache.size() && !!this->imageCache[append];append++);
	if (append>=this->imageCache.size())
		this->imageCache.push_back(img);
	else
		this->imageCache[append]=img;
	img->age=0;
	this->imageCache[append]->refCount++;
	for (ulong a=0;a<this->imageCache.size();a++)
		if (this->imageCache[a] && this->imageCache[a]->age)
			this->imageCache[a]->age++;
	if (this->maxCacheSize>=0){
		ulong cachesize=this->getCacheSize();
		if (cachesize>this->maxCacheSize)
			this->freeOldest(cachesize-this->maxCacheSize);
	}
	return res;
}

bool NONS_ImageLoader::unfetchImage(SDL_Surface *which){
	if (!which)
		return 0;
	for (ulong a=0;a<this->imageCache.size();a++){
		NONS_Image *temp=this->imageCache[a];
		if (temp && temp->image==which){
			temp->refCount--;
			if (!temp->refCount)
				temp->age++;
			return 1;
		}
	}
	return 0;
}

long NONS_ImageLoader::freeOldest(long howMany){
	long res=0;
	for (long a=0;a<howMany;a++){
		ulong max=0,maxp=0;
		for (ulong b=0;b<this->imageCache.size();b++){
			if (this->imageCache[b] && this->imageCache[b]->age>max){
				max=this->imageCache[b]->age;
				maxp=b;
			}
		}
		if (!max)
			break;
		delete this->imageCache[maxp];
		this->imageCache[maxp]=0;
		res++;
	}
	return res;
}

ulong NONS_ImageLoader::clearCache(){
	ulong res=0;
	for (ulong a=0;a<this->imageCache.size();a++){
		if (this->imageCache[a] && this->imageCache[a]->age){
			delete this->imageCache[a];
			this->imageCache[a]=0;
		}else
			res++;
	}
	return res;
}

NONS_Image *NONS_ImageLoader::elementFromSurface(SDL_Surface *srf){
	for (ulong a=0;a<this->imageCache.size();a++)
		if (this->imageCache[a] && this->imageCache[a]->image==srf)
			return this->imageCache[a];
	return 0;
}
#endif
