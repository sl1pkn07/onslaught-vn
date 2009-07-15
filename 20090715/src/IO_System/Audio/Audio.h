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

#ifndef NONS_AUDIO_H
#define NONS_AUDIO_H

#include "SoundEffect.h"
#include "Music.h"
#include "SoundCache.h"
#include "../../Common.h"
#include "../../ErrorCodes.h"
#include <map>
#include <string>

typedef std::map<int,NONS_SoundEffect *> channels_map_t;

struct NONS_Audio{
	NONS_Music *music;
	std::wstring musicDir;
	std::wstring musicFormat;
	NONS_SoundCache *soundcache;
	NONS_Audio(const std::wstring &musicDir);
	~NONS_Audio();
	ErrorCode playMusic(const std::wstring *filename,long times=-1);
	ErrorCode playMusic(const std::wstring &filename,char *buffer,long l,long times=-1);
	ErrorCode stopMusic();
	ErrorCode pauseMusic();
	ErrorCode playSoundAsync(const std::wstring *filename,char *buffer,long l,int channel,long times=-1);
	ErrorCode stopSoundAsync(int channel);
	ErrorCode loadAsyncBuffer(const std::wstring &filename,char *buffer,long l,int channel);
	ErrorCode stopAllSound();
	bool bufferIsLoaded(const std::wstring &filename);
	bool asyncBufferIsLoaded(int channel,std::wstring *filename=0);
	ErrorCode unloadSyncBuffer(int channel);
	int musicVolume(int vol);
	int soundVolume(int vol);
	void freeCacheElement(int chan);
	bool toggleMute();
	bool isInitialized(){
		return !this->uninitialized;
	}
private:
	channels_map_t asynchronous_seffect;
	SDL_mutex *mutex;
	int mvol,
		svol;
	bool notmute;
	bool uninitialized;
};
#endif
