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
*     * Helios' name may not be used to endorse or promote products derived from
*       this software without specific prior written permission.
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

#ifndef NONS_GFX_CPP
#define NONS_GFX_CPP

#include "GFX.h"
#include "../../Functions.h"
#include "../IOFunctions.h"
#include "../../Globals.h"
#include <cmath>

#ifdef NONS_PARALLELIZE
//(Parallelized surface function)
struct PSF_parameters{
	SDL_Surface *src;
	SDL_Rect *srcRect;
	SDL_Surface *dst;
	SDL_Rect *dstRect;
	uchar alpha;
	SDL_Color color;
};
#endif

ulong NONS_GFX::effectblank=0;

NONS_GFX::NONS_GFX(ulong effect,ulong duration,wchar_t *rule){
	this->effect=effect;
	this->duration=duration;
	if (rule)
		this->rule=copyWString(rule);
	else
		this->rule=0;
	this->type=TRANSITION;
	this->stored=0;
	this->color.r=0;
	this->color.g=0;
	this->color.b=0;
	this->color.unused=0;
}

NONS_GFX::NONS_GFX(const NONS_GFX &b){
	this->effect=b.effect;
	this->duration=b.duration;
	this->rule=copyWString(b.rule);
	this->type=b.type;
	this->stored=b.stored;
	this->color=b.color;
}

NONS_GFX &NONS_GFX::operator=(const NONS_GFX &b){
	this->effect=b.effect;
	this->duration=b.effect;
	if (this->rule)
		delete[] this->rule;
	this->rule=copyWString(b.rule);
	this->type=b.type;
	this->stored=b.stored;
	return *this;
}

NONS_GFX::~NONS_GFX(){
	if (this->rule)
		delete[] this->rule;
}

ErrorCode NONS_GFX::callEffect(ulong number,long duration,wchar_t *rule,SDL_Surface *src,SDL_Surface *dst0,NONS_VirtualScreen *dst){
	NONS_GFX *effect=new NONS_GFX(number,duration,rule);
	ErrorCode ret=effect->call(src,dst0,dst);
	delete effect;
	return ret;
}

ErrorCode NONS_GFX::call(SDL_Surface *src,SDL_Surface *dst0,NONS_VirtualScreen *dst){
	//Unused:
	//ulong t0=SDL_GetTicks();
	SDL_Surface *ruleFile=0;
	if (this->rule)
		ruleFile=ImageLoader->fetchSprite(this->rule,this->rule,NO_ALPHA);
	if (this->type==TRANSITION){
		switch (this->effect){
			case 0:
				this->effectNothing(src,ruleFile,dst);
				break;
			case 1:
				this->effectOnlyUpdate(src,ruleFile,dst);
				break;
			case 2:
				this->effectRshutter(src,ruleFile,dst);
				break;
			case 3:
				this->effectLshutter(src,ruleFile,dst);
				break;
			case 4:
				this->effectDshutter(src,ruleFile,dst);
				break;
			case 5:
				this->effectUshutter(src,ruleFile,dst);
				break;
			case 6:
				this->effectRcurtain(src,ruleFile,dst);
				break;
			case 7:
				this->effectLcurtain(src,ruleFile,dst);
				break;
			case 8:
				this->effectDcurtain(src,ruleFile,dst);
				break;
			case 9:
				this->effectUcurtain(src,ruleFile,dst);
				break;
			case 10:
				this->effectCrossfade(src,ruleFile,dst);
				break;
			case 11:
				this->effectRscroll(src,ruleFile,dst);
				break;
			case 12:
				this->effectLscroll(src,ruleFile,dst);
				break;
			case 13:
				this->effectDscroll(src,ruleFile,dst);
				break;
			case 14:
				this->effectUscroll(src,ruleFile,dst);
				break;
			case 15:
				this->effectHardMask(src,ruleFile,dst);
				break;
			case 16:
				this->effectMosaicIn(src,ruleFile,dst);
				break;
			case 17:
				this->effectMosaicOut(src,ruleFile,dst);
				break;
			case 18:
				this->effectSoftMask(src,ruleFile,dst);
				break;
			default:
				if (ruleFile)
					SDL_FreeSurface(ruleFile);
				return NONS_NO_EFFECT;
		}
	}else{
		switch (this->effect){
			case 0:
				this->effectMonochrome(src,dst0);
				break;
			case 1:
				this->effectNegative(src,dst0);
				break;
			case 2:
				this->effectNegativeMono(src,dst0);
				break;
			default:
				if (ruleFile)
					SDL_FreeSurface(ruleFile);
				return NONS_NO_EFFECT;
		}
	}
	if (ruleFile)
		ImageLoader->unfetchImage(ruleFile);
	//Unused:
	//ulong t1=SDL_GetTicks();
	return NONS_NO_ERROR;
}

void NONS_GFX::effectNothing(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
}

void NONS_GFX::effectOnlyUpdate(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	if (!src0 || !dst)
		return;
	LOCKSCREEN;
	manualBlit(src0,0,dst->virtualScreen,0);
	UNLOCKSCREEN;
	dst->updateWholeScreen();
}

void NONS_GFX::effectRshutter(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	UNLOCKSCREEN;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	long shutterW=src0->w/40;
	SDL_Rect rect={0,0,1,src0->h};
	float delay=float(this->duration)/float(shutterW);
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=0;a<shutterW;a++){
		idealtimepos+=delay;
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a<shutterW-1){
			rect.w++;
			continue;
		}
		rect.x=a-rect.w+1;
		LOCKSCREEN;
		for (long b=0;b<40;b++){
			SDL_BlitSurface(src0,&rect,dst->virtualScreen,&rect);
			rect.x+=shutterW;
		}
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		rect.w=1;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
	}
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

void NONS_GFX::effectLshutter(SDL_Surface *src0,SDL_Surface *scr1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	UNLOCKSCREEN;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	long shutterW=src0->w/40;
	SDL_Rect rect={0,0,1,src0->h};
	float delay=float(this->duration)/float(shutterW);
	//Unused:
	long //realtimepos=0,
		idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=shutterW-1;a>=0;a--){
		idealtimepos+=delay;
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a>0){
			rect.w++;
			continue;
		}
		rect.x=a;
		LOCKSCREEN;
		for (long b=0;b<40;b++){
			SDL_BlitSurface(src0,&rect,dst->virtualScreen,&rect);
			rect.x+=shutterW;
		}
		dst->updateWithoutLock();
		LOCKSCREEN;
		rect.w=1;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
	}
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

