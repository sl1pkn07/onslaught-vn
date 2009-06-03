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

#ifndef NONS_SOUNDCACHE_H
#define NONS_SOUNDCACHE_H

#include "../../Common.h"
#include "SoundEffect.h"
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_thread.h>
#include <map>
#include <list>

typedef std::map<std::wstring,NONS_CachedSound *> cache_map_t;

struct NONS_SoundCache{
	cache_map_t cache;
	std::list<NONS_SoundEffect *> channelWatch;
	NONS_SoundCache();
	~NONS_SoundCache();
	NONS_CachedSound *getSound(const std::wstring &filename);
	NONS_CachedSound *checkSound(const std::wstring &filename);
	NONS_CachedSound *newSound(const std::wstring &filename,char *databuffer,long size);
	static int GarbageCollector(NONS_SoundCache *dis);
	SDL_mutex *mutex;
private:
	SDL_Thread *thread;
	bool kill_thread;
};
#endif
