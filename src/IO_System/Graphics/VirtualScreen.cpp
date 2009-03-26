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

#ifndef NONS_VIRTUALSCREEN_CPP
#define NONS_VIRTUALSCREEN_CPP

#include "VirtualScreen.h"
#include "../../Functions.h"
#include "../../Globals.h"

//#define ONLY_NEAREST_NEIGHBOR

NONS_VirtualScreen::NONS_VirtualScreen(ulong w,ulong h){
	this->realScreen=SDL_SetVideoMode(w,h,32,SDL_HWSURFACE|SDL_DOUBLEBUF|((CLOptions.startFullscreen)?SDL_FULLSCREEN:0));
	this->virtualScreen=this->realScreen;
	this->x_multiplier=0x100;
	this->y_multiplier=0x100;
	this->x_divisor=this->x_multiplier;
	this->y_divisor=this->y_multiplier;
	this->inRect=this->realScreen->clip_rect;
	this->outRect=this->inRect;
	this->normalInterpolation=0;
	this->fullscreen=CLOptions.startFullscreen;
}

NONS_VirtualScreen::NONS_VirtualScreen(ulong iw,ulong ih,ulong ow,ulong oh){
	this->realScreen=SDL_SetVideoMode(ow,oh,24,SDL_HWSURFACE|SDL_DOUBLEBUF|((CLOptions.startFullscreen)?SDL_FULLSCREEN:0));
	if (iw==ow && ih==oh){
		this->virtualScreen=this->realScreen;
		this->x_multiplier=0x100;
		this->y_multiplier=0x100;
		this->inRect=this->realScreen->clip_rect;
		this->outRect=this->inRect;
		this->normalInterpolation=0;
	}else{
		this->virtualScreen=SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,iw,ih,32,rmask,gmask,bmask,amask);
		float ratio0=float(iw)/float(ih),
			ratio1=float(ow)/float(oh);
		this->normalInterpolation=&bilinearInterpolation;
		this->inRect=this->virtualScreen->clip_rect;
		if (ABS(ratio0-ratio1)<.00001){
			this->x_multiplier=(ow<<8)/iw;
			this->y_multiplier=(oh<<8)/ih;
			this->x_divisor=(iw<<8)/ow;
			this->y_divisor=(ih<<8)/oh;
			if (x_multiplier<0x100 || y_multiplier<0x100)
				this->normalInterpolation=&bilinearInterpolation2;
			this->outRect=this->realScreen->clip_rect;
		}else if (ratio0>ratio1){
			float h=float(ow)/float(ratio0);
			this->outRect.x=0;
			this->outRect.y=(oh-h)/2;
			this->outRect.w=ow;
			this->outRect.h=h;
			this->x_multiplier=(ow<<8)/iw;
			this->y_multiplier=ulong(h*256)/ih;
			this->x_divisor=(iw<<8)/ow;
			this->y_divisor=ulong(ih*256)/h;
			if (x_multiplier<0x100 || h/float(ih)<1)
				this->normalInterpolation=&bilinearInterpolation2;
		}else{
			float w=float(oh)*float(ratio0);
			this->outRect.x=(ow-w)/2;
			this->outRect.y=0;
			this->outRect.w=w;
			this->outRect.h=oh;
			this->x_multiplier=ulong(w*256)/iw;
			this->y_multiplier=(oh<<8)/ih;
			this->x_divisor=ulong(iw*256)/w;
			this->y_divisor=(ih<<8)/oh;
			if (w/float(iw)<1 || y_multiplier<=0x100)
				this->normalInterpolation=&bilinearInterpolation2;
		}
		//this->normalInterpolation=&nearestNeighborInterpolation;
		this->realScreen->clip_rect=this->outRect;
	}
	this->fullscreen=CLOptions.startFullscreen;
}

NONS_VirtualScreen::~NONS_VirtualScreen(){
	SDL_FreeSurface(this->realScreen);
	if (this->virtualScreen!=this->realScreen)
		SDL_FreeSurface(this->virtualScreen);
}

void NONS_VirtualScreen::blitToScreen(SDL_Surface *src,SDL_Rect *srcrect,SDL_Rect *dstrect){
	SDL_BlitSurface(src,srcrect,this->virtualScreen,dstrect);
}

void NONS_VirtualScreen::updateScreen(ulong x,ulong y,ulong w,ulong h,bool fast){
	if (this->virtualScreen==this->realScreen)
		SDL_UpdateRect(this->realScreen,x,y,w,h);
	else{
		SDL_Rect s={x,y,w,h},
			d={this->convertX(x),this->convertY(y),this->convertW(w),this->convertH(h)};
		if (d.x+d.w>this->outRect.w+this->outRect.x)
			d.w=this->outRect.w-d.x+this->outRect.x;
		if (d.y+d.h>this->outRect.h+this->outRect.y)
			d.h=this->outRect.h-d.y+this->outRect.y;
#ifndef ONLY_NEAREST_NEIGHBOR
		if (fast)
			nearestNeighborInterpolation(this->virtualScreen,&s,this->realScreen,&d,this->x_multiplier,this->y_multiplier);
		else
			(*this->normalInterpolation)(this->virtualScreen,&s,this->realScreen,&d,this->x_multiplier,this->y_multiplier);
#else
		nearestNeighborInterpolation(this->virtualScreen,&s,this->realScreen,&d,this->x_multiplier,this->y_multiplier);
#endif
		SDL_UpdateRect(this->realScreen,d.x,d.y,d.w,d.h);
		//SDL_UpdateRect(this->realScreen,this->outRect.x,this->outRect.y,this->outRect.w,this->outRect.h);
	}
}

void NONS_VirtualScreen::updateWholeScreen(bool fast){
	LOCKSCREEN;
	this->updateWithoutLock(fast);
	UNLOCKSCREEN;
}