void NONS_GFX::effectDshutter(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	UNLOCKSCREEN;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	long shutterH=src0->h/30;
	SDL_Rect rect={0,0,src0->w,1};
	float delay=float(this->duration)/float(shutterH);
	//Unused:
	long //realtimepos=0,
		idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=0;a<shutterH;a++){
		idealtimepos+=delay;
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a<shutterH-1){
			rect.h++;
			continue;
		}
		rect.y=a-rect.h+1;
		LOCKSCREEN;
		for (long b=0;b<30;b++){
			SDL_BlitSurface(src0,&rect,dst->virtualScreen,&rect);
			rect.y+=shutterH;
		}
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		rect.h=1;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
	}
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

void NONS_GFX::effectUshutter(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	UNLOCKSCREEN;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	long shutterH=src0->h/30;
	SDL_Rect rect={0,0,src0->w,1};
	float delay=float(this->duration)/float(shutterH);
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=shutterH-1;a>=0;a--){
		idealtimepos+=delay;
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a>0){
			rect.h++;
			continue;
		}
		rect.y=a;
		LOCKSCREEN;
		for (long b=0;b<30;b++){
			SDL_BlitSurface(src0,&rect,dst->virtualScreen,&rect);
			rect.y+=shutterH;
		}
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		rect.h=1;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
	}
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

void NONS_GFX::effectRcurtain(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	UNLOCKSCREEN;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	long shutterW=(long)sqrt(double(src0->w));
	SDL_Rect rect={0,0,1,src0->w};
	float delay=float(this->duration)/float(shutterW*2);
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=0;a<shutterW*2;a++){
		idealtimepos+=delay;
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a<shutterW-1){
			rect.w++;
			continue;
		}
		rect.x=a-rect.w+1;
		LOCKSCREEN;
		for (long b=0;b<=a;b++){
			manualBlit(src0,&rect,dst->virtualScreen,&rect);
			rect.x+=shutterW;
		}
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		rect.w=1;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
	}
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

void NONS_GFX::effectLcurtain(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	UNLOCKSCREEN;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	long w=src0->w,
		shutterW=(long)sqrt(double(w));
	SDL_Rect rect={0,0,1,src0->h};
	float delay=float(this->duration)/float(shutterW*2);
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=0;a<shutterW*2;a++){
		idealtimepos+=delay;
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a<shutterW-1){
			rect.w++;
			continue;
		}
		rect.x=(w-a)/*-rect.w+1*/;
		LOCKSCREEN;
		for (long b=0;b<=a;b++){
			manualBlit(src0,&rect,dst->virtualScreen,&rect);
			rect.x-=shutterW;
		}
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		rect.w=1;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
	}
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

void NONS_GFX::effectDcurtain(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	UNLOCKSCREEN;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	long shutterH=(long)sqrt(double(src0->h));
	SDL_Rect rect={0,0,src0->w,1};
	float delay=float(this->duration)/float(shutterH*2);
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=0;a<shutterH*2;a++){
		idealtimepos+=delay;
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a<shutterH-1){
			rect.h++;
			continue;
		}
		rect.y=a-rect.h+1;
		LOCKSCREEN;
		for (long b=0;b<=a;b++){
			manualBlit(src0,&rect,dst->virtualScreen,&rect);
			rect.y+=shutterH;
		}
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		rect.h=1;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
	}
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

void NONS_GFX::effectUcurtain(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	UNLOCKSCREEN;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	long h=src0->h,
		shutterH=(long)sqrt(double(h));
	SDL_Rect rect={0,0,src0->w,1};
	float delay=float(this->duration)/float(shutterH*2);
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=0;a<shutterH*2;a++){
		idealtimepos+=delay;
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a<shutterH-1){
			rect.h++;
			continue;
		}
		rect.y=(h-a)/*-rect.h+1*/;
		LOCKSCREEN;
		for (long b=0;b<=a;b++){
			manualBlit(src0,&rect,dst->virtualScreen,&rect);
			rect.y-=shutterH;
		}
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		rect.h=1;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
	}
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

void NONS_GFX::effectCrossfade(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	UNLOCKSCREEN;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	const long step=1;
	LOCKSCREEN;
	SDL_Surface *copyDst=SDL_CreateRGBSurface(
		SDL_HWSURFACE|SDL_SRCALPHA,
		dst->virtualScreen->w,
		dst->virtualScreen->h,
		32,
		rmask,gmask,bmask,amask);
	manualBlit(dst->virtualScreen,0,copyDst,0);
	UNLOCKSCREEN;
	float delay=float(this->duration)/float(256/step);
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=step;a<256;a+=step){
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a<255){
			idealtimepos+=delay;
			continue;
		}
		LOCKSCREEN;
		manualBlit(copyDst,0,dst->virtualScreen,0);
		manualBlit(src0,0,dst->virtualScreen,0,a);
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
		idealtimepos+=delay;
	}
	SDL_FreeSurface(copyDst);
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

void NONS_GFX::effectRscroll(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	UNLOCKSCREEN;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	float delay=float(this->duration)/float(src0->w);
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	SDL_Rect srcrect={0,0,src0->w,src0->h},
		dstrect=srcrect;
	for (long a=src0->w-1;a>=0;a--){
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a>0){
			idealtimepos+=delay;
			continue;
		}
		srcrect.x=a;
		LOCKSCREEN;
		manualBlit(src0,&srcrect,dst->virtualScreen,&dstrect);
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
		idealtimepos+=delay;
	}
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

void NONS_GFX::effectLscroll(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	UNLOCKSCREEN;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	float delay=float(this->duration)/float(src0->w);
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	SDL_Rect srcrect={0,0,src0->w,src0->h},
		dstrect=srcrect;
	for (long a=src0->w-1;a>=0;a--){
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a>0){
			idealtimepos+=delay;
			continue;
		}
		dstrect.x=a;
		LOCKSCREEN;
		manualBlit(src0,&srcrect,dst->virtualScreen,&dstrect);
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
		idealtimepos+=delay;
	}
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

