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
#include "../../../UTF.h"
#include "../../../Globals.h"

#define LOG_FILENAME_OLD "NScrflog.dat"
#define LOG_FILENAME_NEW "nonsflog.dat"

NONS_ImageLoader::NONS_ImageLoader(NONS_GeneralArchive *archive,long maxCacheSize)
		:filelog(LOG_FILENAME_OLD,LOG_FILENAME_NEW){
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

SDL_Surface *NONS_ImageLoader::fetchSprite(const wchar_t *string,optim_t *rects){
	if (!string)
		return 0;
	NONS_AnimationInfo anim(string);
	if (!anim.valid)
		return 0;
	long bestFit=-1,maskMatch=-1,fileMatch=-1;
	for (ulong a=0;a<this->imageCache.size() && bestFit<0;a++){
		NONS_Image *el=this->imageCache[a];
		if (el){
			if (el->animation.method==NONS_AnimationInfo::COPY_TRANS){
				if (!wcscmp(el->animation.filename,anim.filename))
					fileMatch=a;
				if (anim.method==NONS_AnimationInfo::SEPARATE_MASK && !wcscmp(el->animation.filename,anim.mask_filename))
					maskMatch=a;
			}
			if (el->animation.method==anim.method){
				if (anim.method==NONS_AnimationInfo::SEPARATE_MASK){
					if (fileMatch==a && maskMatch==a)
						bestFit=a;
				}else if (fileMatch==a)
					bestFit=a;
			}
		}
	}
	if (bestFit>=0){
		NONS_Image *el=this->imageCache[bestFit];
		el->age=0;
		el->refCount++;
		for (ulong a=0;a<this->imageCache.size();a++)
			if (this->imageCache[a] && this->imageCache[a]->age)
				this->imageCache[a]->age++;
		if (!!rects)
			*rects=el->optimized_updates;
		return el->image;
	}
	NONS_Image *primary=0;
	bool freePrimary=0;
	if (fileMatch>=0){
		primary=this->imageCache[fileMatch];
		if (primary->age)
			primary->age=1;
	}else{
		long l;
		uchar *buffer=this->archive->getFileBuffer(anim.filename,(ulong *)&l);
		if (!buffer)
			return 0;
		primary=new NONS_Image;
		primary->LoadImage(anim.filename,buffer,l);
		this->filelog.addString(anim.filename);
		delete[] buffer;
		freePrimary=1;
	}
	NONS_Image *secondary=0;
	bool freeSecondary=0;
	if (maskMatch>=0){
		secondary=this->imageCache[maskMatch];
		if (secondary->age)
			secondary->age=1;
	}else if (anim.method==NONS_AnimationInfo::SEPARATE_MASK){
		long l;
		uchar *buffer=this->archive->getFileBuffer(anim.mask_filename,(ulong *)&l);
		if (!buffer)
			return 0;
		secondary=new NONS_Image;
		secondary->LoadImage(anim.mask_filename,buffer,l);
		this->filelog.addString(anim.mask_filename);
		delete[] buffer;
		freeSecondary=1;
	}
	NONS_Image *image=new NONS_Image(&anim,primary,secondary,rects);
	image->refCount++;
	this->addElementToCache(image,1);	
	if (freePrimary && !this->addElementToCache(primary,0))
		delete primary;
	if (freeSecondary && !this->addElementToCache(secondary,0))
		delete secondary;
	for (ulong a=0;a<this->imageCache.size();a++)
		if (this->imageCache[a] && this->imageCache[a]->age)
			this->imageCache[a]->age++;
	return image->image;
}

bool NONS_ImageLoader::addElementToCache(NONS_Image *img,bool force){
	if (!force && !this->maxCacheSize)
		return 0;
	ulong append=0;
	for (;append<this->imageCache.size() && !!this->imageCache[append];append++);
	if (append>=this->imageCache.size())
		this->imageCache.push_back(img);
	else
		this->imageCache[append]=img;
	ulong cachesize=this->getCacheSize();
	if (this->maxCacheSize>0 && cachesize>this->maxCacheSize)
		this->freeOldest(cachesize-this->maxCacheSize);
	return 1;
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
