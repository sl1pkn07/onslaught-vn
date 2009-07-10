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

#ifndef NONS_AUDIO_CPP
#define NONS_AUDIO_CPP

#include "Audio.h"
#include "../FileIO.h"
#include "../../Functions.h"
#include "../../Globals.h"
#include <sstream>

NONS_Audio::NONS_Audio(const std::string &musicDir){
	this->music=0;
	if (CLOptions.no_sound){
		this->uninitialized=1;
		this->soundcache=0;
		this->soundcache=0;
		this->mutex=0;
		this->notmute=0;
		this->mvol=0;
		this->svol=0;
		return;
	}
	Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,512);
	if (!musicDir.size())
		this->musicDir="./CD";
	else
		this->musicDir=musicDir;
	this->musicFormat=CLOptions.musicFormat;
	this->soundcache=new NONS_SoundCache();
	this->mvol=100;
	this->svol=100;
	this->notmute=1;
	this->mutex=SDL_CreateMutex();
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

ErrorCode NONS_Audio::playMusic(const std::string *filename,long times){
	static const char *formats[]={"ogg","mp3","it","xm","s3m","mod",0};
	if (this->uninitialized)
		return NONS_NO_ERROR;
	if (!filename){
		if (!this->music)
			return NONS_NO_MUSIC_LOADED;
		this->music->play(times);
		return NONS_NO_ERROR;
	}else{
		std::string temp;
		if (this->music){
			this->music->stop();
			delete this->music;
		}
		if (!this->musicFormat.size()){
			ulong a=0;
			while (1){
				if (!formats[a])
					break;
				temp=this->musicDir+"/"+*filename+"."+formats[a];
				if (fileExists(temp.c_str()))
					break;
				a++;
			}
		}else
			temp=this->musicDir+"/"+*filename+"."+this->musicFormat;
		if (!fileExists(temp.c_str()))
			return NONS_FILE_NOT_FOUND;
		this->music=new NONS_Music(temp);
		if (!this->music->loaded())
			return NONS_UNDEFINED_ERROR;
		return this->playMusic(0,times);
	}
}

ErrorCode NONS_Audio::playMusic(const std::string &filename,char *buffer,long l,long times){
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
		SDL_LockMutex(this->soundcache->mutex);
		this->soundcache->channelWatch.push_back(se);
		SDL_UnlockMutex(this->soundcache->mutex);
		if (CLOptions.verbosity>=255)
			std::cout <<"At "<<secondsSince1970()<<" started "<<se<<"\n"
				"    cache item "<<se->sound->chunk<<"\n"
				"    on channel "<<se->channel<<std::endl;
		return NONS_NO_ERROR;
	}else{
		_HANDLE_POSSIBLE_ERRORS(this->loadAsyncBuffer(*filename,buffer,l,channel),)
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
	SDL_LockMutex(this->mutex);
	if (!music){
		int ret=this->mvol;
		SDL_UnlockMutex(this->mutex);
		return ret;
	}
	if (this->notmute)
		this->mvol=this->music->volume(vol);
	else if (vol>=0)
		this->mvol=(vol<100)?vol:100;
	SDL_UnlockMutex(this->mutex);
	return this->mvol;
}

int NONS_Audio::soundVolume(int vol){
	if (this->uninitialized)
		return 0;
	SDL_LockMutex(this->mutex);
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
	SDL_UnlockMutex(this->mutex);
	return this->svol;
}

bool NONS_Audio::toggleMute(){
	if (this->uninitialized)
		return 0;
	SDL_LockMutex(this->mutex);
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
	SDL_UnlockMutex(this->mutex);
	return this->notmute;

}
#endif