void NONS_GFX::effectDscroll(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	UNLOCKSCREEN;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	float delay=float(this->duration)/float(src0->h);
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	SDL_Rect srcrect={0,0,src0->w,src0->h},
		dstrect=srcrect;
	for (long a=src0->h-1;a>=0;a--){
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a>0){
			idealtimepos+=delay;
			continue;
		}
		srcrect.y=a;
		LOCKSCREEN;
		manualBlit(src0,&srcrect,dst->virtualScreen,&dstrect);
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
		idealtimepos+=delay;
	}
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

void NONS_GFX::effectUscroll(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	UNLOCKSCREEN;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	float delay=float(this->duration)/float(src0->h);
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	SDL_Rect srcrect={0,0,src0->w,src0->h},
		dstrect=srcrect;
	for (long a=src0->h-1;a>=0;a--){
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a>0){
			idealtimepos+=delay;
			continue;
		}
		dstrect.y=a;
		LOCKSCREEN;
		manualBlit(src0,&srcrect,dst->virtualScreen,&dstrect);
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
		idealtimepos+=delay;
	}
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

#ifndef NONS_PARALLELIZE
void NONS_GFX::effectHardMask(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !src1 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	if (CURRENTLYSKIPPING){
		UNLOCKSCREEN;
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	SDL_Surface *copyMask=SDL_CreateRGBSurface(
		SDL_HWSURFACE|SDL_SRCALPHA,
		dst->virtualScreen->w,
		dst->virtualScreen->h,
		32,
		rmask,gmask,bmask,amask);
	SDL_Rect srcrect={0,0,src1->w,src1->h},
		dstrect=srcrect;
	for (dstrect.y=0;dstrect.y<dst->virtualScreen->h;dstrect.y+=srcrect.h)
		for (dstrect.x=0;dstrect.x<dst->virtualScreen->w;dstrect.x+=srcrect.w)
			manualBlit(src1,&srcrect,copyMask,&dstrect);
	src1=copyMask;
	UNLOCKSCREEN;
	float delay=float(this->duration)/256.0;
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=0;a<256;a++){
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a<255){
			idealtimepos+=delay;
			continue;
		}
		{
			long w0=dst->virtualScreen->w,h0=dst->virtualScreen->h;
			LOCKSCREEN;
			SDL_LockSurface(src0);
			SDL_LockSurface(src1);
			SDL_LockSurface(dst->virtualScreen);

			uchar *pos0=(uchar *)src0->pixels;
			uchar *pos1=(uchar *)src1->pixels;
			uchar *pos2=(uchar *)dst->virtualScreen->pixels;

			uchar Roffset0=(src0->format->Rshift)>>3;
			uchar Goffset0=(src0->format->Gshift)>>3;
			uchar Boffset0=(src0->format->Bshift)>>3;

			uchar Boffset1=(src1->format->Bshift)>>3;

			uchar Roffset2=(dst->virtualScreen->format->Rshift)>>3;
			uchar Goffset2=(dst->virtualScreen->format->Gshift)>>3;
			uchar Boffset2=(dst->virtualScreen->format->Bshift)>>3;

			unsigned advance0=src0->format->BytesPerPixel;
			unsigned advance1=src1->format->BytesPerPixel;
			unsigned advance2=dst->virtualScreen->format->BytesPerPixel;

			for (int y0=0;y0<h0;y0++){
				for (int x0=0;x0<w0;x0++){
					uchar r0=pos0[Roffset0];
					uchar g0=pos0[Goffset0];
					uchar b0=pos0[Boffset0];

					uchar b1=pos1[Boffset1];
					uchar *b12=pos1+Boffset1;

					uchar *r2=pos2+Roffset2;
					uchar *g2=pos2+Goffset2;
					uchar *b2=pos2+Boffset2;

					if (b1<=a){
						*r2=r0;
						*g2=g0;
						*b2=b0;
						*b12=0xFF;
					}

					pos0+=advance0;
					pos1+=advance1;
					pos2+=advance2;
				}
			}
			SDL_UnlockSurface(dst->virtualScreen);
			SDL_UnlockSurface(src1);
			SDL_UnlockSurface(src0);
		}
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
		idealtimepos+=delay;
	}
	SDL_FreeSurface(copyMask);
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}
#else
struct EHM_parameters{
	uchar *pos0,
		*pos1,
		*pos2;
	uchar Roffset0,
		Goffset0,
		Boffset0,
		Boffset1,
		Roffset2,
		Goffset2,
		Boffset2;
	ulong advance0,
		advance1,
		advance2,
		pitch0,
		pitch1,
		pitch2;
	int w,h;
	long a;
};

int effectHardMask_threaded(void *parameters){
	EHM_parameters *p=(EHM_parameters *)parameters;
	uchar *pos0=p->pos0,
		*pos1=p->pos1,
		*pos2=p->pos2;
	for (int y0=0;y0<p->h;y0++){
		for (int x0=0;x0<p->w;x0++){
			uchar r0=pos0[p->Roffset0];
			uchar g0=pos0[p->Goffset0];
			uchar b0=pos0[p->Boffset0];

			uchar b1=pos1[p->Boffset1];
			uchar *b12=pos1+p->Boffset1;

			uchar *r2=pos2+p->Roffset2;
			uchar *g2=pos2+p->Goffset2;
			uchar *b2=pos2+p->Boffset2;

			if (b1<=p->a){
				*r2=r0;
				*g2=g0;
				*b2=b0;
				*b12=0xFF;
			}

			pos0+=p->advance0;
			pos1+=p->advance1;
			pos2+=p->advance2;
		}
	}
	return 0;
}