void NONS_VirtualScreen::updateWithoutLock(bool fast){
	if (this->virtualScreen==this->realScreen)
		SDL_UpdateRect(this->realScreen,0,0,0,0);
	else{
#ifndef ONLY_NEAREST_NEIGHBOR
		if (fast)
			nearestNeighborInterpolation(this->virtualScreen,&this->inRect,this->realScreen,&this->outRect,this->x_multiplier,this->y_multiplier);
		else
			(*this->normalInterpolation)(this->virtualScreen,&this->inRect,this->realScreen,&this->outRect,this->x_multiplier,this->y_multiplier);
#else
		nearestNeighborInterpolation(this->virtualScreen,&this->inRect,this->realScreen,&this->outRect,this->x_multiplier,this->y_multiplier);
#endif
		//manualBlit(this->virtualScreen,0,this->realScreen,0);
		SDL_UpdateRect(this->realScreen,this->outRect.x,this->outRect.y,this->outRect.w,this->outRect.h);
	}
}

long NONS_VirtualScreen::convertX(long x){
	return this->convertW(x)+this->outRect.x;
}

long NONS_VirtualScreen::convertY(long y){
	return this->convertH(y)+this->outRect.y;
}

long NONS_VirtualScreen::unconvertX(long x){
	return (((x-this->outRect.x)<<8)/this->x_multiplier);
}

long NONS_VirtualScreen::unconvertY(long y){
	return (((y-this->outRect.y)<<8)/this->y_multiplier);
}

ulong NONS_VirtualScreen::convertW(ulong w){
	ulong r=(w<<8)*this->x_multiplier;
	if ((r&0xFFFF)>0)
		r=(r>>16)+1;
	else
		r>>=16;
	return r;
}

ulong NONS_VirtualScreen::convertH(ulong h){
	ulong r=(h<<8)*this->y_multiplier;
	if ((r&0xFFFF)>0)
		r=(r>>16)+1;
	else
		r>>=16;
	return r;
}

bool NONS_VirtualScreen::toggleFullscreen(uchar mode){
	LOCKSCREEN;
	bool a=(this->virtualScreen==this->realScreen);
	if (mode==2)
		this->fullscreen=!this->fullscreen;
	else
		this->fullscreen=mode&1;
	ushort w=this->realScreen->w,
		h=this->realScreen->h;
	SDL_Surface *tempCopy=0;
	if (a){
		tempCopy=SDL_CreateRGBSurface(
			SDL_HWSURFACE|SDL_DOUBLEBUF,
			w,
			h,
			this->realScreen->format->BitsPerPixel,
			this->realScreen->format->Rmask,
			this->realScreen->format->Gmask,
			this->realScreen->format->Bmask,
			this->realScreen->format->Amask
		);
		//SDL_BlitSurface
		manualBlit(this->realScreen,0,tempCopy,0);
	}
	this->realScreen=SDL_SetVideoMode(w,h,24,SDL_HWSURFACE|SDL_DOUBLEBUF|((this->fullscreen)?SDL_FULLSCREEN:0));
	if (a){
		this->virtualScreen=this->realScreen;
		//SDL_BlitSurface
		manualBlit(tempCopy,0,this->realScreen,0);
		SDL_FreeSurface(tempCopy);
	}else
		this->realScreen->clip_rect=this->outRect;
	this->updateWithoutLock();
	UNLOCKSCREEN;
	return this->fullscreen;
}

