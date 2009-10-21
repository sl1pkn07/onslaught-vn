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

#include "ImageLoader.h"
#include "../../Functions.h"

void NONS_AnimationInfo::parse(const std::wstring &image_string){
	this->string=image_string;
	this->valid=0;
	this->method=COPY_TRANS;
	this->animation_length=1;
	this->animation_time_offset=0;
	this->animation_direction=1;
	this->frame_ends.clear();
	size_t p=0;
	static const std::wstring slash_semi=UniFromISO88591("/;");
	if (image_string[p]==':'){
		p++;
		size_t semicolon=image_string.find(';',p);
		if (semicolon==image_string.npos)
			return;
		switch (wchar_t c=NONS_tolower(image_string[p++])){
			case 'l':
			case 'r':
			case 'c':
			case 'a':
			case 'm':
				this->method=(TRANSPARENCY_METHODS)c;
				break;
			default:
				return;
		}
		size_t p2=image_string.find_first_of(slash_semi,p);
		if (this->method==SEPARATE_MASK){
			if (p2==image_string.npos)
				return;
			this->mask_filename=std::wstring(image_string.begin()+p,image_string.begin()+p2);
			tolower(this->mask_filename);
			toforwardslash(this->mask_filename);
		}
		p=p2;
		if (image_string[p]=='/'){
			std::wstringstream stream;
			while (image_string[++p]!=',' && image_string[p]!=';')
				stream <<image_string[p];
			if (!(stream >>this->animation_length) || image_string[p]!=',')
				return;
			stream.clear();
			if (image_string[p+1]!='<'){
				while (image_string[++p]!=',' && image_string[p]!=';')
					stream <<image_string[p];
				ulong delay;
				if (!(stream >>delay))
					return;
				stream.clear();
				if (this->frame_ends.size())
					delay+=this->frame_ends.back();
				this->frame_ends.push_back(delay);
			}else{
				p++;
				size_t gt=image_string.find('>',p);
				if (gt==image_string.npos || gt>semicolon)
					return;
				while (image_string[p]!='>' && image_string[++p]!='>'){
					while (image_string[p]!=',' && image_string[p]!='>')
						stream <<image_string[p++];
					ulong delay;
					if (!(stream >>delay))
						return;
					stream.clear();
					this->frame_ends.push_back(delay);
				}
				p++;
			}
			if (!this->frame_ends.size() || this->frame_ends.size()>1 && this->animation_length!=this->frame_ends.size())
				return;
			if (image_string[p]!=',')
				return;
			while (image_string[++p]!=',' && image_string[p]!=';')
				stream <<image_string[p];
			ulong type;
			if (!(stream >>type))
				return;
			this->loop_type=(LOOP_TYPE)type;
			p++;
		}
	}
	if (image_string[p]==';')
		p++;
	this->filename=image_string.substr(p);
	tolower(this->filename);
	toforwardslash(this->filename);
	this->animation_time_offset=0;
	this->valid=1;
}

NONS_AnimationInfo::NONS_AnimationInfo(const std::wstring &image_string){
	this->parse(image_string);
}

NONS_AnimationInfo::NONS_AnimationInfo(const NONS_AnimationInfo &b){
	*this=b;
}

NONS_AnimationInfo &NONS_AnimationInfo::operator=(const NONS_AnimationInfo &b){
	this->method=b.method;
	this->mask_filename=b.mask_filename;
	this->animation_length=b.animation_length;
	this->frame_ends=b.frame_ends;
	this->loop_type=b.loop_type;
	this->filename=b.filename;
	this->string=b.string;
	this->animation_time_offset=b.animation_time_offset;
	this->valid=b.valid;
	return *this;
}

NONS_AnimationInfo::~NONS_AnimationInfo(){
}

void NONS_AnimationInfo::resetAnimation(){
	this->animation_time_offset=0;
	this->animation_direction=1;
}