void NONS_GFX::effectHardMask(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !src1 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	if (CURRENTLYSKIPPING){
		UNLOCKSCREEN;
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	SDL_Surface *copyMask=SDL_CreateRGBSurface(
		SDL_HWSURFACE|SDL_SRCALPHA,
		dst->virtualScreen->w,
		dst->virtualScreen->h,
		32,
		rmask,gmask,bmask,amask);
	SDL_Rect srcrect={0,0,src1->w,src1->h},
		dstrect=srcrect;
	//copy the rule as a tile
	for (dstrect.y=0;dstrect.y<dst->virtualScreen->h;dstrect.y+=srcrect.h)
		for (dstrect.x=0;dstrect.x<dst->virtualScreen->w;dstrect.x+=srcrect.w)
			manualBlit(src1,&srcrect,copyMask,&dstrect);
	src1=copyMask;
	//prepare for threading
	SDL_Thread **threads=new SDL_Thread *[cpu_count];
	EHM_parameters *parameters=new EHM_parameters[cpu_count];
	ulong division=float(dst->virtualScreen->h)/float(cpu_count);
	ulong total=0;
	parameters[0].pos0=(uchar *)src0->pixels;
	parameters[0].pos1=(uchar *)src1->pixels;
	parameters[0].pos2=(uchar *)dst->virtualScreen->pixels;
	parameters[0].Roffset0=(src0->format->Rshift)>>3;
	parameters[0].Goffset0=(src0->format->Gshift)>>3;
	parameters[0].Boffset0=(src0->format->Bshift)>>3;
	parameters[0].Boffset1=(src1->format->Bshift)>>3;
	parameters[0].Roffset2=(dst->virtualScreen->format->Rshift)>>3;
	parameters[0].Goffset2=(dst->virtualScreen->format->Gshift)>>3;
	parameters[0].Boffset2=(dst->virtualScreen->format->Bshift)>>3;
	parameters[0].advance0=src0->format->BytesPerPixel;
	parameters[0].advance1=src1->format->BytesPerPixel;
	parameters[0].advance2=dst->virtualScreen->format->BytesPerPixel;
	parameters[0].pitch0=src0->pitch;
	parameters[0].pitch1=src1->pitch;
	parameters[0].pitch2=dst->virtualScreen->pitch;
	parameters[0].w=dst->virtualScreen->w;
	parameters[0].h=dst->virtualScreen->h;
	for (ushort a=0;a<cpu_count;a++){
		parameters[a]=parameters[0];
		parameters[a].pos0+=parameters[a].pitch0*Sint16(a*division);
		parameters[a].pos1+=parameters[a].pitch1*Sint16(a*division);
		parameters[a].pos2+=parameters[a].pitch2*Sint16(a*division);
		parameters[a].h=Sint16(division);
		total+=parameters[a].h;
	}
	parameters[cpu_count].h+=dst->virtualScreen->h-total;
	UNLOCKSCREEN;

	float delay=float(this->duration)/256.0;
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=0;a<256;a++){
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a<255){
			idealtimepos+=delay;
			continue;
		}

		LOCKSCREEN;
		SDL_LockSurface(src0);
		SDL_LockSurface(src1);
		SDL_LockSurface(dst->virtualScreen);

		for (ushort b=0;b<cpu_count;b++)
			parameters[b].a=a;
		for (ushort b=0;b<cpu_count;b++)
			threads[b]=SDL_CreateThread(effectHardMask_threaded,parameters+b);
		for (ushort b=0;b<cpu_count;b++)
			SDL_WaitThread(threads[b],0);

		SDL_UnlockSurface(dst->virtualScreen);
		SDL_UnlockSurface(src1);
		SDL_UnlockSurface(src0);
		dst->updateWithoutLock();
		UNLOCKSCREEN;

		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
		idealtimepos+=delay;
	}
	SDL_FreeSurface(copyMask);
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}
#endif

#ifndef NONS_PARALLELIZE
void NONS_GFX::effectSoftMask(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !src1 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	if (CURRENTLYSKIPPING){
		UNLOCKSCREEN;
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	SDL_Surface *copyDst=SDL_CreateRGBSurface(
		SDL_HWSURFACE|SDL_SRCALPHA,
		dst->virtualScreen->w,
		dst->virtualScreen->h,
		32,
		rmask,gmask,bmask,amask);
	manualBlit(dst->virtualScreen,0,copyDst,0);
	SDL_Surface *copyMask=SDL_CreateRGBSurface(
		SDL_HWSURFACE|SDL_SRCALPHA,
		dst->virtualScreen->w,
		dst->virtualScreen->h,
		32,
		rmask,gmask,bmask,amask);
	SDL_Rect srcrect={0,0,src1->w,src1->h},
		dstrect=srcrect;
	for (dstrect.y=0;dstrect.y<dst->virtualScreen->h;dstrect.y+=srcrect.h)
		for (dstrect.x=0;dstrect.x<dst->virtualScreen->w;dstrect.x+=srcrect.w)
			manualBlit(src1,&srcrect,copyMask,&dstrect);
	UNLOCKSCREEN;
	src1=copyMask;
	float delay=float(this->duration)/512.0;
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=0;a<512;a++){
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a<511){
			idealtimepos+=delay;
			continue;
		}
		LOCKSCREEN;
		manualBlit(copyDst,0,dst->virtualScreen,0);
		{
			long w0=dst->virtualScreen->w,h0=dst->virtualScreen->h;
			SDL_LockSurface(src0);
			SDL_LockSurface(src1);
			SDL_LockSurface(dst->virtualScreen);

			uchar *pos0=(uchar *)src0->pixels;
			uchar *pos1=(uchar *)src1->pixels;
			uchar *pos2=(uchar *)dst->virtualScreen->pixels;

			uchar Roffset0=(src0->format->Rshift)>>3;
			uchar Goffset0=(src0->format->Gshift)>>3;
			uchar Boffset0=(src0->format->Bshift)>>3;

			uchar Boffset1=(src1->format->Bshift)>>3;

			uchar Roffset2=(dst->virtualScreen->format->Rshift)>>3;
			uchar Goffset2=(dst->virtualScreen->format->Gshift)>>3;
			uchar Boffset2=(dst->virtualScreen->format->Bshift)>>3;

			unsigned advance0=src0->format->BytesPerPixel;
			unsigned advance1=src1->format->BytesPerPixel;
			unsigned advance2=dst->virtualScreen->format->BytesPerPixel;

			for (int y0=0;y0<h0;y0++){
				for (int x0=0;x0<w0;x0++){
					short r0=pos0[Roffset0];
					short g0=pos0[Goffset0];
					short b0=pos0[Boffset0];

					uchar b1=pos1[Boffset1];
					uchar *b12=pos1+Boffset1;

					uchar *r2=pos2+Roffset2;
					uchar *g2=pos2+Goffset2;
					uchar *b2=pos2+Boffset2;

					if (long(b1)<=a){
						short alpha=a-b1;
						short deltar=r0-*r2;
						short deltag=g0-*g2;
						short deltab=b0-*b2;
						if (alpha<0) alpha=0;
						if (alpha>255) alpha=255;
						(*r2)+=(deltar*alpha)/255;
						(*g2)+=(deltag*alpha)/255;
						(*b2)+=(deltab*alpha)/255;
						if (long(b1)<a-255)
							*b12=0;
					}

					pos0+=advance0;
					pos1+=advance1;
					pos2+=advance2;
				}
			}
			SDL_UnlockSurface(dst->virtualScreen);
			SDL_UnlockSurface(src1);
			SDL_UnlockSurface(src0);
		}
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
		idealtimepos+=delay;
	}
	SDL_FreeSurface(copyMask);
	SDL_FreeSurface(copyDst);
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}
#else
struct ESM_parameters{
	uchar *pos0,
		*pos1,
		*pos2;
	uchar Roffset0,
		Goffset0,
		Boffset0,
		Boffset1,
		Roffset2,
		Goffset2,
		Boffset2;
	ulong advance0,
		advance1,
		advance2,
		pitch0,
		pitch1,
		pitch2;
	int w,h;
	long a;
};