#ifndef NONS_PARALLELIZE
void nearestNeighborInterpolation(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor){
	if (!src || !dst)
		return;
	SDL_Rect srcRect0,dstRect0;
	//assign SDL_Rects
	if (!srcRect){
		srcRect0.x=0;
		srcRect0.y=0;
		srcRect0.w=src->w;
		srcRect0.h=src->h;
	}else
		srcRect0=*srcRect;
	if (!dstRect){
		dstRect0.x=0;
		dstRect0.y=0;
		dstRect0.w=dst->w;
		dstRect0.h=dst->h;
	}else
		dstRect0=*dstRect;
	//correct SDL_Rects
	if (srcRect0.x+srcRect0.w>src->w)
		srcRect0.w-=srcRect0.x+srcRect0.w-src->w;
	if (srcRect0.y+srcRect0.h>src->h)
		srcRect0.h-=srcRect0.y+srcRect0.h-src->h;
	if (dstRect0.x+dstRect0.w>dst->w)
		srcRect0.w-=dstRect0.x+srcRect0.w-dst->w;
	if (dstRect0.y+dstRect0.h>dst->h)
		srcRect0.h-=dstRect0.y+srcRect0.h-dst->h;
	//check SDL_Rects
	if (srcRect0.x<0 || srcRect0.x>=src->w || srcRect0.y<0 || srcRect0.y>=src->h || srcRect0.w<=0 || srcRect0.h<=0 ||
		dstRect0.x<0 || dstRect0.x>=dst->w || dstRect0.y<0 || dstRect0.y>=dst->h || dstRect0.w<=0 || dstRect0.h<=0)
		return;
	if (srcRect0.w==dstRect0.w && srcRect0.h==dstRect0.h){
		SDL_BlitSurface(src,&srcRect0,dst,&dstRect0);
		return;
	}
	if (src->format->BitsPerPixel<24 || dst->format->BitsPerPixel<24)
		return;
	SDL_FillRect(dst,&dstRect0,dst->format->Amask|dst->format->Gmask);
	SDL_LockSurface(src);
	SDL_LockSurface(dst);
	uchar *pixels0=(uchar *)src->pixels;
	uchar *pixels1=(uchar *)dst->pixels;
	uchar Roffset0=(src->format->Rshift)>>3;
	uchar Goffset0=(src->format->Gshift)>>3;
	uchar Boffset0=(src->format->Bshift)>>3;
	uchar Roffset1=(dst->format->Rshift)>>3;
	uchar Goffset1=(dst->format->Gshift)>>3;
	uchar Boffset1=(dst->format->Bshift)>>3;
	uchar advance0=src->format->BytesPerPixel;
	uchar advance1=dst->format->BytesPerPixel;
	Uint16 pitch0=src->pitch;
	Uint16 pitch1=dst->pitch;
	uchar *pos0=pixels0+srcRect0.x*advance0+srcRect0.y*pitch0;
	uchar *pos1=pixels1+dstRect0.x*advance1+dstRect0.y*pitch1;
#define NN_NOT_FLOAT
#ifndef NN_NOT_FLOAT
	float errorX=0,errorY=0;
	float derrorX=!x_factor?float(dstRect0.w)/float(srcRect0.w):float(x_factor)/256;
	float derrorY=!y_factor?float(dstRect0.h)/float(srcRect0.h):float(y_factor)/256;
#else
	long errorX=0,errorY=0;
	ulong derrorX=!x_factor?(ulong(dstRect0.w)<<8)/srcRect0.w:x_factor;
	ulong derrorY=!y_factor?(ulong(dstRect0.h)<<8)/srcRect0.h:y_factor;
#endif
	srcRect0.w+=srcRect0.x;
	dstRect0.w+=dstRect0.x;
	srcRect0.h+=srcRect0.y;
	dstRect0.h+=dstRect0.y;
	for (unsigned y0=srcRect0.y,y1=dstRect0.y;y0<srcRect0.h && y1<dstRect0.h;){
		uchar *pos00=pos0;
		errorY+=derrorY;
#ifndef NN_NOT_FLOAT
		//while (errorY>=.5 && y1<dstRect0.h){
		while (errorY>0 && y1<dstRect0.h){
#else
		//while (errorY>=0x80 && y1<dstRect0.h){
		while (errorY>0 && y1<dstRect0.h){
#endif
			uchar *pos10=pos1;
			errorX=0;
			for (unsigned x0=srcRect0.x,x1=dstRect0.x;x0<srcRect0.w && x1<dstRect0.w;){
				errorX+=derrorX;
#ifndef NN_NOT_FLOAT
				//while (errorX>=.5 && x1<dstRect0.w){
				while (errorX>0 && x1<dstRect0.w){
#else
				//while (errorX>0x80 && x1<dstRect0.w){
				while (errorX>0 && x1<dstRect0.w){
#endif
					pos1[Roffset1]=pos0[Roffset0];
					pos1[Goffset1]=pos0[Goffset0];
					pos1[Boffset1]=pos0[Boffset0];
#ifndef NN_NOT_FLOAT
					errorX--;
#else
					errorX-=0x100;
#endif
					pos1+=advance1;
					x1++;
				}
				pos0+=advance0;
				x0++;
			}
#ifndef NN_NOT_FLOAT
			errorY--;
#else
			errorY-=0x100;
#endif
			pos1=pos10+pitch1;
			y1++;
			pos0=pos00;
		}
		pos0+=pitch0;
		y0++;
	}
	SDL_UnlockSurface(src);
	SDL_UnlockSurface(dst);
}
#else
//(Interpolation Function)
struct IF_parameters{
	SDL_Surface *src;
	SDL_Surface *dst;
	SDL_Rect *srcRect;
	SDL_Rect *dstRect;
	ulong x_factor;
	ulong y_factor;
};

void nearestNeighborInterpolation_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor);
int nearestNeighborInterpolation_threaded(void *parameters);

void nearestNeighborInterpolation(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor){
	if (!src || !dst)
		return;
	SDL_Rect srcRect0,dstRect0;
	//assign SDL_Rects
	if (!srcRect){
		srcRect0.x=0;
		srcRect0.y=0;
		srcRect0.w=src->w;
		srcRect0.h=src->h;
	}else
		srcRect0=*srcRect;
	if (!dstRect){
		dstRect0.x=0;
		dstRect0.y=0;
		dstRect0.w=dst->w;
		dstRect0.h=dst->h;
	}else
		dstRect0=*dstRect;
	//correct SDL_Rects
	if (srcRect0.x+srcRect0.w>src->w)
		srcRect0.w-=srcRect0.x+srcRect0.w-src->w;
	if (srcRect0.y+srcRect0.h>src->h)
		srcRect0.h-=srcRect0.y+srcRect0.h-src->h;
	if (dstRect0.x+dstRect0.w>dst->w)
		srcRect0.w-=dstRect0.x+srcRect0.w-dst->w;
	if (dstRect0.y+dstRect0.h>dst->h)
		srcRect0.h-=dstRect0.y+srcRect0.h-dst->h;
	//check SDL_Rects
	if (srcRect0.x<0 || srcRect0.x>=src->w || srcRect0.y<0 || srcRect0.y>=src->h || srcRect0.w<=0 || srcRect0.h<=0 ||
		dstRect0.x<0 || dstRect0.x>=dst->w || dstRect0.y<0 || dstRect0.y>=dst->h || dstRect0.w<=0 || dstRect0.h<=0)
		return;
	if (srcRect0.w==dstRect0.w && srcRect0.h==dstRect0.h){
		SDL_BlitSurface(src,&srcRect0,dst,&dstRect0);
		return;
	}
	if (src->format->BitsPerPixel<24 || dst->format->BitsPerPixel<24)
		return;
	SDL_FillRect(dst,&dstRect0,dst->format->Amask|dst->format->Gmask);

	SDL_Thread **threads=new SDL_Thread *[cpu_count];
	SDL_Rect *rects0=new SDL_Rect[cpu_count];
	SDL_Rect *rects1=new SDL_Rect[cpu_count];
	IF_parameters *parameters=new IF_parameters[cpu_count];
	ulong division0=float(srcRect0.h)/float(cpu_count);
	ulong division1=float(division0)*(float(y_factor)/256);
	ulong total0=0,
		total1=0;
	for (ushort a=0;a<cpu_count;a++){
		rects0[a]=srcRect0;
		rects1[a]=dstRect0;
		rects0[a].y+=Sint16(a*division0);
		//rects0[a].h=Sint16(division0);
		rects1[a].y+=Sint16(a*division1);
		rects1[a].h=Sint16(division1);
		
		total0+=rects0[a].h;
		total1+=rects1[a].h;
		parameters[a].src=src;
		parameters[a].srcRect=rects0+a;
		parameters[a].dst=dst;
		parameters[a].dstRect=rects1+a;
		parameters[a].x_factor=x_factor;
		parameters[a].y_factor=y_factor;
	}
	//rects0[cpu_count-1].h+=srcRect0.h-total0;
	rects1[cpu_count-1].h+=dstRect0.h-total1;

	SDL_LockSurface(src);
	SDL_LockSurface(dst);
	for (ushort a=0;a<cpu_count;a++)
		threads[a]=SDL_CreateThread(nearestNeighborInterpolation_threaded,parameters+a);
	for (ushort a=0;a<cpu_count;a++)
		SDL_WaitThread(threads[a],0);
	SDL_UnlockSurface(src);
	SDL_UnlockSurface(dst);
	delete[] threads;
	delete[] rects0;
	delete[] rects1;
	delete[] parameters;
}

int nearestNeighborInterpolation_threaded(void *parameters){
	IF_parameters *p=(IF_parameters *)parameters;
	nearestNeighborInterpolation_threaded(p->src,p->srcRect,p->dst,p->dstRect,p->x_factor,p->y_factor);
	return 0;
}

void nearestNeighborInterpolation_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor){
	SDL_Rect &srcRect0=*srcRect,
		&dstRect0=*dstRect;
	uchar *pixels0=(uchar *)src->pixels;
	uchar *pixels1=(uchar *)dst->pixels;
	uchar Roffset0=(src->format->Rshift)>>3;
	uchar Goffset0=(src->format->Gshift)>>3;
	uchar Boffset0=(src->format->Bshift)>>3;
	uchar Roffset1=(dst->format->Rshift)>>3;
	uchar Goffset1=(dst->format->Gshift)>>3;
	uchar Boffset1=(dst->format->Bshift)>>3;
	uchar advance0=src->format->BytesPerPixel;
	uchar advance1=dst->format->BytesPerPixel;
	Uint16 pitch0=src->pitch;
	Uint16 pitch1=dst->pitch;
	uchar *pos0=pixels0+srcRect0.x*advance0+srcRect0.y*pitch0;
	uchar *pos1=pixels1+dstRect0.x*advance1+dstRect0.y*pitch1;
	long errorX=0,errorY=0;
	ulong derrorX=!x_factor?(ulong(dstRect0.w)<<8)/srcRect0.w:x_factor;
	ulong derrorY=!y_factor?(ulong(dstRect0.h)<<8)/srcRect0.h:y_factor;
	SDL_Rect s=srcRect0,
		d=dstRect0;
	srcRect0.w+=srcRect0.x;
	dstRect0.w+=dstRect0.x;
	srcRect0.h+=srcRect0.y;
	dstRect0.h+=dstRect0.y;
	for (unsigned y0=srcRect0.y,y1=dstRect0.y;y0<srcRect0.h && y1<dstRect0.h;){
		uchar *pos00=pos0;
		errorY+=derrorY;
		//while (errorY>=0x80 && y1<dstRect0.h){
		while (errorY>0 && y1<dstRect0.h){
			uchar *pos10=pos1;
			errorX=0;
			for (unsigned x0=srcRect0.x,x1=dstRect0.x;x0<srcRect0.w && x1<dstRect0.w;){
				errorX+=derrorX;
				//while (errorX>=0x80 && x1<dstRect0.w){
				while (errorX>0 && x1<dstRect0.w){
					pos1[Roffset1]=pos0[Roffset0];
					pos1[Goffset1]=pos0[Goffset0];
					pos1[Boffset1]=pos0[Boffset0];
					errorX-=0x100;
					pos1+=advance1;
					x1++;
				}
				pos0+=advance0;
				x0++;
			}
			errorY-=0x100;
			pos1=pos10+pitch1;
			y1++;
			pos0=pos00;
		}
		pos0+=pitch0;
		y0++;
	}
}
#endif