long NONS_AnimationInfo::advanceAnimation(ulong msecs){
	if (!this->frame_ends.size() || !this->animation_direction || this->loop_type==NO_CYCLE)
		return -1;
	long original=this->getCurrentAnimationFrame();
	if (this->animation_direction>0)
		this->animation_time_offset+=msecs;
	else if (msecs>this->animation_time_offset){
		this->animation_time_offset=msecs-this->animation_time_offset+this->frame_ends.front();
		this->animation_direction=1;
	}else
		this->animation_time_offset-=msecs;
	long updated=this->getCurrentAnimationFrame();
	if (updated==original)
		return -1;
	if (updated>=0)
		return updated;
	switch (this->loop_type){
		case SAWTOOTH_WAVE_CYCLE:
			this->animation_time_offset%=this->frame_ends.back();
			break;
		case SINGLE_CYCLE:
			this->animation_direction=0;
			return -1;
		case TRIANGLE_WAVE_CYCLE:
			if (this->frame_ends.size()==1)
				this->animation_time_offset=
					this->frame_ends.back()*(this->animation_length-1)-
					(this->animation_time_offset-this->frame_ends.back()*this->animation_length)
					-1;
			else
				this->animation_time_offset=
					this->frame_ends.back()-
					this->animation_time_offset-1-
					(this->frame_ends.back()-this->frame_ends[this->frame_ends.size()-2]);
			this->animation_direction=-1;
			break;
	}
	return this->getCurrentAnimationFrame();
}

long NONS_AnimationInfo::getCurrentAnimationFrame(){
	if (this->frame_ends.size()==1){
		ulong frame=this->animation_time_offset/this->frame_ends.front();
		if (frame>=this->animation_length)
			return -1;
		return frame;
	}
	for (size_t a=0;a<this->frame_ends.size();a++)
		if (this->animation_time_offset<this->frame_ends[a])
			return a;
	return -1;
}

NONS_Image::NONS_Image(){
	this->age=0;
	this->image=0;
	this->refCount=0;
}