int effectSoftMask_threaded(void *parameters){
	EHM_parameters *p=(EHM_parameters *)parameters;
	uchar *pos0=p->pos0,
		*pos1=p->pos1,
		*pos2=p->pos2;
	for (int y0=0;y0<p->h;y0++){
		for (int x0=0;x0<p->w;x0++){
			short r0=pos0[p->Roffset0];
			short g0=pos0[p->Goffset0];
			short b0=pos0[p->Boffset0];

			uchar b1=pos1[p->Boffset1];
			uchar *b12=pos1+p->Boffset1;

			uchar *r2=pos2+p->Roffset2;
			uchar *g2=pos2+p->Goffset2;
			uchar *b2=pos2+p->Boffset2;

			if (long(b1)<=p->a){
				short alpha=p->a-b1;
				short deltar=r0-*r2;
				short deltag=g0-*g2;
				short deltab=b0-*b2;
				if (alpha<0) alpha=0;
				if (alpha>255) alpha=255;
				(*r2)+=(deltar*alpha)/255;
				(*g2)+=(deltag*alpha)/255;
				(*b2)+=(deltab*alpha)/255;
				if (long(b1)<p->a-255)
					*b12=0;
			}

			pos0+=p->advance0;
			pos1+=p->advance1;
			pos2+=p->advance2;
		}
	}
	return 0;
}

void NONS_GFX::effectSoftMask(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	if (!src0 || !src1 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h)
		return;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	LOCKSCREEN;
	SDL_Surface *copyDst=SDL_CreateRGBSurface(
		SDL_HWSURFACE|SDL_SRCALPHA,
		dst->virtualScreen->w,
		dst->virtualScreen->h,
		32,
		rmask,gmask,bmask,amask);
	manualBlit(dst->virtualScreen,0,copyDst,0);
	SDL_Surface *copyMask=SDL_CreateRGBSurface(
		SDL_HWSURFACE|SDL_SRCALPHA,
		dst->virtualScreen->w,
		dst->virtualScreen->h,
		32,
		rmask,gmask,bmask,amask);
	SDL_Rect srcrect={0,0,src1->w,src1->h},
		dstrect=srcrect;
	for (dstrect.y=0;dstrect.y<dst->virtualScreen->h;dstrect.y+=srcrect.h)
		for (dstrect.x=0;dstrect.x<dst->virtualScreen->w;dstrect.x+=srcrect.w)
			manualBlit(src1,&srcrect,copyMask,&dstrect);
	UNLOCKSCREEN;
	src1=copyMask;
	//prepare for threading
	SDL_Thread **threads=new SDL_Thread *[cpu_count];
	ESM_parameters *parameters=new ESM_parameters[cpu_count];
	ulong division=float(dst->virtualScreen->h)/float(cpu_count);
	ulong total=0;
	parameters[0].pos0=(uchar *)src0->pixels;
	parameters[0].pos1=(uchar *)src1->pixels;
	parameters[0].pos2=(uchar *)dst->virtualScreen->pixels;
	parameters[0].Roffset0=(src0->format->Rshift)>>3;
	parameters[0].Goffset0=(src0->format->Gshift)>>3;
	parameters[0].Boffset0=(src0->format->Bshift)>>3;
	parameters[0].Boffset1=(src1->format->Bshift)>>3;
	parameters[0].Roffset2=(dst->virtualScreen->format->Rshift)>>3;
	parameters[0].Goffset2=(dst->virtualScreen->format->Gshift)>>3;
	parameters[0].Boffset2=(dst->virtualScreen->format->Bshift)>>3;
	parameters[0].advance0=src0->format->BytesPerPixel;
	parameters[0].advance1=src1->format->BytesPerPixel;
	parameters[0].advance2=dst->virtualScreen->format->BytesPerPixel;
	parameters[0].pitch0=src0->pitch;
	parameters[0].pitch1=src1->pitch;
	parameters[0].pitch2=dst->virtualScreen->pitch;
	parameters[0].w=dst->virtualScreen->w;
	parameters[0].h=Sint16(division);
	total=parameters[0].h;
	for (ushort a=1;a<cpu_count;a++){
		parameters[a]=parameters[0];
		parameters[a].pos0+=parameters[a].pitch0*Sint16(a*division);
		parameters[a].pos1+=parameters[a].pitch1*Sint16(a*division);
		parameters[a].pos2+=parameters[a].pitch2*Sint16(a*division);
		total+=parameters[a].h;
	}
	parameters[cpu_count].h+=dst->virtualScreen->h-total;

	float delay=float(this->duration)/512.0;
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=0;a<512;a++){
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && a<511){
			idealtimepos+=delay;
			continue;
		}

		LOCKSCREEN;

		parameters[0].pos2=(uchar *)dst->virtualScreen->pixels;
		for (ushort b=1;b<cpu_count;b++)
			parameters[b].pos2=parameters[0].pos2+parameters[b].pitch2*Sint16(b*division);

		manualBlit(copyDst,0,dst->virtualScreen,0);
		long w0=dst->virtualScreen->w,h0=dst->virtualScreen->h;
		SDL_LockSurface(src0);
		SDL_LockSurface(src1);
		SDL_LockSurface(dst->virtualScreen);

		for (ushort b=0;b<cpu_count;b++)
			parameters[b].a=a;
		for (ushort b=0;b<cpu_count;b++)
			threads[b]=SDL_CreateThread(effectSoftMask_threaded,parameters+b);
		for (ushort b=0;b<cpu_count;b++)
			SDL_WaitThread(threads[b],0);

		SDL_UnlockSurface(dst->virtualScreen);
		SDL_UnlockSurface(src1);
		SDL_UnlockSurface(src0);
		dst->updateWithoutLock();
		UNLOCKSCREEN;

		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
		idealtimepos+=delay;
	}
	SDL_FreeSurface(copyMask);
	SDL_FreeSurface(copyDst);
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}
#endif