#ifndef NONS_PARALLELIZE
void bilinearInterpolation(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor){
	if (!src || !dst)
		return;
	SDL_Rect srcRect0,dstRect0;
	//assign SDL_Rects
	if (!srcRect){
		srcRect0.x=0;
		srcRect0.y=0;
		srcRect0.w=src->w;
		srcRect0.h=src->h;
	}else
		srcRect0=*srcRect;
	if (!dstRect){
		dstRect0.x=0;
		dstRect0.y=0;
		dstRect0.w=dst->w;
		dstRect0.h=dst->h;
	}else
		dstRect0=*dstRect;
	//correct SDL_Rects
	if (srcRect0.x+srcRect0.w>src->w)
		srcRect0.w-=srcRect0.x+srcRect0.w-src->w;
	if (srcRect0.y+srcRect0.h>src->h)
		srcRect0.h-=srcRect0.y+srcRect0.h-src->h;
	if (dstRect0.x+dstRect0.w>dst->w)
		srcRect0.w-=dstRect0.x+srcRect0.w-dst->w;
	if (dstRect0.y+dstRect0.h>dst->h)
		srcRect0.h-=dstRect0.y+srcRect0.h-dst->h;
	//check SDL_Rects
	if (srcRect0.x<0 || srcRect0.x>=src->w || srcRect0.y<0 || srcRect0.y>=src->h || srcRect0.w<=0 || srcRect0.h<=0 ||
		dstRect0.x<0 || dstRect0.x>=dst->w || dstRect0.y<0 || dstRect0.y>=dst->h || dstRect0.w<=0 || dstRect0.h<=0)
		return;
	if (srcRect0.w==dstRect0.w && srcRect0.h==dstRect0.h){
		SDL_BlitSurface(src,&srcRect0,dst,&dstRect0);
		return;
	}
	if (src->format->BitsPerPixel<24 || dst->format->BitsPerPixel<24)
		return;
	SDL_FillRect(dst,&dstRect0,dst->format->Amask|dst->format->Gmask);
	SDL_LockSurface(src);
	SDL_LockSurface(dst);
	uchar *pixels0=(uchar *)src->pixels;
	uchar *pixels1=(uchar *)dst->pixels;
	uchar Roffset0=(src->format->Rshift)>>3;
	uchar Goffset0=(src->format->Gshift)>>3;
	uchar Boffset0=(src->format->Bshift)>>3;
	uchar Roffset1=(dst->format->Rshift)>>3;
	uchar Goffset1=(dst->format->Gshift)>>3;
	uchar Boffset1=(dst->format->Bshift)>>3;
	uchar advance0=src->format->BytesPerPixel;
	uchar advance1=dst->format->BytesPerPixel;
	Uint16 pitch0=src->pitch;
	Uint16 pitch1=dst->pitch;
	uchar *pos0=pixels0;//+srcRect0.x*advance0+srcRect0.y*pitch0;
	uchar *pos1=pixels1;//+dstRect0.x*advance1+dstRect0.y*pitch1;
	long X=0,
		Y=0;
	ulong advanceX=(src->clip_rect.w<<16)/dst->clip_rect.w,
		advanceY=(src->clip_rect.h<<16)/dst->clip_rect.h;
	srcRect0.w+=srcRect0.x;
	dstRect0.w+=dstRect0.x;
	srcRect0.h+=srcRect0.y;
	dstRect0.h+=dstRect0.y;
	for (long y1=dstRect0.y;y1<dstRect0.h;y1++){
		Y=(advanceY>>2)+(y1-dst->clip_rect.y)*advanceY;
		long y0=Y>>16;
		uchar *pos00=pos0+pitch0*y0;
		ulong fractionY=Y&0xFFFF,
			ifractionY=0xFFFF-fractionY;
		for (long x1=dstRect0.x;x1<dstRect0.w;x1++){
			pos1=pixels1+pitch1*y1+advance1*x1;
			X=(advanceX>>2)+(x1-dst->clip_rect.x)*advanceX;
			long x0=X>>16;
			ulong fractionX=X&0xFFFF,
				ifractionX=0xFFFF-fractionX;
			uchar *pixel0=pos00+advance0*x0,
				*pixel1=pixel0+advance0,
				*pixel2=pixel0+pitch0,
				*pixel3=pixel1+pitch0;
			ulong weight0=((ifractionX>>8)*ifractionY)>>8,
				weight1=((fractionX>>8)*ifractionY)>>8,
				weight2=((ifractionX>>8)*fractionY)>>8,
				weight3=((fractionX>>8)*fractionY)>>8;
			ulong r0,g0,b0;
			uchar *r1=pos1+Roffset1;
			uchar *g1=pos1+Goffset1;
			uchar *b1=pos1+Boffset1;
			if (weight0){
				r0=pixel0[Roffset0]*weight0;
				g0=pixel0[Goffset0]*weight0;
				b0=pixel0[Boffset0]*weight0;
			}else{
				r0=0;
				g0=0;
				b0=0;
			}
			if (weight1){
				r0+=pixel1[Roffset0]*weight1;
				g0+=pixel1[Goffset0]*weight1;
				b0+=pixel1[Boffset0]*weight1;
			}
			if (weight2){
				r0+=pixel2[Roffset0]*weight2;
				g0+=pixel2[Goffset0]*weight2;
				b0+=pixel2[Boffset0]*weight2;
			}
			if (weight3){
				r0+=pixel3[Roffset0]*weight3;
				g0+=pixel3[Goffset0]*weight3;
				b0+=pixel3[Boffset0]*weight3;
			}
			*r1=r0/0xFFFF;
			*g1=g0/0xFFFF;
			*b1=b0/0xFFFF;
		}
	}
	SDL_UnlockSurface(src);
	SDL_UnlockSurface(dst);
}
#else
void bilinearInterpolation_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor);
int bilinearInterpolation_threaded(void *parameters);

