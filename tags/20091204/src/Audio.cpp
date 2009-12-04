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

#include "Audio.h"
#include "IOFunctions.h"
#include "CommandLineOptions.h"
#include <iostream>

void GC(void *param){
	NONS_SoundCache::GarbageCollector((NONS_SoundCache *)param);
}

NONS_SoundCache::NONS_SoundCache(){
	this->kill_thread=0;
	this->thread.call(GC,this);
}

NONS_SoundCache::~NONS_SoundCache(){
	this->kill_thread=1;
	this->thread.join();
	for (cache_map_t::iterator i=this->cache.begin();i!=this->cache.end();i++)
		delete i->second;
}

//Sound effects stay in the cache for this amount of seconds
#define SE_EXPIRATION_TIME 60

void NONS_SoundCache::GarbageCollector(NONS_SoundCache *_this){
	NONS_EventQueue queue;
	while (!_this->kill_thread){
		{
			NONS_MutexLocker ml(_this->mutex);
			if (!_this->cache.empty()){
				ulong now=secondsSince1970();
				for (cache_map_t::iterator i=_this->cache.begin();i!=_this->cache.end();){
					if (!i->second->references){
						i->second->references=-1;
						i->second->lastused=now;
						i++;
					}else if (i->second->references<0 && now-i->second->lastused>SE_EXPIRATION_TIME){
						if (CLOptions.verbosity>=255)
							std::cout <<"At "<<now<<" removed "<<i->second->chunk<<" ("<<UniToUTF8(i->first)<<")"<<std::endl;
						delete i->second;
						_this->cache.erase(i);
						if (CLOptions.verbosity>=255)
							std::cout <<"Currently in cache: "<<_this->cache.size()<<" items."<<std::endl;
						i=_this->cache.begin();
					}else
						i++;
				}
				for (ulong a=0;_this->channelWatch.size()>0 && a<10;a++){
					std::list<NONS_SoundEffect *>::iterator i2=_this->channelWatch.begin();
					for (;i2!=_this->channelWatch.end();){
						if (!Mix_Playing((*i2)->channel) && (*i2)->playingHasStarted){
							if (CLOptions.verbosity>=255)
								std::cout <<"At "<<now<<" stopped "<<*i2<<"\n"
									"    cache item "<<(*i2)->sound->chunk<<"\n"
									"    on channel "<<(*i2)->channel<<std::endl;
							(*i2)->unload();
							_this->channelWatch.erase(i2);
							i2=_this->channelWatch.begin();
						}else
							i2++;
					}
					SDL_Delay(100);
				}
			}
			bool justreported=0;
			while (!queue.empty()){
				SDL_Event event=queue.pop();
				if (!justreported && event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_F11){
					if (CLOptions.verbosity>=255)
						std::cout <<"Currently in cache: "<<_this->cache.size()<<" items."<<std::endl;
					justreported=1;
				}
			}
		}
		SDL_Delay(100);
	}
}

NONS_CachedSound *NONS_SoundCache::checkSound(const std::wstring &filename){
	if (this->cache.empty())
		return 0;
	cache_map_t::iterator i=this->cache.find(filename);
	if (i==this->cache.end())
		return 0;
	return i->second;
}

NONS_CachedSound *NONS_SoundCache::getSound(const std::wstring &filename){
	NONS_CachedSound *res=this->checkSound(filename);
	if (!res)
		return 0;
	if (res->references>=0)
		res->references++;
	else
		res->references=1;
	return res;
}

NONS_CachedSound *NONS_SoundCache::newSound(const std::wstring &filename,char *databuffer,long size){
	if (NONS_CachedSound *ret=this->getSound(filename))
		return ret;
	NONS_CachedSound *a=new NONS_CachedSound(databuffer,size);
	std::wstring temp=filename;
	toforwardslash(temp);
	a->name=temp;
	this->cache[temp]=a;
	return a;
}