NONS_Image::NONS_Image(const NONS_AnimationInfo *anim,const NONS_Image *primary,const NONS_Image *secondary,optim_t *rects){
	this->age=0;
	this->image=0;
	this->refCount=0;
	if (!anim || !primary || anim->method==NONS_AnimationInfo::SEPARATE_MASK && !secondary)
		return;
	this->animation=*anim;
	if (this->animation.method==NONS_AnimationInfo::PARALLEL_MASK)
		this->image=makeSurface(primary->image->w/2,primary->image->h,32);
	else
		this->image=makeSurface(primary->image->w,primary->image->h,32);
	uchar *pixels0=(uchar *)this->image->pixels;
	uchar Roffset0=(this->image->format->Rshift)>>3;
	uchar Goffset0=(this->image->format->Gshift)>>3;
	uchar Boffset0=(this->image->format->Bshift)>>3;
	uchar Aoffset0=(this->image->format->Ashift)>>3;
	unsigned advance0=this->image->format->BytesPerPixel,
		pitch0=this->image->pitch;
	uchar *pixels1=(uchar *)primary->image->pixels;
	uchar Roffset1=(primary->image->format->Rshift)>>3;
	uchar Goffset1=(primary->image->format->Gshift)>>3;
	uchar Boffset1=(primary->image->format->Bshift)>>3;
	unsigned advance1=primary->image->format->BytesPerPixel,
		pitch1=primary->image->pitch;
	switch (this->animation.method){
		case NONS_AnimationInfo::LEFT_UP:
		case NONS_AnimationInfo::RIGHT_UP:
			{
				SDL_LockSurface(this->image);
				SDL_LockSurface(primary->image);
				uchar chromaR,chromaG,chromaB;
				if (this->animation.method==NONS_AnimationInfo::LEFT_UP){
					chromaR=pixels1[Roffset1];
					chromaG=pixels1[Goffset1];
					chromaB=pixels1[Boffset1];
				}else{
					uchar *top_right=pixels1+pitch1-advance1;
					chromaR=top_right[Roffset1];
					chromaG=top_right[Goffset1];
					chromaB=top_right[Boffset1];
				}
				long w=this->image->w,h=this->image->h;
				uchar *pos0=pixels0,
					*pos1=pixels1;
				for (long y=0;y<h;y++){
					uchar *pos00=pos0,
						*pos10=pos1;
					for (long x=0;x<w;x++){
						uchar r=pos1[Roffset1],
							g=pos1[Goffset1],
							b=pos1[Boffset1];
						pos0[Roffset0]=r;
						pos0[Goffset0]=g;
						pos0[Boffset0]=b;
						if (r==chromaR && g==chromaG && b==chromaB)
							pos0[Aoffset0]=0;
						else
							pos0[Aoffset0]=255;
						pos0+=advance0;
						pos1+=advance1;
					}
					pos0=pos00+pitch0;
					pos1=pos10+pitch1;
				}
				SDL_UnlockSurface(this->image);
				SDL_UnlockSurface(primary->image);
			}
			break;
		case NONS_AnimationInfo::COPY_TRANS:
			manualBlit(primary->image,0,this->image,0);
			break;
		case NONS_AnimationInfo::SEPARATE_MASK:
			{
				SDL_LockSurface(this->image);
				SDL_LockSurface(primary->image);
				SDL_LockSurface(secondary->image);
				uchar *pixels2=(uchar *)secondary->image->pixels;
				//uchar Roffset2=(secondary->image->format->Rshift)>>3;
				//uchar Goffset2=(secondary->image->format->Gshift)>>3;
				uchar Boffset2=(secondary->image->format->Bshift)>>3;
				unsigned advance2=secondary->image->format->BytesPerPixel,
					pitch2=secondary->image->pitch;
				long w=this->image->w,
					h=this->image->h,
					w2=secondary->image->w,
					h2=secondary->image->h;
				uchar *pos0=pixels0,
					*pos1=pixels1,
					*pos2=pixels2;
				for (long y=0;y<h;y++){
					if (y && !(y%h2))
						pos2=pixels2;
					uchar *pos00=pos0,
						*pos10=pos1,
						*pos20=pos2;
					for (long x=0;x<w;x++){
						if (x && !(x%w2))
							pos2=pos20;
						pos0[Roffset0]=pos1[Roffset1];
						pos0[Goffset0]=pos1[Goffset1];
						pos0[Boffset0]=pos1[Boffset1];
						pos0[Aoffset0]=pos2[Boffset2]^0xFF;
						pos0+=advance0;
						pos1+=advance1;
						pos2+=advance2;
					}
					pos0=pos00+pitch0;
					pos1=pos10+pitch1;
					pos2=pos20+pitch2;
				}
				SDL_UnlockSurface(this->image);
				SDL_UnlockSurface(primary->image);
				SDL_UnlockSurface(secondary->image);
			}
			break;
		case NONS_AnimationInfo::PARALLEL_MASK:
			{
				SDL_LockSurface(this->image);
				SDL_LockSurface(primary->image);
				long w=this->image->w,
					h=this->image->h,
					w1=this->image->w/anim->animation_length,
					h2=primary->image->h;
				uchar *pos0=pixels0,
					*pos_pri=pixels1,
					*pos_mask=pixels1+w1*advance1;
				for (long y=0;y<h;y++){
					if (y && !(y%h2)){
						pos_pri=pixels1;
						pos_mask=pixels1+w1*advance1;
					}
					uchar *pos00=pos0,
						*pos_pri0=pos_pri,
						*pos_mask0=pos_mask;
					for (long x=0;x<w;x++){
						if (x && !(x%w1)){
							pos_pri+=w1*advance1;
							pos_mask+=w1*advance1;
						}
						pos0[Roffset0]=pos_pri[Roffset1];
						pos0[Goffset0]=pos_pri[Goffset1];
						pos0[Boffset0]=pos_pri[Boffset1];
						pos0[Aoffset0]=pos_mask[Boffset1]^0xFF;
						pos0+=advance0;
						pos_pri+=advance1;
						pos_mask+=advance1;
					}
					pos0=pos00+pitch0;
					pos_pri=pos_pri0+pitch1;
					pos_mask=pos_mask0+pitch1;
				}
				SDL_UnlockSurface(this->image);
				SDL_UnlockSurface(primary->image);
			}
			break;
	}
	this->image->clip_rect.w/=(Uint16)this->animation.animation_length;
	for (ulong a=0;a<this->animation.animation_length-1;a++){
		for (ulong b=a+1;a<this->animation.animation_length;a++){
			std::pair<ulong,ulong> p(a,b);
			if (this->optimized_updates.find(p)!=this->optimized_updates.end())
				continue;
			SDL_Rect temp=this->getUpdateRect(a,b);
			this->optimized_updates[p]=temp;
			this->optimized_updates[std::pair<ulong,ulong>(b,a)]=temp;
		}
	}
	if (!!rects)
		*rects=this->optimized_updates;
}

NONS_Image::~NONS_Image(){
	if (this->image && !this->refCount)
		SDL_FreeSurface(this->image);
}

#undef LoadImage

SDL_Surface *NONS_Image::LoadImage(const std::wstring &string,const uchar *buffer,ulong bufferSize){
	if (!buffer || !bufferSize || this->image && this->refCount)
		return 0;
	if (this->image)
		SDL_FreeSurface(this->image);
	SDL_RWops *rwops=SDL_RWFromMem((void *)buffer,bufferSize);
	SDL_Surface *surface=IMG_Load_RW(rwops,0);
	this->image=makeSurface(surface->w,surface->h,32);
	if (!!surface && surface->format->BitsPerPixel<24)
		SDL_BlitSurface(surface,0,this->image,0);
	else
		manualBlit(surface,0,this->image,0);
	SDL_FreeSurface(surface);
	SDL_FreeRW(rwops);
	this->animation.parse(string);
	this->refCount=0;
	return this->image;
}

