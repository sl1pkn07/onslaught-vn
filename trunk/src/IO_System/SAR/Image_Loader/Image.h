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

#ifndef NONS_IMAGE_H
#define NONS_IMAGE_H

#include "../../../Common.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <vector>
#include <map>

struct NONS_AnimationInfo{	
	enum TRANSPARENCY_METHODS{
		LEFT_UP='l',
		RIGHT_UP='r',
		COPY_TRANS='c',
		PARALLEL_MASK='a',
		SEPARATE_MASK='m'
	} method;
	ulong animation_length;
	/*
	If size==1, the first element contains how long each frame will stay on.
	Otherwise, element i contains how long will frame i stay on, plus the
	value of element i-1 if i>0.
	For example, for the string "<10,20,30>", the resulting contents will be
	{10,30,60}
	*/
	std::vector<ulong> frame_ends;
	enum LOOP_TYPE{
		SAWTOOTH_WAVE_CYCLE=0,
		SINGLE_CYCLE,
		TRIANGLE_WAVE_CYCLE,
		NO_CYCLE
	} loop_type;
	ulong animation_time_offset;
	int animation_direction;
	bool valid;
	NONS_AnimationInfo(){}
	NONS_AnimationInfo(const std::wstring &image_string);
	NONS_AnimationInfo(const NONS_AnimationInfo &b);
	NONS_AnimationInfo &operator=(const NONS_AnimationInfo &b);
	~NONS_AnimationInfo();
	void parse(const std::wstring &image_string);
	void resetAnimation();
	long advanceAnimation(ulong msecs);
	long getCurrentAnimationFrame();
	const std::wstring &getFilename(){
		return this->filename;
	}
	const std::wstring &getString(){
		return this->string;
	}
	const std::wstring &getMaskFilename(){
		return this->mask_filename;
	}
private:
	std::wstring filename;
	std::wstring string;
	std::wstring mask_filename;
};

typedef std::map<std::pair<ulong,ulong>,SDL_Rect> optim_t;

struct NONS_Image{
	SDL_Surface *image;
	NONS_AnimationInfo animation;
	/*
	How many fetches have passed since this image was last fetched. Zero if it's
	curretly being used. This variable is used to limit the size of the cache.
	All images are loaded as 32-bit surfaces, so for a 640x480 surface, each
	cache element uses ~1.2 MiB, not counting the name. Therefore, limiting the
	size of the cache can be important on certain systems.
	*/
	ulong age;
	ulong refCount;
	optim_t optimized_updates;
	NONS_Image();
	NONS_Image(const NONS_AnimationInfo *anim,const NONS_Image *primary,const NONS_Image *secondary,optim_t *rects=0);
	~NONS_Image();
	SDL_Surface *LoadImage(const std::wstring &string,const uchar *buffer,ulong bufferSize);
private:
	SDL_Rect getUpdateRect(ulong from,ulong to);
};
#endif