NONS_CachedSound::NONS_CachedSound(char *databuffer,long size){
	this->chunk=Mix_LoadWAV_RW(SDL_RWFromMem((void *)databuffer,size),1);
	delete[] databuffer;
	this->references=1;
	this->lastused=secondsSince1970();
}

NONS_CachedSound::~NONS_CachedSound(){
	Mix_FreeChunk(this->chunk);
}

NONS_SoundEffect::NONS_SoundEffect(int chan){
	this->channel=chan;
	this->sound=0;
	this->playingHasStarted=0;
	this->isplaying=0;
	this->loops=0;
}

NONS_SoundEffect::~NONS_SoundEffect(){
	if (this->sound)
		this->sound->references--;
}

void NONS_SoundEffect::play(bool synchronous,long times){
	this->stop();
	Mix_PlayChannel(this->channel,this->sound->chunk,times);
	this->playingHasStarted=1;
	this->isplaying=1;
	this->loops=times;
}

void NONS_SoundEffect::stop(){
	if (Mix_Playing(this->channel))
		Mix_HaltChannel(this->channel);
	this->isplaying=0;
}

bool NONS_SoundEffect::loaded(){
	return !!this->sound;
}

void NONS_SoundEffect::load(NONS_CachedSound *sound){
	this->unload();
	this->sound=sound;
	this->playingHasStarted=0;
}

void NONS_SoundEffect::unload(){
	if (this->loaded()){
		this->stop();
		this->sound->references--;
		this->sound=0;
		this->playingHasStarted=0;
	}
}

int NONS_SoundEffect::volume(int vol){
	Mix_Volume(this->channel,vol);
	return Mix_Volume(this->channel,-1);
}

NONS_Music::NONS_Music(const std::wstring &filename){
	this->data=Mix_LoadMUS(UniToUTF8(filename).c_str());
	this->buffer=0;
	this->buffersize=0;
	this->filename=filename;
	this->RWop=0;
}

NONS_Music::NONS_Music(const std::wstring &filename,char *databuffer,long size){
	this->buffer=databuffer;
	this->buffersize=size;
	this->RWop=SDL_RWFromMem((void *)databuffer,size);
	this->data=Mix_LoadMUS_RW(this->RWop);
	this->filename=filename;
}

NONS_Music::~NONS_Music(){
	this->stop();
	if (this->data)
		Mix_FreeMusic(this->data);
	if (this->buffer)
		delete[] this->buffer;
}

void NONS_Music::play(long times){
	if (Mix_PlayingMusic())
		return;
	Mix_PlayMusic(this->data,times);
	int x=Mix_VolumeMusic(-1);
	Mix_VolumeMusic(0);
	SDL_Delay(50);
	Mix_VolumeMusic(x);
}

void NONS_Music::stop(){
	if (Mix_PlayingMusic() || Mix_PausedMusic())
		Mix_HaltMusic();
}

void NONS_Music::pause(){
	if (Mix_PlayingMusic() && !Mix_PausedMusic())
		Mix_PauseMusic();
	else
		Mix_ResumeMusic();
}

bool NONS_Music::loaded(){
	return this->data!=0;
}

int NONS_Music::volume(int vol){
	Mix_VolumeMusic(vol);
	return Mix_VolumeMusic(-1);
}

bool NONS_Music::is_playing(){
	return !!Mix_PlayingMusic();
}

NONS_Audio::NONS_Audio(const std::wstring &musicDir){
	this->music=0;
	if (CLOptions.no_sound){
		this->uninitialized=1;
		this->soundcache=0;
		this->soundcache=0;
		this->notmute=0;
		this->mvol=0;
		this->svol=0;
		return;
	}
	Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,512);
	if (!musicDir.size())
		this->musicDir=L"./CD";
	else
		this->musicDir=musicDir;
	this->musicFormat=CLOptions.musicFormat;
	this->soundcache=new NONS_SoundCache();
	this->mvol=100;
	this->svol=100;
	this->notmute=1;
	this->uninitialized=0;
}