void NONS_GFX::effectMosaicIn(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	UNLOCKSCREEN;
	if (CURRENTLYSKIPPING){
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	float delay=float(this->duration)/10.0;
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=9;a>=0;a--){
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && lastT>5 && a>0){
			idealtimepos+=delay;
			continue;
		}
		LOCKSCREEN;
		if (a==0){
			manualBlit(src0,0,dst->virtualScreen,0);
		}else{
			SDL_LockSurface(src0);
			long advance=src0->format->BytesPerPixel;
			long pixelSize=1;
			for (short b=0;b<a;b++) pixelSize*=2;
			for (long y=0;y<dst->virtualScreen->h;y+=pixelSize){
				long tps=y+pixelSize-1>=dst->virtualScreen->h?dst->virtualScreen->h-y-1:pixelSize;
				for (long x=0;x<dst->virtualScreen->w;x+=pixelSize){
					SDL_Rect rect={x,y,pixelSize,pixelSize};
					uchar *pos0=(uchar *)src0->pixels;
					pos0+=src0->pitch*(y+tps-1)+advance*x;
					uchar Roffset0=(src0->format->Rshift)>>3;
					uchar Goffset0=(src0->format->Gshift)>>3;
					uchar Boffset0=(src0->format->Bshift)>>3;
					unsigned r=pos0[Roffset0],
						g=pos0[Goffset0],
						b=pos0[Boffset0];
					r<<=dst->virtualScreen->format->Rshift;
					g<<=dst->virtualScreen->format->Gshift;
					b<<=dst->virtualScreen->format->Bshift;
					r|=g|b;
					SDL_FillRect(dst->virtualScreen,&rect,r);
				}
			}
			SDL_UnlockSurface(src0);
		}
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
		idealtimepos+=delay;
	}
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

void NONS_GFX::effectMosaicOut(SDL_Surface *src0,SDL_Surface *src1,NONS_VirtualScreen *dst){
	LOCKSCREEN;
	if (!src0 || !dst || src0->w!=dst->virtualScreen->w || src0->h!=dst->virtualScreen->h){
		UNLOCKSCREEN;
		return;
	}
	if (CURRENTLYSKIPPING){
		UNLOCKSCREEN;
		dst->blitToScreen(src0,0,0);
		dst->updateWholeScreen();
		return;
	}
	float delay=float(this->duration)/10.0;
	long advance=dst->virtualScreen->format->BytesPerPixel;
	uchar Roffset0=(dst->virtualScreen->format->Rshift)>>3;
	uchar Goffset0=(dst->virtualScreen->format->Gshift)>>3;
	uchar Boffset0=(dst->virtualScreen->format->Bshift)>>3;
	UNLOCKSCREEN;
	long idealtimepos=0,
		lastT=9999,
		start=SDL_GetTicks();
	for (long a=0;a<10;a++){
		long t0=SDL_GetTicks();
		if ((t0-start-idealtimepos>lastT || CURRENTLYSKIPPING) && lastT>5 && a<9){
			idealtimepos+=delay;
			continue;
		}
		LOCKSCREEN;
		SDL_LockSurface(src0);
		long pixelSize=1;
		for (short b=0;b<a;b++) pixelSize*=2;
		for (long y=0;y<dst->virtualScreen->h;y+=pixelSize){
			//long tps;
			long tps=y+pixelSize-1>=dst->virtualScreen->h?dst->virtualScreen->h-y-1:pixelSize;
			for (long x=0;x<dst->virtualScreen->w;x+=pixelSize){
				SDL_Rect rect={x,y,pixelSize,pixelSize};
				uchar *pos0=(uchar *)dst->virtualScreen->pixels;
				pos0+=dst->virtualScreen->pitch*(y+tps-1)+advance*x;
				unsigned r=pos0[Roffset0],
					g=pos0[Goffset0],
					b=pos0[Boffset0];
				r<<=dst->virtualScreen->format->Rshift;
				g<<=dst->virtualScreen->format->Gshift;
				b<<=dst->virtualScreen->format->Bshift;
				r|=g|b;
				SDL_FillRect(dst->virtualScreen,&rect,r);
			}
		}
		SDL_UnlockSurface(src0);
		dst->updateWithoutLock();
		UNLOCKSCREEN;
		long t1=SDL_GetTicks();
		lastT=t1-t0;
		if (lastT<delay)
			SDL_Delay(delay-lastT);
		idealtimepos+=delay;
	}
	manualBlit(src0,0,dst->virtualScreen,0);
	if (!CURRENTLYSKIPPING && NONS_GFX::effectblank)
		waitNonCancellable(NONS_GFX::effectblank);
}

#ifndef NONS_PARALLELIZE
void NONS_GFX::effectMonochrome(SDL_Surface *src0,SDL_Surface *dst){
	if (!dst || dst->format->BitsPerPixel<24)
		return;
	long w=dst->w,h=dst->h;
	SDL_LockSurface(dst);
	uchar *pos=(uchar *)dst->pixels;
	long advance=dst->format->BytesPerPixel;
	uchar Roffset=(dst->format->Rshift)>>3;
	uchar Goffset=(dst->format->Gshift)>>3;
	uchar Boffset=(dst->format->Bshift)>>3;
	for (long y=0;y<h;y++){
		uchar *pos0=pos;
		for (long x=0;x<w;x++){
			ulong r1,g1,b1;
			r1=long(pos[Roffset])*long(this->color.r)+
				long(pos[Goffset])*long(this->color.r)+
				long(pos[Boffset])*long(this->color.r);
			r1/=255*3;
			g1=long(pos[Roffset])*long(this->color.g)+
				long(pos[Goffset])*long(this->color.g)+
				long(pos[Boffset])*long(this->color.g);
			g1/=255*3;
			b1=long(pos[Roffset])*long(this->color.b)+
				long(pos[Goffset])*long(this->color.b)+
				long(pos[Boffset])*long(this->color.b);
			b1/=255*3;
			pos[Roffset]=r1;
			pos[Goffset]=g1;
			pos[Boffset]=b1;
			pos+=advance;
		}
		pos=pos0+dst->pitch;
	}
	SDL_UnlockSurface(dst);
}
#else
void effectMonochrome_threaded(SDL_Surface *dst,SDL_Rect *dstRect,SDL_Color &color);
int effectMonochrome_threaded(void *parameters);

