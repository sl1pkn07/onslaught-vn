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

#ifndef NONS_SOUNDCACHE_CPP
#define NONS_SOUNDCACHE_CPP

#include "SoundCache.h"
#include "../../Functions.h"
#include "../InputHandler.h"
#include "../../Globals.h"

int GC(void *nil){
	return NONS_SoundCache::GarbageCollector((NONS_SoundCache *)nil);
}

NONS_SoundCache::NONS_SoundCache(){
	this->kill_thread=0;
	this->mutex=SDL_CreateMutex();
	this->thread=SDL_CreateThread(GC,this);
}

NONS_SoundCache::~NONS_SoundCache(){
	this->kill_thread=1;
	//SDL_UnlockMutex(this->mutex);
	SDL_DestroyMutex(this->mutex);
	SDL_WaitThread(this->thread,0);
	for (std::map<char *,NONS_CachedSound *,strCmp>::iterator i=this->cache.begin();i!=this->cache.end();i++){
		delete[] i->first;
		delete i->second;
	}
}

//Sound effects stay in the cache for this amount of seconds
#define SE_EXPIRATION_TIME 60

int NONS_SoundCache::GarbageCollector(NONS_SoundCache *dis){
	NONS_EventQueue *queue=InputObserver.attach();
	while (!dis->kill_thread){
		SDL_LockMutex(dis->mutex);
		if (!dis->cache.empty()){
			ulong now=secondsSince1900();
			for (std::map<char *,NONS_CachedSound *,strCmp>::iterator i=dis->cache.begin();i!=dis->cache.end();){
				if (!i->second->references){
					i->second->references=-1;
					i->second->lastused=now;
					i++;
				}else if (i->second->references<0 && now-i->second->lastused>SE_EXPIRATION_TIME){
					if (CLOptions.verbosity>=255)
						std::cout <<"At "<<now<<" removed "<<i->second->chunk<<" ("<<i->first<<")"<<std::endl;
					delete[] i->first;
					delete i->second;
					dis->cache.erase(i);
					if (CLOptions.verbosity>=255)
						std::cout <<"Currently in cache: "<<dis->cache.size()<<" items."<<std::endl;
					i=dis->cache.begin();
				}else
					i++;
			}
			for (ulong a=0;dis->channelWatch.size()>0 && a<10;a++){
				std::list<NONS_SoundEffect *>::iterator i2=dis->channelWatch.begin();
				for (;i2!=dis->channelWatch.end();){
					if (!Mix_Playing((*i2)->channel) && (*i2)->playingHasStarted){
						if (CLOptions.verbosity>=255)
							std::cout <<"At "<<now<<" stopped "<<*i2<<"\n"
								"    cache item "<<(*i2)->sound->chunk<<"\n"
								"    on channel "<<(*i2)->channel<<std::endl;
						(*i2)->unload();
						dis->channelWatch.erase(i2);
						i2=dis->channelWatch.begin();
					}else
						i2++;
				}
				SDL_Delay(100);
			}
		}
		bool justreported=0;
		while (!queue->data.empty()){
			SDL_Event event=queue->pop();
			if (!justreported && event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_F12){
				if (CLOptions.verbosity>=255)
					std::cout <<"Currently in cache: "<<dis->cache.size()<<" items."<<std::endl;
				justreported=1;
			}
		}
		SDL_UnlockMutex(dis->mutex);
		SDL_Delay(100);
	}
	InputObserver.detach(queue);
	return 0;
}

NONS_CachedSound *NONS_SoundCache::checkSound(const char *filename){
	if (this->cache.empty())
		return 0;
	std::map<char *,NONS_CachedSound *,strCmp>::iterator i=this->cache.find((char*)filename);
	if (i==this->cache.end())
		return 0;
	return i->second;
}

NONS_CachedSound *NONS_SoundCache::getSound(const char *filename){
	NONS_CachedSound *res=this->checkSound(filename);
	if (!res)
		return 0;
	if (res->references>=0)
		res->references++;
	else
		res->references=1;
	return res;
}

NONS_CachedSound *NONS_SoundCache::getSound(const wchar_t *filename){
	char *copy=copyString(filename);
	NONS_CachedSound *ret=this->getSound(copy);
	delete[] copy;
	return ret;
}

NONS_CachedSound *NONS_SoundCache::newSound(const char *filename,char *databuffer,long size){
	if (NONS_CachedSound *ret=this->getSound(filename))
		return ret;
	NONS_CachedSound *a=new NONS_CachedSound(databuffer,size);
	char *temp=copyString(filename);
	toforwardslash(temp);
	a->name=temp;
	this->cache[temp]=a;
	return a;
}

NONS_CachedSound *NONS_SoundCache::newSound(const wchar_t *filename,char *databuffer,long size){
	char *copy=copyString(filename);
	NONS_CachedSound *ret=this->newSound(copy,databuffer,size);
	delete[] copy;
	return ret;
}
#endif