NONS_Audio::~NONS_Audio(){
	if (this->uninitialized)
		return;
	this->stopAllSound();
	if (this->music)
		delete this->music;
	delete this->soundcache;
	for (channels_map_t::iterator i=this->asynchronous_seffect.begin();i!=this->asynchronous_seffect.end();i++)
		delete i->second;
	Mix_CloseAudio();
}

void NONS_Audio::freeCacheElement(int channel){
	if (this->uninitialized)
		return;
	channels_map_t::iterator i=this->asynchronous_seffect.find(channel);
	if (i==this->asynchronous_seffect.end() || !i->second)
		return;
	i->second->sound->references--;
	i->second->unload();
}

ErrorCode NONS_Audio::playMusic(const std::wstring *filename,long times){
	static const wchar_t *formats[]={L"ogg",L"mp3",L"it",L"xm",L"s3m",L"mod",0};
	if (this->uninitialized)
		return NONS_NO_ERROR;
	if (!filename){
		if (!this->music)
			return NONS_NO_MUSIC_LOADED;
		this->music->play(times);
		return NONS_NO_ERROR;
	}else{
		std::wstring temp;
		if (this->music){
			this->music->stop();
			delete this->music;
		}
		if (!this->musicFormat.size()){
			ulong a=0;
			while (1){
				if (!formats[a])
					break;
				temp=this->musicDir+L"/"+*filename+L"."+formats[a];
				if (fileExists(temp))
					break;
				a++;
			}
		}else
			temp=this->musicDir+L"/"+*filename+L"."+this->musicFormat;
		if (!fileExists(temp) && !fileExists(temp=*filename))
			return NONS_FILE_NOT_FOUND;
		this->music=new NONS_Music(temp);
		if (!this->music->loaded())
			return NONS_UNDEFINED_ERROR;
		return this->playMusic(0,times);
	}
}

ErrorCode NONS_Audio::playMusic(const std::wstring &filename,char *buffer,long l,long times){
	if (this->uninitialized)
		return NONS_NO_ERROR;
	if (buffer && l){
		if (this->music){
			if (filename==this->music->filename)
				return this->playMusic(0,times);
			delete this->music;
		}
		this->music=new NONS_Music(filename,buffer,l);
		if (!this->music->loaded())
			return NONS_UNDEFINED_ERROR;
		if (this->notmute)
			this->music->volume(this->mvol);
		else
			this->music->volume(0);
		return this->playMusic(0,times);
	}
	o_stderr <<"int NONS_Audio::playMusic(): Internal error.\n";
	return NONS_INTERNAL_INVALID_PARAMETER;
}

ErrorCode NONS_Audio::stopMusic(){
	if (this->uninitialized)
		return NONS_NO_ERROR;
	if (this->music){
		this->music->stop();
		return NONS_NO_ERROR;
	}
	return NONS_NO_MUSIC_LOADED;
}

ErrorCode NONS_Audio::pauseMusic(){
	if (this->uninitialized)
		return NONS_NO_ERROR;
	if (this->music){
		this->music->pause();
		return NONS_NO_ERROR;
	}
	return NONS_NO_MUSIC_LOADED;
}

ErrorCode NONS_Audio::playSoundAsync(const std::wstring *filename,char *buffer,long l,int channel,long times){
	if (this->uninitialized)
		return NONS_NO_ERROR;
	if (!filename){
		NONS_SoundEffect *se=this->asynchronous_seffect[channel];
		if (!se || !se->loaded())
			return NONS_NO_SOUND_EFFECT_LOADED;
		if (this->notmute)
			se->volume(this->svol);
		else
			se->volume(0);
		se->play(0,times);
		{
			NONS_MutexLocker(this->soundcache->mutex);
			this->soundcache->channelWatch.push_back(se);
		}
		if (CLOptions.verbosity>=255)
			std::cout <<"At "<<secondsSince1970()<<" started "<<se<<"\n"
				"    cache item "<<se->sound->chunk<<"\n"
				"    on channel "<<se->channel<<std::endl;
		return NONS_NO_ERROR;
	}else{
		_HANDLE_POSSIBLE_ERRORS(this->loadAsyncBuffer(*filename,buffer,l,channel));
		return this->playSoundAsync(0,0,0,channel,times);
	}
}