//This algorithm works well for scales [.5;inf.), but has VERY slight precision
//problems at scales <1.
void bilinearInterpolation(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor){
	if (!src || !dst)
		return;
	SDL_Rect srcRect0,dstRect0;
	//assign SDL_Rects
	if (!srcRect){
		srcRect0.x=0;
		srcRect0.y=0;
		srcRect0.w=src->w;
		srcRect0.h=src->h;
	}else
		srcRect0=*srcRect;
	if (!dstRect){
		dstRect0.x=0;
		dstRect0.y=0;
		dstRect0.w=dst->w;
		dstRect0.h=dst->h;
	}else
		dstRect0=*dstRect;
	//correct SDL_Rects
	if (srcRect0.x+srcRect0.w>src->w)
		srcRect0.w-=srcRect0.x+srcRect0.w-src->w;
	if (srcRect0.y+srcRect0.h>src->h)
		srcRect0.h-=srcRect0.y+srcRect0.h-src->h;
	if (dstRect0.x+dstRect0.w>dst->w)
		srcRect0.w-=dstRect0.x+srcRect0.w-dst->w;
	if (dstRect0.y+dstRect0.h>dst->h)
		srcRect0.h-=dstRect0.y+srcRect0.h-dst->h;
	//check SDL_Rects
	if (srcRect0.x<0 || srcRect0.x>=src->w || srcRect0.y<0 || srcRect0.y>=src->h || srcRect0.w<=0 || srcRect0.h<=0 ||
		dstRect0.x<0 || dstRect0.x>=dst->w || dstRect0.y<0 || dstRect0.y>=dst->h || dstRect0.w<=0 || dstRect0.h<=0)
		return;
	if (srcRect0.w==dstRect0.w && srcRect0.h==dstRect0.h){
		manualBlit(src,&srcRect0,dst,&dstRect0);
		return;
	}
	if (src->format->BitsPerPixel<24 || dst->format->BitsPerPixel<24)
		return;
	SDL_FillRect(dst,&dstRect0,dst->format->Amask|dst->format->Gmask);

	SDL_Thread **threads=new SDL_Thread *[cpu_count];
	SDL_Rect *rects0=new SDL_Rect[cpu_count];
	SDL_Rect *rects1=new SDL_Rect[cpu_count];
	IF_parameters *parameters=new IF_parameters[cpu_count];
	ulong division0=float(srcRect0.h)/float(cpu_count);
	ulong division1=float(division0)*(float(y_factor)/256);
	ulong total0=0,
		total1=0;
	for (ushort a=0;a<cpu_count;a++){
		rects0[a]=srcRect0;
		rects1[a]=dstRect0;
		rects0[a].y+=Sint16(a*division0);
		//rects0[a].h=Sint16(division0);
		rects1[a].y+=Sint16(a*division1);
		rects1[a].h=Sint16(division1);
		
		total0+=rects0[a].h;
		total1+=rects1[a].h;
		parameters[a].src=src;
		parameters[a].srcRect=rects0+a;
		parameters[a].dst=dst;
		parameters[a].dstRect=rects1+a;
		parameters[a].x_factor=x_factor;
		parameters[a].y_factor=y_factor;
	}
	//rects0[cpu_count-1].h+=srcRect0.h-total0;
	rects1[cpu_count-1].h+=dstRect0.h-total1;

	SDL_LockSurface(src);
	SDL_LockSurface(dst);
	for (ushort a=0;a<cpu_count;a++)
		threads[a]=SDL_CreateThread(bilinearInterpolation_threaded,parameters+a);
	for (ushort a=0;a<cpu_count;a++)
		SDL_WaitThread(threads[a],0);
	SDL_UnlockSurface(src);
	SDL_UnlockSurface(dst);
}