void NONS_GFX::effectMonochrome(SDL_Surface *src0,SDL_Surface *dst){
	if (!dst || dst->format->BitsPerPixel<24)
		return;
	SDL_LockSurface(dst);
	SDL_Rect dstRect={0,0,dst->w,dst->h};
	if (cpu_count==1){
		effectMonochrome_threaded(dst,&dstRect,this->color);
		SDL_UnlockSurface(dst);
		return;
	}
	SDL_Thread **threads=new SDL_Thread *[cpu_count];
	SDL_Rect *rects=new SDL_Rect[cpu_count];
	PSF_parameters *parameters=new PSF_parameters[cpu_count];
	ulong division=float(dstRect.h)/float(cpu_count);
	ulong total=0;
	for (ushort a=0;a<cpu_count;a++){
		rects[a]=dstRect;
		rects[a].y+=Sint16(a*division);
		rects[a].h=Sint16(division);
		total+=rects[a].h;
		parameters[a].dst=dst;
		parameters[a].dstRect=rects+a;
		parameters[a].color=this->color;
	}
	rects[cpu_count-1].h+=dstRect.h-total;
	for (ushort a=0;a<cpu_count;a++)
		threads[a]=SDL_CreateThread(effectMonochrome_threaded,parameters+a);
	for (ushort a=0;a<cpu_count;a++)
		SDL_WaitThread(threads[a],0);
	SDL_UnlockSurface(dst);
	delete[] threads;
	delete[] rects;
	delete[] parameters;
}

int effectMonochrome_threaded(void *parameters){
	PSF_parameters *p=(PSF_parameters *)parameters;
	effectMonochrome_threaded(p->dst,p->dstRect,p->color);
	return 0;
}

void effectMonochrome_threaded(SDL_Surface *dst,SDL_Rect *dstRect,SDL_Color &color){
	long w=dstRect->w,h=dstRect->h;
	uchar *pos=(uchar *)dst->pixels;
	uchar Roffset=(dst->format->Rshift)>>3;
	uchar Goffset=(dst->format->Gshift)>>3;
	uchar Boffset=(dst->format->Bshift)>>3;
	long advance=dst->format->BytesPerPixel;
	long pitch=dst->pitch;
	pos+=dstRect->x*advance+dstRect->y*pitch;
	for (long y=0;y<h;y++){
		uchar *pos0=pos;
		for (long x=0;x<w;x++){
			ulong r1,g1,b1;
			r1=long(pos[Roffset])*long(color.r)+
				long(pos[Goffset])*long(color.r)+
				long(pos[Boffset])*long(color.r);
			r1/=255*3;
			g1=long(pos[Roffset])*long(color.g)+
				long(pos[Goffset])*long(color.g)+
				long(pos[Boffset])*long(color.g);
			g1/=255*3;
			b1=long(pos[Roffset])*long(color.b)+
				long(pos[Goffset])*long(color.b)+
				long(pos[Boffset])*long(color.b);
			b1/=255*3;
			pos[Roffset]=r1;
			pos[Goffset]=g1;
			pos[Boffset]=b1;
			pos+=advance;
		}
		pos=pos0+dst->pitch;
	}
}
#endif

#ifndef NONS_PARALLELIZE
void NONS_GFX::effectNegative(SDL_Surface *src0,SDL_Surface *dst){
	if (!dst || dst->format->BitsPerPixel<24)
		return;
	long w=dst->w,h=dst->h;
	SDL_LockSurface(dst);
	uchar *pos=(uchar *)dst->pixels;
	long advance=dst->format->BytesPerPixel;
	uchar Roffset=(dst->format->Rshift)>>3;
	uchar Goffset=(dst->format->Gshift)>>3;
	uchar Boffset=(dst->format->Bshift)>>3;
	for (long y=0;y<h;y++){
		uchar *pos0=pos;
		for (long x=0;x<w;x++){
			pos[Roffset]=255-pos[Roffset];
			pos[Goffset]=255-pos[Goffset];
			pos[Boffset]=255-pos[Boffset];
			pos+=advance;
		}
		pos=pos0+dst->pitch;
	}
	SDL_UnlockSurface(dst);
}
#else
void effectNegative_threaded(SDL_Surface *dst,SDL_Rect *dstRect);
int effectNegative_threaded(void *parameters);

void NONS_GFX::effectNegative(SDL_Surface *src0,SDL_Surface *dst){
	if (!dst || dst->format->BitsPerPixel<24)
		return;
	SDL_LockSurface(dst);
	SDL_Rect dstRect={0,0,dst->w,dst->h};
	if (cpu_count==1){
		effectNegative_threaded(dst,&dstRect);
		SDL_UnlockSurface(dst);
		return;
	}
	SDL_Thread **threads=new SDL_Thread *[cpu_count];
	SDL_Rect *rects=new SDL_Rect[cpu_count];
	PSF_parameters *parameters=new PSF_parameters[cpu_count];
	ulong division=float(dstRect.h)/float(cpu_count);
	ulong total=0;
	for (ushort a=0;a<cpu_count;a++){
		rects[a]=dstRect;
		rects[a].y+=Sint16(a*division);
		rects[a].h=Sint16(division);
		total+=rects[a].h;
		parameters[a].dst=dst;
		parameters[a].dstRect=rects+a;
	}
	rects[cpu_count-1].h+=dstRect.h-total;
	for (ushort a=0;a<cpu_count;a++)
		threads[a]=SDL_CreateThread(effectNegative_threaded,parameters+a);
	for (ushort a=0;a<cpu_count;a++)
		SDL_WaitThread(threads[a],0);
	SDL_UnlockSurface(dst);
	delete[] threads;
	delete[] rects;
	delete[] parameters;
}