ErrorCode NONS_Audio::stopSoundAsync(int channel){
	if (this->uninitialized)
		return NONS_NO_ERROR;
	std::map<int,NONS_SoundEffect *>::iterator i=this->asynchronous_seffect.find(channel);
	if (i==this->asynchronous_seffect.end() || !i->second)
		return NONS_NO_SOUND_EFFECT_LOADED;
	i->second->stop();
	return NONS_NO_ERROR;
}

ErrorCode NONS_Audio::stopAllSound(){
	if (this->uninitialized)
		return NONS_NO_ERROR;
	this->stopMusic();
	for (std::map<int,NONS_SoundEffect *>::iterator i=this->asynchronous_seffect.begin();i!=this->asynchronous_seffect.end();i++){
		if (i->second)
			i->second->stop();
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_Audio::loadAsyncBuffer(const std::wstring &filename,char *buffer,long l,int channel){
	if (this->uninitialized)
		return NONS_NO_ERROR;
	std::map<int,NONS_SoundEffect *>::iterator i=this->asynchronous_seffect.find(channel);
	if (i==this->asynchronous_seffect.end() || !i->second){
		this->asynchronous_seffect[channel]=new NONS_SoundEffect(channel);
		i=this->asynchronous_seffect.find(channel);
	}
	NONS_CachedSound *cs=this->soundcache->getSound(filename);
	if (!cs){
		if (!buffer)
			return NONS_UNDEFINED_ERROR;
		cs=this->soundcache->newSound(filename,buffer,l);
	}
	i->second->load(cs);
	if (!i->second->loaded())
		return NONS_UNDEFINED_ERROR;
	return NONS_NO_ERROR;
}

bool NONS_Audio::bufferIsLoaded(const std::wstring &filename){
	if (this->uninitialized)
		return 1;
	return this->soundcache->checkSound(filename)!=0;
}

int NONS_Audio::musicVolume(int vol){
	if (this->uninitialized)
		return 0;
	NONS_MutexLocker ml(this->mutex);
	if (!music){
		int ret=this->mvol;
		return ret;
	}
	if (this->notmute)
		this->mvol=this->music->volume(vol);
	else if (vol>=0)
		this->mvol=(vol<100)?vol:100;
	return this->mvol;
}

int NONS_Audio::soundVolume(int vol){
	if (this->uninitialized)
		return 0;
	NONS_MutexLocker ml(this->mutex);
	if (this->notmute){
		this->svol=0;
		for (std::map<int,NONS_SoundEffect *>::iterator i=this->asynchronous_seffect.begin();i!=this->asynchronous_seffect.end();i++)
			this->svol+=i->second->volume(vol);
		if (this->svol)
			this->svol/=this->asynchronous_seffect.size();
		else
			this->svol=100;
	}else if (vol>=0)
		this->svol=(vol<100)?vol:100;
	return this->svol;
}

bool NONS_Audio::toggleMute(){
	if (this->uninitialized)
		return 0;
	NONS_MutexLocker ml(this->mutex);
	this->notmute=!this->notmute;
	if (this->notmute){
		this->music->volume(this->mvol);
		for (std::map<int,NONS_SoundEffect *>::iterator i=this->asynchronous_seffect.begin();i!=this->asynchronous_seffect.end();i++)
			i->second->volume(this->svol);
	}else{
		this->music->volume(0);
		for (std::map<int,NONS_SoundEffect *>::iterator i=this->asynchronous_seffect.begin();i!=this->asynchronous_seffect.end();i++)
			i->second->volume(0);
	}
	return this->notmute;

}