int bilinearInterpolation_threaded(void *parameters){
	IF_parameters *p=(IF_parameters *)parameters;
	bilinearInterpolation_threaded(p->src,p->srcRect,p->dst,p->dstRect,p->x_factor,p->y_factor);
	return 0;
}

void bilinearInterpolation_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor){
	SDL_Rect srcRect0=*srcRect,
		dstRect0=*dstRect;
	uchar *pixels0=(uchar *)src->pixels;
	uchar *pixels1=(uchar *)dst->pixels;
	uchar Roffset0=(src->format->Rshift)>>3;
	uchar Goffset0=(src->format->Gshift)>>3;
	uchar Boffset0=(src->format->Bshift)>>3;
	uchar Roffset1=(dst->format->Rshift)>>3;
	uchar Goffset1=(dst->format->Gshift)>>3;
	uchar Boffset1=(dst->format->Bshift)>>3;
	uchar advance0=src->format->BytesPerPixel;
	uchar advance1=dst->format->BytesPerPixel;
	Uint16 pitch0=src->pitch;
	Uint16 pitch1=dst->pitch;
	uchar *pos0=pixels0;
	uchar *pos1=pixels1;
	long X=0,
		Y=0;
	ulong advanceX=(src->clip_rect.w<<16)/dst->clip_rect.w,
		advanceY=(src->clip_rect.h<<16)/dst->clip_rect.h;
	srcRect0.w+=srcRect0.x;
	dstRect0.w+=dstRect0.x;
	srcRect0.h+=srcRect0.y;
	dstRect0.h+=dstRect0.y;
	for (long y1=dstRect0.y;y1<dstRect0.h;y1++){
		Y=(advanceY>>2)+(y1-dst->clip_rect.y)*advanceY;
		long y0=Y>>16;
		uchar *pos00=pos0+pitch0*y0;
		ulong fractionY=Y&0xFFFF,
			ifractionY=0xFFFF-fractionY;
		for (long x1=dstRect0.x;x1<dstRect0.w;x1++){
			pos1=pixels1+pitch1*y1+advance1*x1;
			X=(advanceX>>2)+(x1-dst->clip_rect.x)*advanceX;
			long x0=X>>16;
			ulong fractionX=X&0xFFFF,
				ifractionX=0xFFFF-fractionX;
			uchar *pixel0=pos00+advance0*x0,
				*pixel1=pixel0+advance0,
				*pixel2=pixel0+pitch0,
				*pixel3=pixel1+pitch0;
			ulong weight0=((ifractionX>>8)*ifractionY)>>8,
				weight1=((fractionX>>8)*ifractionY)>>8,
				weight2=((ifractionX>>8)*fractionY)>>8,
				weight3=((fractionX>>8)*fractionY)>>8;
			ulong r0,g0,b0;
			uchar *r1=pos1+Roffset1;
			uchar *g1=pos1+Goffset1;
			uchar *b1=pos1+Boffset1;
			if (weight0){
				r0=pixel0[Roffset0]*weight0;
				g0=pixel0[Goffset0]*weight0;
				b0=pixel0[Boffset0]*weight0;
			}else{
				r0=0;
				g0=0;
				b0=0;
			}
			if (weight1){
				r0+=pixel1[Roffset0]*weight1;
				g0+=pixel1[Goffset0]*weight1;
				b0+=pixel1[Boffset0]*weight1;
			}
			if (weight2){
				r0+=pixel2[Roffset0]*weight2;
				g0+=pixel2[Goffset0]*weight2;
				b0+=pixel2[Boffset0]*weight2;
			}
			if (weight3){
				r0+=pixel3[Roffset0]*weight3;
				g0+=pixel3[Goffset0]*weight3;
				b0+=pixel3[Boffset0]*weight3;
			}
			*r1=r0/0xFFFF;
			*g1=g0/0xFFFF;
			*b1=b0/0xFFFF;
		}
	}
}
#endif

#define FLOOR(x) ((x)&0xFFFF0000)
#define CEIL(x) (((x)&0xFFFF)?(FLOOR(x)+0x10000):FLOOR(x))