SDL_Rect NONS_Image::getUpdateRect(ulong from,ulong to){
	if (!this->image || this->animation.animation_length==1)
		return SDL_Rect();
	SDL_LockSurface(this->image);
	uchar *pixels=(uchar *)this->image->pixels;
	uchar Roffset=(this->image->format->Rshift)>>3;
	uchar Goffset=(this->image->format->Gshift)>>3;
	uchar Boffset=(this->image->format->Bshift)>>3;
	uchar Aoffset=(this->image->format->Ashift)>>3;
	unsigned advance=this->image->format->BytesPerPixel,
		pitch=this->image->pitch;
	uchar *first=pixels+from*this->image->clip_rect.w*advance,
		*second=pixels+to*this->image->clip_rect.w*advance;
	ulong w=this->image->clip_rect.w,
		h=this->image->clip_rect.h,
		minx=w,
		maxx=0,
		miny=h,
		maxy=0;
	for (ulong y=0;y<h;y++){
		uchar *first0=first,
			*second0=second;
		for (ulong x=0;x<w;x++){
			short rgba0=(short(first[Roffset])+short(first[Goffset])+short(first[Boffset])+short(first[Aoffset]))/4;
			short rgba1=(short(second[Roffset])+short(second[Goffset])+short(second[Boffset])+short(second[Aoffset]))/4;
			short diff=rgba0-rgba1;
			if (diff<=-8 || diff>=8){
				if (x<minx)
					minx=x;
				if (x>maxx)
					maxx=x;
				if (y<miny)
					miny=y;
				if (y>maxy)
					maxy=y;
			}
			first+=advance;
			second+=advance;
		}
		first=first0+pitch;
		second=second0+pitch;
	}
	SDL_UnlockSurface(this->image);
	SDL_Rect ret={
		(Sint16)minx,
		(Sint16)miny,
		Uint16(maxx-minx+1),
		Uint16(maxy-miny+1)
	};
	return ret;
}

#define LOG_FILENAME_OLD L"NScrflog.dat"
#define LOG_FILENAME_NEW L"nonsflog.dat"

NONS_ImageLoader *ImageLoader=0;

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

SDL_Surface *NONS_ImageLoader::fetchSprite(const std::wstring &string,optim_t *rects){
	NONS_AnimationInfo anim(string);
	if (!anim.valid)
		return 0;
	long bestFit=-1,maskMatch=-1,fileMatch=-1;
	for (ulong a=0;a<this->imageCache.size() && bestFit<0;a++){
		NONS_Image *el=this->imageCache[a];
		if (el){
			if (el->animation.method==NONS_AnimationInfo::COPY_TRANS){
				if (el->animation.getFilename()==anim.getFilename())
					fileMatch=a;
				if (anim.method==NONS_AnimationInfo::SEPARATE_MASK && el->animation.getFilename()==anim.getMaskFilename())
					maskMatch=a;
			}
			if (el->animation.method==anim.method){
				if (anim.method==NONS_AnimationInfo::SEPARATE_MASK){
					if ((ulong)fileMatch==a && (ulong)maskMatch==a)
						bestFit=a;
				}else if ((ulong)fileMatch==a)
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
		ulong l;
		uchar *buffer=this->archive->getFileBuffer(anim.getFilename(),l);
		if (!buffer)
			return 0;
		primary=new NONS_Image;
		primary->LoadImage(anim.getFilename(),buffer,l);
		this->filelog.addString(anim.getFilename());
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
		ulong l;
		uchar *buffer=this->archive->getFileBuffer(anim.getMaskFilename(),l);
		if (!buffer)
			return 0;
		secondary=new NONS_Image;
		secondary->LoadImage(anim.getMaskFilename(),buffer,l);
		this->filelog.addString(anim.getMaskFilename());
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
	if (this->maxCacheSize>0 && cachesize>(ulong)this->maxCacheSize)
		this->freeOldest(cachesize-(ulong)this->maxCacheSize);
	return 1;
}

bool NONS_ImageLoader::unfetchImage(SDL_Surface *which){
	if (!which)
		return 0;
	for (ulong a=0;a<this->imageCache.size();a++){
		NONS_Image *temp=this->imageCache[a];
		if (temp && temp->image==which){
			temp->refCount--;
			if (!temp->refCount){
				if (!this->maxCacheSize){
					delete temp;
					this->imageCache[a]=0;
				}else
					temp->age++;
			}
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
