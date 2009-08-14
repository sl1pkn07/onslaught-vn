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

#ifndef NONS_LAYER_H
#define NONS_LAYER_H

#include <SDL/SDL_image.h>
#include <vector>
#include "FontCache.h"
#include "../../Common.h"
#include "../SAR/ImageLoader.h"
#include "../../enums.h"
#include <map>

struct NONS_Layer{
	//The actual bitmap and other stuff.
	SDL_Surface *data;
	//If null, it's an image layer. Otherwise it's a text layer.
	NONS_FontCache *fontCache;
	//Um... I think this is passed to SDL_FillRect() whenever told to do a Clear().
	unsigned defaultShade;
	//Determines whether this layer will be included in the blend.
	bool visible;
	bool useDataAsDefaultShade;
	SDL_Rect clip_rect,
		position;
	uchar alpha;
	optim_t optimized_updates;
	NONS_AnimationInfo animation;
	NONS_Layer(SDL_Rect *size,unsigned rgba);
	NONS_Layer(SDL_Surface *img,unsigned rgba);
	NONS_Layer(const std::wstring *string);
	~NONS_Layer();
	void MakeTextLayer(NONS_Font *font,SDL_Color *foreground,bool shadow);
	bool load(const std::wstring *string);
	bool load(SDL_Surface *src);
	//if the parameter is true and the image isn't shared, the call has no effect
	bool unload(bool youCantTouchThis=0);
	void usePicAsDefaultShade(SDL_Surface *pic);
	void setShade(uchar r,uchar g,uchar b);
	void Clear();
	//1 if the layer should be re-blended.
	bool advanceAnimation(ulong msec);
	void centerAround(int x);
	void useBaseline(int y);
	bool animated(){
		return this->animation.frame_ends.size() && 
			this->animation.animation_time_offset<this->animation.frame_ends.back()*
			(this->animation.frame_ends.size()==1?this->animation.animation_length:1);
	}
};
#endif