//This algorithm works well for scales [0;1]. It's ~50% more expensive than
//bilinearInterpolation().
#ifndef NONS_PARALLELIZE
void bilinearInterpolation2(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor){
	if (!src || !dst)
		return;
	SDL_Rect srcRect0,dstRect0;
	//assign SDL_Rects
	if (!srcRect){
		srcRect0.x=0;
		srcRect0.y=0;
		srcRect0.w=src->w;
		srcRect0.h=src->h;
	}else
		srcRect0=*srcRect;
	if (!dstRect){
		dstRect0.x=0;
		dstRect0.y=0;
		dstRect0.w=dst->w;
		dstRect0.h=dst->h;
	}else
		dstRect0=*dstRect;
	//correct SDL_Rects
	if (srcRect0.x+srcRect0.w>src->w)
		srcRect0.w-=srcRect0.x+srcRect0.w-src->w;
	if (srcRect0.y+srcRect0.h>src->h)
		srcRect0.h-=srcRect0.y+srcRect0.h-src->h;
	if (dstRect0.x+dstRect0.w>dst->w)
		srcRect0.w-=dstRect0.x+srcRect0.w-dst->w;
	if (dstRect0.y+dstRect0.h>dst->h)
		srcRect0.h-=dstRect0.y+srcRect0.h-dst->h;
	//check SDL_Rects
	if (srcRect0.x<0 || srcRect0.x>=src->w || srcRect0.y<0 || srcRect0.y>=src->h || srcRect0.w<=0 || srcRect0.h<=0 ||
		dstRect0.x<0 || dstRect0.x>=dst->w || dstRect0.y<0 || dstRect0.y>=dst->h || dstRect0.w<=0 || dstRect0.h<=0)
		return;
	if (srcRect0.w==dstRect0.w && srcRect0.h==dstRect0.h){
		SDL_BlitSurface(src,&srcRect0,dst,&dstRect0);
		return;
	}
	if (src->format->BitsPerPixel<24 || dst->format->BitsPerPixel<24)
		return;
	SDL_FillRect(dst,&dstRect0,dst->format->Amask|dst->format->Gmask);
	SDL_LockSurface(src);
	SDL_LockSurface(dst);
	uchar *pixels0=(uchar *)src->pixels;
	uchar *pixels1=(uchar *)dst->pixels;
	uchar Roffset0=(src->format->Rshift)>>3;
	uchar Goffset0=(src->format->Gshift)>>3;
	uchar Boffset0=(src->format->Bshift)>>3;
	uchar Roffset1=(dst->format->Rshift)>>3;
	uchar Goffset1=(dst->format->Gshift)>>3;
	uchar Boffset1=(dst->format->Bshift)>>3;
	uchar advance0=src->format->BytesPerPixel;
	uchar advance1=dst->format->BytesPerPixel;
	Uint16 pitch0=src->pitch;
	Uint16 pitch1=dst->pitch;
	uchar *pos0=pixels0;
	uchar *pos1=pixels1;
	integer32 sizeX=(src->clip_rect.w<<16)/dst->clip_rect.w,
		sizeY=(src->clip_rect.h<<16)/dst->clip_rect.h;
	srcRect0.w+=srcRect0.x;
	dstRect0.w+=dstRect0.x;
	srcRect0.h+=srcRect0.y;
	dstRect0.h+=dstRect0.y;
	integer32 Y0=srcRect0.y<<16,Y1=Y0+sizeY,
		X0,X1;
	integer32 area=((sizeX>>8)*sizeY)>>8;
	uchar *pixel1=pos1+pitch1*dstRect0.y+advance1*dstRect0.x;
	for (long y1=dstRect0.y;y1<dstRect0.h;y1++){
		X0=srcRect0.x<<16;
		X1=X0+sizeX;
		uchar *row=pos0+pitch0*(Y0>>16);
		uchar *pixel10=pixel1;
		for (long x1=dstRect0.x;x1<dstRect0.w;x1++){
			uchar *pixel0=row+advance0*(X0>>16);
			unsigned r=0,g=0,b=0;
			for (integer32 y2=Y0;y2<Y1;){
				integer32 ymultiplier;
				uchar *pixel00=pixel0;
				if (Y1-y2<0x10000)
					ymultiplier=Y1-y2;
				else if (y2==Y0)
					ymultiplier=FLOOR(Y0)+0x10000-Y0;
				else
					ymultiplier=0x10000;
				for (integer32 x2=X0;x2<X1;){
					integer32 xmultiplier;
					if (X1-x2<0x10000)
						xmultiplier=X1-x2;
					else if (x2==X0)
						xmultiplier=FLOOR(X0)+0x10000-X0;
					else
						xmultiplier=0x10000;
					integer32 compound_mutiplier=((ymultiplier>>8)*xmultiplier)>>8;
					r+=pixel0[Roffset0]*compound_mutiplier;
					g+=pixel0[Goffset0]*compound_mutiplier;
					b+=pixel0[Boffset0]*compound_mutiplier;
					pixel0+=advance0;
					x2=FLOOR(x2)+0x10000;
				}
				pixel0=pixel00+pitch0;
				y2=FLOOR(y2)+0x10000;
			}
			pixel1[Roffset1]=r/area;
			pixel1[Goffset1]=g/area;
			pixel1[Boffset1]=b/area;
			pixel1+=advance1;
			X0=X1;
			X1+=sizeX;
		}
		pixel1=pixel10+pitch1;
		Y0=Y1;
		Y1+=sizeY;
	}
	SDL_UnlockSurface(src);
	SDL_UnlockSurface(dst);
}
#else
void bilinearInterpolation2_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor);
int bilinearInterpolation2_threaded(void *parameters);