int effectNegative_threaded(void *parameters){
	PSF_parameters *p=(PSF_parameters *)parameters;
	effectNegative_threaded(p->dst,p->dstRect);
	return 0;
}

void effectNegative_threaded(SDL_Surface *dst,SDL_Rect *dstRect){
	long w=dstRect->w,h=dstRect->h;
	uchar *pos=(uchar *)dst->pixels;
	uchar Roffset=(dst->format->Rshift)>>3;
	uchar Goffset=(dst->format->Gshift)>>3;
	uchar Boffset=(dst->format->Bshift)>>3;
	long advance=dst->format->BytesPerPixel;
	long pitch=dst->pitch;
	pos+=dstRect->x*advance+dstRect->y*pitch;
	for (long y=0;y<h;y++){
		uchar *pos0=pos;
		for (long x=0;x<w;x++){
			pos[Roffset]=255-pos[Roffset];
			pos[Goffset]=255-pos[Goffset];
			pos[Boffset]=255-pos[Boffset];
			pos+=advance;
		}
		pos=pos0+pitch;
	}
}
#endif

#ifndef NONS_PARALLELIZE
void NONS_GFX::effectNegativeMono(SDL_Surface *src0,SDL_Surface *dst){
	if (!dst || dst->format->BitsPerPixel<24)
		return;
	long w=dst->w,h=dst->h;
	SDL_LockSurface(dst);
	uchar *pos=(uchar *)dst->pixels;
	long advance=dst->format->BytesPerPixel;
	uchar Roffset=(dst->format->Rshift)>>3;
	uchar Goffset=(dst->format->Gshift)>>3;
	uchar Boffset=(dst->format->Bshift)>>3;
	for (long y=0;y<h;y++){
		uchar *pos0=pos;
		for (long x=0;x<w;x++){
			ushort rgb=((255-pos[Roffset])+(255-pos[Goffset])+(255-pos[Boffset]))/3;
			pos[Roffset]=rgb;
			pos[Goffset]=rgb;
			pos[Boffset]=rgb;
			pos+=advance;
		}
		pos=pos0+dst->pitch;
	}
	SDL_UnlockSurface(dst);
}
#else
void effectNegativeMono_threaded(SDL_Surface *dst,SDL_Rect *dstRect);
int effectNegativeMono_threaded(void *parameters);

void NONS_GFX::effectNegativeMono(SDL_Surface *src0,SDL_Surface *dst){
	if (!dst || dst->format->BitsPerPixel<24)
		return;
	SDL_LockSurface(dst);
	SDL_Rect dstRect={0,0,dst->w,dst->h};
	if (cpu_count==1){
		effectNegative_threaded(dst,&dstRect);
		SDL_UnlockSurface(dst);
		return;
	}
	SDL_Thread **threads=new SDL_Thread *[cpu_count];
	SDL_Rect *rects=new SDL_Rect[cpu_count];
	PSF_parameters *parameters=new PSF_parameters[cpu_count];
	ulong division=float(dstRect.h)/float(cpu_count);
	ulong total=0;
	for (ushort a=0;a<cpu_count;a++){
		rects[a]=dstRect;
		rects[a].y+=Sint16(a*division);
		rects[a].h=Sint16(division);
		total+=rects[a].h;
		parameters[a].dst=dst;
		parameters[a].dstRect=rects+a;
	}
	rects[cpu_count-1].h+=dstRect.h-total;
	for (ushort a=0;a<cpu_count;a++)
		threads[a]=SDL_CreateThread(effectNegativeMono_threaded,parameters+a);
	for (ushort a=0;a<cpu_count;a++)
		SDL_WaitThread(threads[a],0);
	SDL_UnlockSurface(dst);
	delete[] threads;
	delete[] rects;
	delete[] parameters;
}

int effectNegativeMono_threaded(void *parameters){
	PSF_parameters *p=(PSF_parameters *)parameters;
	effectNegativeMono_threaded(p->dst,p->dstRect);
	return 0;
}

void effectNegativeMono_threaded(SDL_Surface *dst,SDL_Rect *dstRect){
	long w=dstRect->w,h=dstRect->h;
	uchar *pos=(uchar *)dst->pixels;
	uchar Roffset=(dst->format->Rshift)>>3;
	uchar Goffset=(dst->format->Gshift)>>3;
	uchar Boffset=(dst->format->Bshift)>>3;
	long advance=dst->format->BytesPerPixel;
	long pitch=dst->pitch;
	pos+=dstRect->x*advance+dstRect->y*pitch;
	for (long y=0;y<h;y++){
		uchar *pos0=pos;
		for (long x=0;x<w;x++){
			ushort rgb=((255-pos[Roffset])+(255-pos[Goffset])+(255-pos[Boffset]))/3;
			pos[Roffset]=rgb;
			pos[Goffset]=rgb;
			pos[Boffset]=rgb;
			pos+=advance;
		}
		pos=pos0+pitch;
	}
}
#endif
NONS_GFXstore::NONS_GFXstore(){
	this->effects[0]=new NONS_GFX();
	this->effects[0]->stored=1;
	this->effects[1]=new NONS_GFX(1);
	this->effects[1]->stored=1;
}

NONS_GFXstore::~NONS_GFXstore(){
	for (std::map<ulong,NONS_GFX *>::iterator i=this->effects.begin();i!=this->effects.begin();i++)
		delete i->second;
}

NONS_GFX *NONS_GFXstore::retrieve(ulong code){
	std::map<ulong,NONS_GFX *>::iterator i=this->effects.find(code);
	if (i==this->effects.end())
		return 0;
	return i->second;
}

bool NONS_GFXstore::remove(ulong code){
	std::map<ulong,NONS_GFX *>::iterator i=this->effects.find(code);
	if (i==this->effects.end())
		return 0;
	delete i->second;
	this->effects.erase(i);
	return 1;
}

NONS_GFX *NONS_GFXstore::add(ulong code,ulong effect,ulong duration,wchar_t *rule){
	if (this->retrieve(code))
		return 0;
	NONS_GFX *res=new NONS_GFX(effect,duration,rule);
	this->effects[code]=res;
	return res;
}
#endif