void bilinearInterpolation2(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor){
	if (!src || !dst)
		return;
	SDL_Rect srcRect0,dstRect0;
	//assign SDL_Rects
	if (!srcRect){
		srcRect0.x=0;
		srcRect0.y=0;
		srcRect0.w=src->w;
		srcRect0.h=src->h;
	}else
		srcRect0=*srcRect;
	if (!dstRect){
		dstRect0.x=0;
		dstRect0.y=0;
		dstRect0.w=dst->w;
		dstRect0.h=dst->h;
	}else
		dstRect0=*dstRect;
	//correct SDL_Rects
	if (srcRect0.x+srcRect0.w>src->w)
		srcRect0.w-=srcRect0.x+srcRect0.w-src->w;
	if (srcRect0.y+srcRect0.h>src->h)
		srcRect0.h-=srcRect0.y+srcRect0.h-src->h;
	if (dstRect0.x+dstRect0.w>dst->w)
		srcRect0.w-=dstRect0.x+srcRect0.w-dst->w;
	if (dstRect0.y+dstRect0.h>dst->h)
		srcRect0.h-=dstRect0.y+srcRect0.h-dst->h;
	//check SDL_Rects
	if (srcRect0.x<0 || srcRect0.x>=src->w || srcRect0.y<0 || srcRect0.y>=src->h || srcRect0.w<=0 || srcRect0.h<=0 ||
		dstRect0.x<0 || dstRect0.x>=dst->w || dstRect0.y<0 || dstRect0.y>=dst->h || dstRect0.w<=0 || dstRect0.h<=0)
		return;
	if (srcRect0.w==dstRect0.w && srcRect0.h==dstRect0.h){
		manualBlit(src,&srcRect0,dst,&dstRect0);
		return;
	}
	if (src->format->BitsPerPixel<24 || dst->format->BitsPerPixel<24)
		return;
	SDL_FillRect(dst,&dstRect0,dst->format->Amask|dst->format->Gmask);

	SDL_Thread **threads=new SDL_Thread *[cpu_count];
	SDL_Rect *rects0=new SDL_Rect[cpu_count];
	SDL_Rect *rects1=new SDL_Rect[cpu_count];
	IF_parameters *parameters=new IF_parameters[cpu_count];
	ulong division0=float(srcRect0.h)/float(cpu_count);
	ulong division1=float(dstRect0.h)/float(cpu_count);
	ulong total1=0;
	for (ushort a=0;a<cpu_count;a++){
		rects0[a]=srcRect0;
		rects1[a]=dstRect0;
		rects0[a].y+=Sint16(a*division0);
		rects1[a].y+=Sint16(a*division1);
		rects1[a].h=Sint16(division1);
		
		total1+=rects1[a].h;
		parameters[a].src=src;
		parameters[a].srcRect=rects0+a;
		parameters[a].dst=dst;
		parameters[a].dstRect=rects1+a;
		parameters[a].x_factor=x_factor;
		parameters[a].y_factor=y_factor;
	}
	rects1[cpu_count-1].h+=dstRect0.h-total1;

	SDL_LockSurface(src);
	SDL_LockSurface(dst);
	for (ushort a=0;a<cpu_count;a++)
		threads[a]=SDL_CreateThread(bilinearInterpolation2_threaded,parameters+a);
	for (ushort a=0;a<cpu_count;a++)
		SDL_WaitThread(threads[a],0);
	SDL_UnlockSurface(src);
	SDL_UnlockSurface(dst);
	delete[] threads;
	delete[] rects0;
	delete[] rects1;
	delete[] parameters;
}

int bilinearInterpolation2_threaded(void *parameters){
	IF_parameters *p=(IF_parameters *)parameters;
	bilinearInterpolation2_threaded(p->src,p->srcRect,p->dst,p->dstRect,p->x_factor,p->y_factor);
	return 0;
}

void bilinearInterpolation2_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,ulong x_factor,ulong y_factor){
	SDL_Rect srcRect0=*srcRect,
		dstRect0=*dstRect;
	uchar *pixels0=(uchar *)src->pixels;
	uchar *pixels1=(uchar *)dst->pixels;
	uchar Roffset0=(src->format->Rshift)>>3;
	uchar Goffset0=(src->format->Gshift)>>3;
	uchar Boffset0=(src->format->Bshift)>>3;
	uchar Roffset1=(dst->format->Rshift)>>3;
	uchar Goffset1=(dst->format->Gshift)>>3;
	uchar Boffset1=(dst->format->Bshift)>>3;
	uchar advance0=src->format->BytesPerPixel;
	uchar advance1=dst->format->BytesPerPixel;
	Uint16 pitch0=src->pitch;
	Uint16 pitch1=dst->pitch;
	uchar *pos0=pixels0;
	uchar *pos1=pixels1;
	Uint32 sizeX=(src->clip_rect.w<<16)/dst->clip_rect.w,
		sizeY=(src->clip_rect.h<<16)/dst->clip_rect.h;
	srcRect0.w+=srcRect0.x;
	dstRect0.w+=dstRect0.x;
	srcRect0.h+=srcRect0.y;
	dstRect0.h+=dstRect0.y;
	Uint32 Y0=(dstRect0.y-dst->clip_rect.y)*sizeY,
		Y1=Y0+sizeY,
		X0,X1;
	Uint32 area=((sizeX>>8)*sizeY)>>8;
	uchar *pixel1=pos1+pitch1*dstRect0.y+advance1*dstRect0.x;
	for (long y1=dstRect0.y;y1<dstRect0.h;y1++){
		X0=(dstRect0.x-dst->clip_rect.x)*sizeX,
		X1=X0+sizeX;
		uchar *row=pos0+pitch0*(Y0>>16);
		uchar *pixel10=pixel1;
		for (long x1=dstRect0.x;x1<dstRect0.w;x1++){
			uchar *pixel0=row+advance0*(X0>>16);
			unsigned r=0,g=0,b=0;
			for (Uint32 y2=Y0;y2<Y1;){
				Uint32 ymultiplier;
				uchar *pixel00=pixel0;
				if (Y1-y2<0x10000)
					ymultiplier=Y1-y2;
				else if (y2==Y0)
					ymultiplier=FLOOR(Y0)+0x10000-Y0;
				else
					ymultiplier=0x10000;
				for (Uint32 x2=X0;x2<X1;){
					Uint32 xmultiplier;
					if (X1-x2<0x10000)
						xmultiplier=X1-x2;
					else if (x2==X0)
						xmultiplier=FLOOR(X0)+0x10000-X0;
					else
						xmultiplier=0x10000;
					Uint32 compound_mutiplier=((ymultiplier>>8)*xmultiplier)>>8;
					r+=pixel0[Roffset0]*compound_mutiplier;
					g+=pixel0[Goffset0]*compound_mutiplier;
					b+=pixel0[Boffset0]*compound_mutiplier;
					pixel0+=advance0;
					x2=FLOOR(x2)+0x10000;
				}
				pixel0=pixel00+pitch0;
				y2=FLOOR(y2)+0x10000;
			}
			pixel1[Roffset1]=r/area;
			pixel1[Goffset1]=g/area;
			pixel1[Boffset1]=b/area;
			pixel1+=advance1;
			X0=X1;
			X1+=sizeX;
		}
		pixel1=pixel10+pitch1;
		Y0=Y1;
		Y1+=sizeY;
	}
}
#endif
#endif
