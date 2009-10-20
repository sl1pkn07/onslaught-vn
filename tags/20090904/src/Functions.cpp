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

#ifndef NONS_FUNCTIONS_CPP
#define NONS_FUNCTIONS_CPP

#define USE_ACCURATE_MULTIPLICATION
#include "Functions.h"
#ifndef TOOLS_BARE_FILE
#include <bzlib.h>
#ifndef TOOLS_NSAIO
#include "Globals.h"
#else
#include "enums.h"
#endif
#include "ThreadManager.h"

#include "UTF.h"
#include <cmath>

//(Parallelized surface function)
struct PSF_parameters{
	SDL_Surface *src;
	SDL_Rect *srcRect;
	SDL_Surface *dst;
	SDL_Rect *dstRect;
	manualBlitAlpha_t alpha;
	SDL_Color color;
};

bool getbit(uchar *arr,ulong *byteoffset,uchar *bitoffset){
	bool res=(arr[*byteoffset]>>(7-*bitoffset))&1;
	(*bitoffset)++;
	if (*bitoffset>7){
		(*byteoffset)++;
		*bitoffset=0;
	}
	return res;
}

ulong getbits(uchar *arr,uchar bits,ulong *byteoffset,uchar *bitoffset){
	ulong res=0;
	if (bits>sizeof(ulong)*8)
		bits=sizeof(ulong)*8;
	for (;bits>0;bits--){
		res<<=1;
		res|=(ulong)getbit(arr,byteoffset,bitoffset);
	}
	return res;
}

Uint32 secondsSince1970(){
	return time(0);
}

#if !defined(TOOLS_BARE_FILE) && !defined(TOOLS_NSAIO)
void manualBlit_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,manualBlitAlpha_t alpha);
#ifndef USE_THREAD_MANAGER
int 
#else
void 
#endif
manualBlit_threaded(void *parameters);

void manualBlit(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,manualBlitAlpha_t alpha){
	if (!src || !dst || src->format->BitsPerPixel<24 ||dst->format->BitsPerPixel<24 || !alpha)
		return;
	SDL_Rect srcRect0,dstRect0;
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

	if (srcRect0.x<0){
		if (srcRect0.w<=-srcRect0.x)
			return;
		srcRect0.w+=srcRect0.x;
		srcRect0.x=0;
	}else if (srcRect0.x>=src->w)
		return;
	if (srcRect0.y<0){
		if (srcRect0.h<=-srcRect0.y)
			return;
		srcRect0.h+=srcRect0.y;
		srcRect0.y=0;
	}else if (srcRect0.y>=src->h)
		return;
	if (srcRect0.x+srcRect0.w>src->w)
		srcRect0.w-=srcRect0.x+srcRect0.w-src->w;
	if (srcRect0.y+srcRect0.h>src->h)
		srcRect0.h-=srcRect0.y+srcRect0.h-src->h;
	if (dstRect0.x+srcRect0.w>dst->w)
		srcRect0.w-=dstRect0.x+srcRect0.w-dst->w;
	if (dstRect0.y+srcRect0.h>dst->h)
		srcRect0.h-=dstRect0.y+srcRect0.h-dst->h;

	if (dstRect0.x<0){
		srcRect0.x=-dstRect0.x;
		srcRect0.w+=dstRect0.x;
		dstRect0.x=0;
	}else if (dstRect0.x>=dst->w)
		return;
	if (dstRect0.y<0){
		srcRect0.y=-dstRect0.y;
		srcRect0.h+=dstRect0.y;
		dstRect0.y=0;
	}else if (dstRect0.y>=dst->h)
		return;

	if (srcRect0.w<=0 || srcRect0.h<=0)
		return;


	SDL_LockSurface(src);
	SDL_LockSurface(dst);
	if (cpu_count==1 || srcRect0.w*srcRect0.h<5000){
		manualBlit_threaded(src,&srcRect0,dst,&dstRect0,alpha);
		SDL_UnlockSurface(dst);
		SDL_UnlockSurface(src);
		return;
	}
#ifndef USE_THREAD_MANAGER
	SDL_Thread **threads=new SDL_Thread *[cpu_count];
#endif
	SDL_Rect *rects0=new SDL_Rect[cpu_count];
	SDL_Rect *rects1=new SDL_Rect[cpu_count];
	PSF_parameters *parameters=new PSF_parameters[cpu_count];
	ulong division=float(srcRect0.h)/float(cpu_count);
	ulong total=0;
	for (ushort a=0;a<cpu_count;a++){
		rects0[a]=srcRect0;
		rects1[a]=dstRect0;
		rects0[a].y+=Sint16(a*division);
		rects0[a].h=Sint16(division);
		rects1[a].y+=Sint16(a*division);
		total+=rects0[a].h;
		parameters[a].src=src;
		parameters[a].srcRect=rects0+a;
		parameters[a].dst=dst;
		parameters[a].dstRect=rects1+a;
		parameters[a].alpha=alpha;
	}
	rects0[cpu_count-1].h+=srcRect0.h-total;
	//ulong t0=SDL_GetTicks();
	for (ushort a=1;a<cpu_count;a++)
#ifndef USE_THREAD_MANAGER
		threads[a]=SDL_CreateThread(manualBlit_threaded,parameters+a);
#else
		threadManager.call(a-1,manualBlit_threaded,parameters+a);
#endif
	manualBlit_threaded(parameters);
#ifndef USE_THREAD_MANAGER
	for (ushort a=1;a<cpu_count;a++)
		SDL_WaitThread(threads[a],0);
#else
	threadManager.waitAll();
#endif
	//ulong t1=SDL_GetTicks();
	//o_stderr <<"Done in "<<t1-t0<<" ms."<<std::endl;
	SDL_UnlockSurface(dst);
	SDL_UnlockSurface(src);
#ifndef USE_THREAD_MANAGER
	delete[] threads;
#endif
	delete[] rects0;
	delete[] rects1;
	delete[] parameters;
}

#ifndef USE_THREAD_MANAGER
int 
#else
void 
#endif
manualBlit_threaded(void *parameters){
	PSF_parameters *p=(PSF_parameters *)parameters;
	manualBlit_threaded(p->src,p->srcRect,p->dst,p->dstRect,p->alpha);
#ifndef USE_THREAD_MANAGER
	return 0;
#endif
}

void manualBlit_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,manualBlitAlpha_t alpha){
	SDL_Rect &srcRect0=*srcRect,
		&dstRect0=*dstRect;
	int w0=srcRect0.w, h0=srcRect0.h;
	if (srcRect0.w<=0 || srcRect0.h<=0)
		return;

	uchar *pos0=(uchar *)src->pixels;
	uchar *pos1=(uchar *)dst->pixels;

	uchar Roffset0=(src->format->Rshift)>>3;
	uchar Goffset0=(src->format->Gshift)>>3;
	uchar Boffset0=(src->format->Bshift)>>3;
	uchar Aoffset0=(src->format->Ashift)>>3;

	uchar Roffset1=(dst->format->Rshift)>>3;
	uchar Goffset1=(dst->format->Gshift)>>3;
	uchar Boffset1=(dst->format->Bshift)>>3;
	uchar Aoffset1=(dst->format->Ashift)>>3;

	unsigned advance0=src->format->BytesPerPixel;
	unsigned advance1=dst->format->BytesPerPixel;

	pos0+=src->pitch*srcRect0.y+srcRect0.x*advance0;
	pos1+=dst->pitch*dstRect0.y+dstRect0.x*advance1;

	bool alpha0=(Aoffset0!=Roffset0 && Aoffset0!=Goffset0 && Aoffset0!=Boffset0);
	bool alpha1=(Aoffset1!=Roffset1 && Aoffset1!=Goffset1 && Aoffset1!=Boffset1);

	bool negate=(alpha<0);
	if (negate)
		alpha=-alpha;

	for (int y0=0;y0<h0;y0++){
		uchar *pos00=pos0;
		uchar *pos10=pos1;
		for (int x0=0;x0<w0;x0++){
			long r0=pos0[Roffset0],
				g0=pos0[Goffset0],
				b0=pos0[Boffset0],
				a0=255;

			uchar *r1=pos1+Roffset1;
			uchar *g1=pos1+Goffset1;
			uchar *b1=pos1+Boffset1;
			uchar *a1=pos1+Aoffset1;
  
#if 0
//BACKUP NOTE: This code had an accuracy problem when blending RGBA -> RGBA, but
//             I'm keeping it here just in case, since it otherwise works fine.

			if (alpha0){
				a0=uchar((short(pos0[Aoffset0])*short(alpha))/255);
				*r1=((255-a0)*(*r1))/255+(a0*r0)/255;
				*g1=((255-a0)*(*g1))/255+(a0*g0)/255;
				*b1=((255-a0)*(*b1))/255+(a0*b0)/255;
				if (alpha1){
					uchar *a1=pos1+Aoffset1;
					short temp=*a1+a0;
					*a1=temp>255?255:temp;
				}
			}else if (alpha<255){
				a0=255-alpha;
				*r1=(a0*(*r1))/255+(alpha*r0)/255;
				*g1=(a0*(*g1))/255+(alpha*g0)/255;
				*b1=(a0*(*b1))/255+(alpha*b0)/255;
				if (alpha1){
					uchar *a1=pos1+Aoffset1;
					short temp=*a1+a0;
					*a1=temp>255?255:temp;
				}
			}else{
				*r1=r0;
				*g1=g0;
				*b1=b0;
				if (alpha1)
					pos1[Aoffset1]=0xFF;
			}
#else
#define APPLY_ALPHA(c0,c1,a) (INTEGER_MULTIPLICATION((a)^0xFF,(c1))+INTEGER_MULTIPLICATION((a),(c0)))

			if (alpha==255){
				if (!alpha0){
					*r1=r0;
					*g1=g0;
					*b1=b0;
					if (alpha1)
						*a1=0xFF;
				}else{
					a0=pos0[Aoffset0];
					if (!alpha1){
						*r1=APPLY_ALPHA(r0,*r1,a0);
						*g1=APPLY_ALPHA(g0,*g1,a0);
						*b1=APPLY_ALPHA(b0,*b1,a0);
					}else{
						ulong el;
#ifdef LOOKUP_BLEND_CONSTANT
						el=blendData[*a1+(a0<<8)];
#else
						ulong previous=*a1;
						*a1=INTEGER_MULTIPLICATION(a0^0xFF,*a1^0xFF)^0xFF;
						el=(!a0 && !previous)?0:(a0*255)/(*a1);
#endif
						*r1=APPLY_ALPHA(r0,*r1,el);
						*g1=APPLY_ALPHA(g0,*g1,el);
						*b1=APPLY_ALPHA(b0,*b1,el);
#ifdef LOOKUP_BLEND_CONSTANT
						*a1=INTEGER_MULTIPLICATION(a0^0xFF,*a1^0xFF)^0xFF;
#endif
					}
				}
			}else{
				if (!alpha0){
					if (!alpha1){
						*r1=APPLY_ALPHA(r0,*r1,alpha);
						*g1=APPLY_ALPHA(g0,*g1,alpha);
						*b1=APPLY_ALPHA(b0,*b1,alpha);
					}else{
						ulong el;
#ifdef LOOKUP_BLEND_CONSTANT
						el=blendData[*a1+(alpha<<8)];
#else
						ulong previous=*a1;
						*a1=INTEGER_MULTIPLICATION(alpha^0xFF,*a1^0xFF)^0xFF;
						el=(!alpha && !previous)?0:(alpha*255)/(*a1);
#endif
						*r1=APPLY_ALPHA(r0,*r1,el);
						*g1=APPLY_ALPHA(g0,*g1,el);
						*b1=APPLY_ALPHA(b0,*b1,el);
#ifdef LOOKUP_BLEND_CONSTANT
						*a1=INTEGER_MULTIPLICATION(alpha^0xFF,*a1^0xFF)^0xFF;
#endif
					}
				}else{
					a0=INTEGER_MULTIPLICATION(pos0[Aoffset0],alpha);
					if (!alpha1){
						*r1=APPLY_ALPHA(r0,*r1,a0);
						*g1=APPLY_ALPHA(g0,*g1,a0);
						*b1=APPLY_ALPHA(b0,*b1,a0);
					}else{
						ulong el;
#ifdef LOOKUP_BLEND_CONSTANT
						el=blendData[*a1+(a0<<8)];
#else
						ulong previous=*a1;
						*a1=INTEGER_MULTIPLICATION(a0^0xFF,*a1^0xFF)^0xFF;
						el=(!a0 && !previous)?0:(a0*255)/(*a1);
#endif
						*r1=APPLY_ALPHA(r0,*r1,el);
						*g1=APPLY_ALPHA(g0,*g1,el);
						*b1=APPLY_ALPHA(b0,*b1,el);
#ifdef LOOKUP_BLEND_CONSTANT
						*a1=INTEGER_MULTIPLICATION(a0^0xFF,*a1^0xFF)^0xFF;
#endif
					}
				}
			}
#endif

			if (negate && a0){
				*r1=~*r1;
				*g1=~*g1;
				*b1=~*b1;
			}

			pos0+=advance0;
			pos1+=advance1;
		}
		pos0=pos00+src->pitch;
		pos1=pos10+dst->pitch;
	}
}

void multiplyBlend_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect);
#ifndef USE_THREAD_MANAGER
int 
#else
void 
#endif
multiplyBlend_threaded(void *parameters);

void multiplyBlend(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect){
	if (!src || !dst || src->format->BitsPerPixel<24 ||dst->format->BitsPerPixel<24)
		return;
	SDL_Rect srcRect0,dstRect0;
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

	if (srcRect0.x<0){
		if (srcRect0.w<=-srcRect0.x)
			return;
		srcRect0.w+=srcRect0.x;
		srcRect0.x=0;
	}else if (srcRect0.x>=src->w)
		return;
	if (srcRect0.y<0){
		if (srcRect0.h<=-srcRect0.y)
			return;
		srcRect0.h+=srcRect0.y;
		srcRect0.y=0;
	}else if (srcRect0.y>=src->h)
		return;
	if (srcRect0.x+srcRect0.w>src->w)
		srcRect0.w-=srcRect0.x+srcRect0.w-src->w;
	if (srcRect0.y+srcRect0.h>src->h)
		srcRect0.h-=srcRect0.y+srcRect0.h-src->h;
	if (dstRect0.x+srcRect0.w>dst->w)
		srcRect0.w-=dstRect0.x+srcRect0.w-dst->w;
	if (dstRect0.y+srcRect0.h>dst->h)
		srcRect0.h-=dstRect0.y+srcRect0.h-dst->h;

	if (dstRect0.x<0){
		srcRect0.x=-dstRect0.x;
		srcRect0.w+=dstRect0.x;
		dstRect0.x=0;
	}else if (dstRect0.x>=dst->w)
		return;
	if (dstRect0.y<0){
		dstRect0.h+=dstRect0.y;
		dstRect0.y=0;
	}else if (dstRect0.y>=dst->h)
		return;

	if (srcRect0.w<=0 || srcRect0.h<=0)
		return;

	SDL_LockSurface(src);
	SDL_LockSurface(dst);
	if (cpu_count==1 || srcRect0.w*srcRect0.h<1000){
		multiplyBlend_threaded(src,&srcRect0,dst,&dstRect0);
		SDL_UnlockSurface(dst);
		SDL_UnlockSurface(src);
		return;
	}
#ifndef USE_THREAD_MANAGER
	SDL_Thread **threads=new SDL_Thread *[cpu_count];
#endif
	SDL_Rect *rects0=new SDL_Rect[cpu_count];
	SDL_Rect *rects1=new SDL_Rect[cpu_count];
	PSF_parameters *parameters=new PSF_parameters[cpu_count];
	ulong division=float(srcRect0.h)/float(cpu_count);
	ulong total=0;
	for (ushort a=0;a<cpu_count;a++){
		rects0[a]=srcRect0;
		rects1[a]=dstRect0;
		rects0[a].y+=Sint16(a*division);
		rects0[a].h=Sint16(division);
		rects1[a].y+=Sint16(a*division);
		total+=rects0[a].h;
		parameters[a].src=src;
		parameters[a].srcRect=rects0+a;
		parameters[a].dst=dst;
		parameters[a].dstRect=rects1+a;
	}
	rects0[cpu_count-1].h+=srcRect0.h-total;
	for (ushort a=1;a<cpu_count;a++)
#ifndef USE_THREAD_MANAGER
		threads[a]=SDL_CreateThread(multiplyBlend_threaded,parameters+a);
#else
		threadManager.call(a-1,multiplyBlend_threaded,parameters+a);
#endif
	multiplyBlend_threaded(parameters);
#ifndef USE_THREAD_MANAGER
	for (ushort a=1;a<cpu_count;a++)
		SDL_WaitThread(threads[a],0);
#else
	threadManager.waitAll();
#endif
	SDL_UnlockSurface(dst);
	SDL_UnlockSurface(src);
#ifndef USE_THREAD_MANAGER
	delete[] threads;
#endif
	delete[] rects0;
	delete[] rects1;
	delete[] parameters;
}

#ifndef USE_THREAD_MANAGER
int 
#else
void 
#endif
multiplyBlend_threaded(void *parameters){
	PSF_parameters *p=(PSF_parameters *)parameters;
	multiplyBlend_threaded(p->src,p->srcRect,p->dst,p->dstRect);
#ifndef USE_THREAD_MANAGER
	return 0;
#endif
}

void multiplyBlend_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect){
	SDL_Rect &srcRect0=*srcRect,
		&dstRect0=*dstRect;
	int w0=srcRect0.w, h0=srcRect0.h;

	uchar *pos0=(uchar *)src->pixels;
	uchar *pos1=(uchar *)dst->pixels;

	uchar Roffset0=(src->format->Rshift)>>3;
	uchar Goffset0=(src->format->Gshift)>>3;
	uchar Boffset0=(src->format->Bshift)>>3;

	uchar Roffset1=(dst->format->Rshift)>>3;
	uchar Goffset1=(dst->format->Gshift)>>3;
	uchar Boffset1=(dst->format->Bshift)>>3;

	unsigned advance0=src->format->BytesPerPixel;
	unsigned advance1=dst->format->BytesPerPixel;

	pos0+=src->pitch*srcRect0.y+srcRect0.x*advance0;
	pos1+=dst->pitch*dstRect0.y+dstRect0.x*advance1;

	for (int y0=0;y0<h0;y0++){
		uchar *pos00=pos0;
		uchar *pos10=pos1;
		for (int x0=0;x0<w0;x0++){
			uchar r0=pos0[Roffset0];
			uchar g0=pos0[Goffset0];
			uchar b0=pos0[Boffset0];

			uchar *r1=pos1+Roffset1;
			uchar *g1=pos1+Goffset1;
			uchar *b1=pos1+Boffset1;

			*r1=uchar((short(r0)*short(*r1))/255);
			*g1=uchar((short(g0)*short(*g1))/255);
			*b1=uchar((short(b0)*short(*b1))/255);

			pos0+=advance0;
			pos1+=advance1;
		}
		pos0=pos00+src->pitch;
		pos1=pos10+dst->pitch;
	}
}

void getFinalSize(SDL_Surface *src,float matrix[4],ulong &w,ulong &h){
	ulong w0=src->w,
		h0=src->h;
	float coords[][2]={
		{0,0},
		{0,h0},
		{w0,0},
		{w0,h0}
	};
	float minx,miny,maxx,maxy;
	maxx=minx=coords[0][0]*matrix[0]+coords[0][1]*matrix[1];
	maxy=miny=coords[0][0]*matrix[2]+coords[0][1]*matrix[3];
	for (int a=1;a<4;a++){
		float x=coords[a][0]*matrix[0]+coords[a][1]*matrix[1];
		float y=coords[a][0]*matrix[2]+coords[a][1]*matrix[3];
		if (x<minx)
			minx=x;
		if (x>maxx)
			maxx=x;
		if (y<miny)
			miny=y;
		if (y>maxy)
			maxy=y;
	}
	w=maxx-minx;
	h=maxy-miny;
}

void invert_matrix(float m[4]){
	float temp=1/(m[0]*m[3]-m[1]*m[2]);
	float m2[]={
		temp*m[3],
		temp*-m[1],
		temp*-m[2],
		temp*m[0]
	};
	memcpy(m,m2,4*sizeof(float));
}

void getCorrected(ulong &x0,ulong &y0,float matrix[4]){
	float coords[][2]={
		{0,0},
		{0,y0},
		{x0,0},
		{x0,y0}
	};
	float minx,miny;
	minx=coords[0][0]*matrix[0]+coords[0][1]*matrix[1];
	miny=coords[0][0]*matrix[2]+coords[0][1]*matrix[3];
	for (int a=1;a<4;a++){
		float x=coords[a][0]*matrix[0]+coords[a][1]*matrix[1];
		float y=coords[a][0]*matrix[2]+coords[a][1]*matrix[3];
		if (x<minx)
			minx=x;
		if (y<miny)
			miny=y;
	}
	x0=-minx;
	y0=-miny;
}

struct applyTransformationMatrix_parameters{
	uchar *src,
		*dst;
	ulong x,y,
		w0,h0,
		w1,h1,
		advance0,advance1,
		pitch0,pitch1;
	long matrix[4];
	ulong correct_x,correct_y;
};

void applyTransformationMatrix_threaded(const applyTransformationMatrix_parameters &param);
#ifndef USE_THREAD_MANAGER
int 
#else
void 
#endif
applyTransformationMatrix_threaded(void *parameters);

SDL_Surface *applyTransformationMatrix(SDL_Surface *src,float matrix[4]){
	if (!src || src->format->BitsPerPixel<24 || !(matrix[0]*matrix[3]-matrix[1]*matrix[2]))
		return 0;
	ulong w,h;
	float inv_matrix[4];
	memcpy(inv_matrix,matrix,4*sizeof(float));
	invert_matrix(inv_matrix);
	
	getFinalSize(src,matrix,w,h);
	SDL_Surface *res=makeSurface(w,h,src->format->BitsPerPixel,
		src->format->Rmask,
		src->format->Gmask,
		src->format->Bmask,
		src->format->Amask);
	//SDL_FillRect(res,0,src->format->Gmask|src->format->Amask);
	
	ulong correct_x=src->w,
		correct_y=src->h;
	getCorrected(correct_x,correct_y,matrix);

	SDL_UnlockSurface(src);
	SDL_UnlockSurface(res);

	ulong division=float(h)/float(cpu_count);
#ifndef USE_THREAD_MANAGER
	SDL_Thread **threads=new SDL_Thread *[cpu_count];
#endif
	applyTransformationMatrix_parameters *parameters=new applyTransformationMatrix_parameters[cpu_count];
	ulong total=0;
	for (ulong a=0;a<cpu_count;a++){
		parameters[a].src=(uchar *)src->pixels;
		parameters[a].dst=(uchar *)res->pixels;
		parameters[a].x=0;
		parameters[a].y=a*division;
		parameters[a].w0=src->w;
		parameters[a].h0=src->h;
		parameters[a].w1=w;
		parameters[a].h1=division;
		total+=division;
		parameters[a].advance0=src->format->BytesPerPixel;
		parameters[a].advance1=res->format->BytesPerPixel;
		parameters[a].pitch0=src->pitch;
		parameters[a].pitch1=res->pitch;

		parameters[a].dst+=parameters[a].y*parameters[a].pitch1;

		for (int b=0;b<4;b++)
			parameters[a].matrix[b]=inv_matrix[b]*0x10000;
		parameters[a].correct_x=correct_x;
		parameters[a].correct_y=correct_y;
	}
	parameters[cpu_count-1].h1+=h-total;
	for (ulong a=1;a<cpu_count;a++)
#ifndef USE_THREAD_MANAGER
		threads[a]=SDL_CreateThread(applyTransformationMatrix_threaded,parameters+a);
#else
		threadManager.call(a-1,applyTransformationMatrix_threaded,parameters+a);
#endif
	applyTransformationMatrix_threaded(parameters);
#ifndef USE_THREAD_MANAGER
	for (ushort a=1;a<cpu_count;a++)
		SDL_WaitThread(threads[a],0);
#else
	threadManager.waitAll();
#endif
	SDL_UnlockSurface(src);
	SDL_UnlockSurface(res);
#ifndef USE_THREAD_MANAGER
	delete[] threads;
#endif
	delete[] parameters;
	return res;
}

#ifndef USE_THREAD_MANAGER
int 
#else
void
#endif
applyTransformationMatrix_threaded(void *parameters){
	applyTransformationMatrix_threaded(*(applyTransformationMatrix_parameters *)parameters);
#ifndef USE_THREAD_MANAGER
	return 0;
#endif
}

void applyTransformationMatrix_threaded(const applyTransformationMatrix_parameters &param){
	uchar *src=param.src,
		*dst=param.dst;
	ulong x0=param.x,
		y0=param.y,
		w0=param.w0,
		h0=param.h0,
		w1=param.w1,
		h1=param.h1+y0,
		advance0=param.advance0,
		advance1=param.advance1,
		pitch0=param.pitch0,
		pitch1=param.pitch1,
		correct_x=param.correct_x,
		correct_y=param.correct_y;
	assert(advance0==advance1);
	const long *matrix=param.matrix;
	for (ulong y=y0;y<h1;y++){
		uchar *dst0=dst;
		bool pixels_were_copied=0;
		for (ulong x=x0;x<w1;x++){
			long src_x=(x-correct_x)*matrix[0]+(y-correct_y)*matrix[1];
			long src_y=(x-correct_x)*matrix[2]+(y-correct_y)*matrix[3];
			src_x>>=16;
			src_y>>=16;
			if (src_x<0 || src_y<0 || (ulong)src_x>=w0 || (ulong)src_y>=h0){
				if (!pixels_were_copied){
					dst+=advance1;
					continue;
				}
				break;
			}
			uchar *src2=src+src_x*advance0+src_y*pitch0;
			for (ulong a=0;a<advance0;a++)
				*dst++=*src2++;
			pixels_were_copied=1;
		}
		dst=dst0+pitch1;
	}
}

void FlipSurfaceH(SDL_Surface *src,SDL_Surface *dst){
	if (!src || !dst || src->w!=dst->w || src->h!=dst->h)
		return;
	if (src==dst){
		SDL_LockSurface(src);

		ulong w=src->w,
			w0=w,
			h=src->h;
		w/=2;

		uchar *pos=(uchar *)src->pixels;
		
		unsigned advance=src->format->BytesPerPixel;
		unsigned pitch=src->pitch;

		Uint32 putMask=(checkNativeEndianness()==NONS_LITTLE_ENDIAN)?((~0)<<(advance*8)):((~0)>>(advance*8)),
			getMask=~putMask;
		
		for (ulong y=0;y<h;y++){
			uchar *pos0=pos,
				*pos_reverse=pos+(w0-1)*advance;
			for (ulong x=0;x<w;x++){
				Uint32 pixelA=*(Uint32 *)pos&getMask,
					pixelB=*(Uint32 *)pos_reverse&getMask;
				*(Uint32 *)pos&=putMask;
				*(Uint32 *)pos|=pixelB;
				*(Uint32 *)pos_reverse&=putMask;
				*(Uint32 *)pos_reverse|=pixelB;
				pos+=advance;
				pos_reverse-=advance;
			}
			pos=pos0+pitch;
		}

		SDL_UnlockSurface(src);
	}else{
		SDL_LockSurface(src);
		SDL_LockSurface(dst);

		ulong w=src->w,
			h=src->h;

		uchar *pos0=(uchar *)src->pixels;
		uchar *pos1=(uchar *)dst->pixels;

		uchar Roffset0=(src->format->Rshift)>>3;
		uchar Goffset0=(src->format->Gshift)>>3;
		uchar Boffset0=(src->format->Bshift)>>3;
		uchar Aoffset0=(src->format->Ashift)>>3;

		uchar Roffset1=(dst->format->Rshift)>>3;
		uchar Goffset1=(dst->format->Gshift)>>3;
		uchar Boffset1=(dst->format->Bshift)>>3;
		uchar Aoffset1=(dst->format->Ashift)>>3;

		bool alpha0=(Aoffset0!=Roffset0 && Aoffset0!=Goffset0 && Aoffset0!=Boffset0);
		bool alpha1=(Aoffset1!=Roffset1 && Aoffset1!=Goffset1 && Aoffset1!=Boffset1);

		unsigned advance0=src->format->BytesPerPixel;
		unsigned pitch0=src->pitch;
		unsigned advance1=dst->format->BytesPerPixel;
		unsigned pitch1=dst->pitch;

		pos1+=(w-1)*advance1;

		for (ulong y=0;y<h;y++){
			uchar *pos00=pos0,
				*pos10=pos1;
			for (ulong x=0;x<w;x++){
				pos1[Roffset1]=pos0[Roffset0];
				pos1[Goffset1]=pos0[Goffset0];
				pos1[Boffset1]=pos0[Boffset0];
				if (alpha1){
					if (alpha0)
						pos1[Aoffset1]=pos0[Aoffset0];
					else
						pos1[Aoffset1]=0xFF;
				}

				pos0+=advance0;
				pos1-=advance1;
			}
			pos0=pos00+pitch0;
			pos1=pos10+pitch1;
		}

		SDL_UnlockSurface(src);
		SDL_UnlockSurface(dst);
	}
}

void FlipSurfaceV(SDL_Surface *src,SDL_Surface *dst){
	if (!src || !dst || src->w!=dst->w || src->h!=dst->h)
		return;
	if (src==dst){
		SDL_LockSurface(src);

		ulong w=src->w,
			h=src->h,
			h0=h;
		h/=2;

		uchar *pos=(uchar *)src->pixels;
		
		unsigned advance=src->format->BytesPerPixel;
		unsigned pitch=src->pitch;

		Uint32 putMask=(checkNativeEndianness()==NONS_LITTLE_ENDIAN)?((~0)<<(advance*8)):((~0)>>(advance*8)),
			getMask=~putMask;
		
		for (ulong x=0;x<w;x++){
			uchar *pos0=pos,
				*pos_reverse=pos+(h0-1)*pitch;
			for (ulong y=0;y<h;y++){
				Uint32 pixelA=*(Uint32 *)pos&getMask,
					pixelB=*(Uint32 *)pos_reverse&getMask;
				*(Uint32 *)pos&=putMask;
				*(Uint32 *)pos|=pixelB;
				*(Uint32 *)pos_reverse&=putMask;
				*(Uint32 *)pos_reverse|=pixelB;
				pos+=pitch;
				pos_reverse-=pitch;
			}
			pos=pos0+advance;
		}

		SDL_UnlockSurface(src);
	}else{
		SDL_LockSurface(src);
		SDL_LockSurface(dst);

		ulong w=src->w,
			h=src->h;

		uchar *pos0=(uchar *)src->pixels;
		uchar *pos1=(uchar *)dst->pixels;

		uchar Roffset0=(src->format->Rshift)>>3;
		uchar Goffset0=(src->format->Gshift)>>3;
		uchar Boffset0=(src->format->Bshift)>>3;
		uchar Aoffset0=(src->format->Ashift)>>3;

		uchar Roffset1=(dst->format->Rshift)>>3;
		uchar Goffset1=(dst->format->Gshift)>>3;
		uchar Boffset1=(dst->format->Bshift)>>3;
		uchar Aoffset1=(dst->format->Ashift)>>3;

		bool alpha0=(Aoffset0!=Roffset0 && Aoffset0!=Goffset0 && Aoffset0!=Boffset0);
		bool alpha1=(Aoffset1!=Roffset1 && Aoffset1!=Goffset1 && Aoffset1!=Boffset1);

		unsigned advance0=src->format->BytesPerPixel;
		unsigned pitch0=src->pitch;
		unsigned advance1=dst->format->BytesPerPixel;
		unsigned pitch1=dst->pitch;

		pos1+=(h-1)*pitch1;

		for (ulong x=0;x<w;x++){
			uchar *pos00=pos0,
				*pos10=pos1;
			for (ulong y=0;y<h;y++){
				pos1[Roffset1]=pos0[Roffset0];
				pos1[Goffset1]=pos0[Goffset0];
				pos1[Boffset1]=pos0[Boffset0];
				if (alpha1){
					if (alpha0)
						pos1[Aoffset1]=pos0[Aoffset0];
					else
						pos1[Aoffset1]=0xFF;
				}

				pos0+=pitch0;
				pos1-=pitch1;
			}
			pos0=pos00+advance0;
			pos1=pos10+advance1;
		}

		SDL_UnlockSurface(src);
		SDL_UnlockSurface(dst);
	}
}

void FlipSurfaceHV(SDL_Surface *src,SDL_Surface *dst){
	if (!src || !dst || src->w!=dst->w || src->h!=dst->h)
		return;
	if (src==dst){
		SDL_LockSurface(src);

		ulong w=src->w,
			h=src->h,
			h0=h;
		h=h/2+h%2;

		uchar *pos=(uchar *)src->pixels;
		
		unsigned advance=src->format->BytesPerPixel;
		unsigned pitch=src->pitch;

		Uint32 putMask=(checkNativeEndianness()==NONS_LITTLE_ENDIAN)?((~0)<<(advance*8)):((~0)>>(advance*8)),
			getMask=~putMask;
		
		for (ulong y=0;y<h;y++){
			uchar *pos0=pos,
				*pos_reverse=pos+(w-1)*advance+(h-1)*pitch;
			if (y>h0/2)
				w/=2;
			for (ulong x=0;x<w;x++){
				Uint32 pixelA=*(Uint32 *)pos&getMask,
					pixelB=*(Uint32 *)pos_reverse&getMask;
				*(Uint32 *)pos&=putMask;
				*(Uint32 *)pos|=pixelB;
				*(Uint32 *)pos_reverse&=putMask;
				*(Uint32 *)pos_reverse|=pixelB;
				pos+=advance;
				pos_reverse-=advance;
			}
			pos=pos0+pitch;
		}

		SDL_UnlockSurface(src);
	}else{
		SDL_LockSurface(src);
		SDL_LockSurface(dst);

		ulong w=src->w,
			h=src->h;

		uchar *pos0=(uchar *)src->pixels;
		uchar *pos1=(uchar *)dst->pixels;

		uchar Roffset0=(src->format->Rshift)>>3;
		uchar Goffset0=(src->format->Gshift)>>3;
		uchar Boffset0=(src->format->Bshift)>>3;
		uchar Aoffset0=(src->format->Ashift)>>3;

		uchar Roffset1=(dst->format->Rshift)>>3;
		uchar Goffset1=(dst->format->Gshift)>>3;
		uchar Boffset1=(dst->format->Bshift)>>3;
		uchar Aoffset1=(dst->format->Ashift)>>3;

		bool alpha0=(Aoffset0!=Roffset0 && Aoffset0!=Goffset0 && Aoffset0!=Boffset0);
		bool alpha1=(Aoffset1!=Roffset1 && Aoffset1!=Goffset1 && Aoffset1!=Boffset1);

		unsigned advance0=src->format->BytesPerPixel;
		unsigned pitch0=src->pitch;
		unsigned advance1=dst->format->BytesPerPixel;
		unsigned pitch1=dst->pitch;

		pos1+=(w-1)*advance1+(h-1)*pitch1;

		for (ulong y=0;y<h;y++){
			uchar *pos00=pos0,
				*pos10=pos1;
			for (ulong x=0;x<w;x++){
				pos1[Roffset1]=pos0[Roffset0];
				pos1[Goffset1]=pos0[Goffset0];
				pos1[Boffset1]=pos0[Boffset0];
				if (alpha1){
					if (alpha0)
						pos1[Aoffset1]=pos0[Aoffset0];
					else
						pos1[Aoffset1]=0xFF;
				}

				pos0+=advance0;
				pos1-=advance1;
			}
			pos0=pos00+pitch0;
			pos1=pos10-pitch1;
		}

		SDL_UnlockSurface(src);
		SDL_UnlockSurface(dst);
	}
}

SDL_Surface *horizontalShear(SDL_Surface *src,float amount){
	if (!src || src->format->BitsPerPixel<24)
		return 0;
	if (!amount){
		SDL_Surface *res=SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,src->w,src->h,
			src->format->BitsPerPixel,
			src->format->Rmask,
			src->format->Gmask,
			src->format->Bmask,
			src->format->Amask
		);
		manualBlit(src,0,res,0);
		return res;
	}

	ulong w=src->w,
		h=src->h;

	//ulong resWidth=float(src->w)*(1.0f+ABS(amount));
	//ulong resWidth=float(src->w)*(1.0f+ABS(amount)*(float(src->h)/float(src->w)));
	ulong resWidth=src->w+ABS(amount)*float(src->h);
	SDL_Surface *res=SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,resWidth,src->h,
		src->format->BitsPerPixel,
		src->format->Rmask,
		src->format->Gmask,
		src->format->Bmask,
		src->format->Amask
	);

	SDL_LockSurface(src);
	SDL_LockSurface(res);

	uchar *pos0=(uchar *)src->pixels;
	uchar *pos1=(uchar *)res->pixels;

	ulong advance0=src->format->BytesPerPixel;
	ulong advance1=res->format->BytesPerPixel;
	ulong pitch0=src->pitch;
	ulong pitch1=res->pitch;

	ulong delta=ABS(amount)*0x10000;
	ulong X=0;
	ulong x=0;

	if (amount>0){
		for (ulong y=0;y<h;y++){
			if (w+x<=resWidth)
				memcpy(pos1,pos0,advance0*w);
			else
				memcpy(pos1,pos0,advance0*(resWidth-w-x));
			X+=delta;
			while (X>=0x10000){
				pos1+=advance1;
				X-=0x10000;
				x++;
			}
			pos0+=pitch0;
			pos1+=pitch1;
		}
	}else{
		pos0+=(h-1)*pitch0;
		pos1+=(h-1)*pitch1;
		for (ulong y=h-1;y<h;y--){
			if (w+x<=resWidth)
				memcpy(pos1,pos0,advance0*w);
			else
				memcpy(pos1,pos0,advance0*(resWidth-w-x));
			X+=delta;
			while (X>=0x10000){
				pos1+=advance1;
				X-=0x10000;
				x++;
			}
			pos0-=pitch0;
			pos1-=pitch1;
		}
	}

	SDL_UnlockSurface(src);
	SDL_UnlockSurface(res);

	return res;
}

SDL_Surface *verticalShear(SDL_Surface *src,float amount){
	if (!src || src->format->BitsPerPixel<24)
		return 0;
	if (!amount){
		SDL_Surface *res=SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,src->w,src->h,
			src->format->BitsPerPixel,
			src->format->Rmask,
			src->format->Gmask,
			src->format->Bmask,
			src->format->Amask
		);
		manualBlit(src,0,res,0);
		return res;
	}

	ulong w=src->w,
		h=src->h;

	//ulong resHeight=float(src->h)*(1.0f+ABS(amount)*(float(src->w)/float(src->h)));
	ulong resHeight=src->h+ABS(amount)*float(src->w);
	SDL_Surface *res=SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,src->w,resHeight,
		src->format->BitsPerPixel,
		src->format->Rmask,
		src->format->Gmask,
		src->format->Bmask,
		src->format->Amask
	);

	SDL_LockSurface(src);
	SDL_LockSurface(res);

	uchar *pos0=(uchar *)src->pixels;
	uchar *pos1=(uchar *)res->pixels;

	ulong advance0=src->format->BytesPerPixel;
	ulong advance1=res->format->BytesPerPixel;
	ulong pitch0=src->pitch;
	ulong pitch1=res->pitch;

	ulong delta=ABS(amount)*0x10000;
	ulong X=0;
	ulong y=0;

	if (amount>0){
		for (ulong x=0;x<w;x++){
			uchar *pos00=pos0,
				*pos10=pos1;
			for (ulong y2=y,a=0;a<h && y2<resHeight;y2++,a++){
				for (ulong b=0;b<advance0;b++)
					pos10[b]=pos00[b];
				pos00+=pitch0;
				pos10+=pitch1;
			}
			X+=delta;
			while (X>=0x10000){
				pos1+=pitch1;
				X-=0x10000;
				y++;
			}
			pos0+=advance0;
			pos1+=advance1;
		}
	}else{
		pos0+=(w-1)*advance0;
		pos1+=(w-1)*advance1;
		for (ulong x=w-1;x<w;x--){
			uchar *pos00=pos0,
				*pos10=pos1;
			for (ulong y2=y,a=0;a<h && y2<resHeight;y2++,a++){
				for (ulong b=0;b<advance0;b++)
					pos10[b]=pos00[b];
				pos00+=pitch0;
				pos10+=pitch1;
			}
			X+=delta;
			while (X>=0x10000){
				pos1+=pitch1;
				X-=0x10000;
				y++;
			}
			pos0-=advance0;
			pos1-=advance1;
		}
	}

	SDL_UnlockSurface(src);
	SDL_UnlockSurface(res);

	return res;
}
#endif // !defined(TOOLS_BARE_FILE) && !defined(TOOLS_NSAIO)

template <typename T,typename T2>
bool multicomparison_template(T character,const T2 *characters){
	for (;*characters;characters++)
		if (character==*characters)
			return 1;
	return 0;
}

bool multicomparison(char character,const char *characters){
	return multicomparison_template<char,char>(character,characters);
}

bool multicomparison(wchar_t character,const char *characters){
	return multicomparison_template<wchar_t,char>(character,characters);
}

bool multicomparison(char character,const wchar_t *characters){
	return multicomparison_template<char,wchar_t>(character,characters);
}

bool multicomparison(wchar_t character,const wchar_t *characters){
	return multicomparison_template<wchar_t,wchar_t>(character,characters);
}

Uint8 readByte(char *buffer,ulong &offset){
	return buffer[offset++];
}

Sint16 readSignedWord(char *buffer,ulong &offset){
	Sint16 r=0;
	for (char a=2;a>=0;a--){
		r<<=8;
		r|=(uchar)buffer[offset+a];
	}
	offset+=2;
	return r;
}

Uint16 readWord(char *buffer,ulong &offset){
	Uint16 r=0;
	for (char a=2;a>=0;a--){
		r<<=8;
		r|=(uchar)buffer[offset+a];
	}
	offset+=2;
	return r;
}

Sint32 readSignedDWord(char *buffer,ulong &offset){
	Sint32 r=0;
	for (char a=3;a>=0;a--){
		r<<=8;
		r|=(uchar)buffer[offset+a];
	}
	offset+=4;
	return r;
}

Uint32 readDWord(char *buffer,ulong &offset){
	Uint32 r=0;
	for (char a=3;a>=0;a--){
		r<<=8;
		r|=(uchar)buffer[offset+a];
	}
	offset+=4;
	return r;
}

std::string readString(char *buffer,ulong &offset){
	std::string r(buffer+offset);
	offset+=r.size()+1;
	return r;
}

void writeByte(Uint8 a,std::string &str,ulong offset){
	if (offset==ULONG_MAX)
		str.push_back(a&0xFF);
	else
		str[offset]=a&0xFF;
}

void writeWord(Uint16 a,std::string &str,ulong offset){
	ulong off=(offset==ULONG_MAX)?str.size():offset;
	for (ulong b=0;b<2;b++,off++){
		if (str.size()>off)
			str[off]=a&0xFF;
		else
			str.push_back(a&0xFF);
		a>>=8;
	}
}

void writeDWord(Uint32 a,std::string &str,ulong offset){
	ulong off=(offset==ULONG_MAX)?str.size():offset;
	for (ulong b=0;b<4;b++,off++){
		if (str.size()>off)
			str[off]=a&0xFF;
		else
			str.push_back(a&0xFF);
		a>>=8;
	}
}

void writeWordBig(Uint16 a,std::string &str,ulong offset){
	if (offset==ULONG_MAX)
		offset=str.size();
	for (ulong b=0;b<2;b++,offset++){
		if (str.size()>offset)
			str[offset]=a>>8;
		else
			str.push_back(a>>8);
		a<<=8;
	}
}

void writeDWordBig(Uint32 a,std::string &str,ulong offset){
	if (offset==ULONG_MAX)
		offset=str.size();
	for (ulong b=0;b<4;b++,offset++){
		if (str.size()>offset)
			str[offset]=a>>24;
		else
			str.push_back(a>>24);
		a<<=8;
	}
}

void writeString(const std::wstring &a,std::string &str){
	str.append(UniToUTF8(a));
	str.push_back(0);
}

#ifndef TOOLS_BARE_FILE
char *compressBuffer_BZ2(char *src,unsigned long srcl,unsigned long *dstl){
	unsigned long l=srcl,realsize=l;
	char *dst=new char[l];
	while (BZ2_bzBuffToBuffCompress(dst,(unsigned int *)&l,src,srcl,1,0,0)==BZ_OUTBUFF_FULL){
		delete[] dst;
		l*=2;
		realsize=l;
		dst=new char[l];
	}
	if (l!=realsize){
		char *temp=new char[l];
		memcpy(temp,dst,l);
		delete[] dst;
		dst=temp;
	}
	*dstl=l;
	return dst;
}

char *decompressBuffer_BZ2(char *src,unsigned long srcl,unsigned long *dstl){
	unsigned long l=srcl,realsize=l;
	char *dst=new char[l];
	while (BZ2_bzBuffToBuffDecompress(dst,(unsigned int *)&l,src,srcl,1,0)==BZ_OUTBUFF_FULL){
		delete[] dst;
		l*=2;
		realsize=l;
		dst=new char[l];
	}
	if (l!=realsize){
		char *temp=new char[l];
		memcpy(temp,dst,l);
		delete[] dst;
		dst=temp;
	}
	*dstl=l;
	return dst;
}
#endif

std::wstring readline(std::wstring::const_iterator start,std::wstring::const_iterator end,std::wstring::const_iterator *out){
	std::wstring::const_iterator end2=start;
	for (;end2!=end && *end2!=13 && *end2!=10;end2++);
	if (!!out){
		*out=end2;
		for (;*out!=end && (**out==13 || **out==10);(*out)++);
	}
	return std::wstring(start,end2);
}
#endif

ErrorCode inPlaceDecryption(char *buffer,ulong length,ulong mode){
	switch (mode){
		case NO_ENCRYPTION:
		default:
			return NONS_NO_ERROR;
		case XOR84_ENCRYPTION:
			for (ulong a=0;a<length;a++)
				buffer[a]^=0x84;
			return NONS_NO_ERROR;
		case VARIABLE_XOR_ENCRYPTION:
			{
				char magic_numbers[5]={0x79,0x57,0x0d,0x80,0x04};
				ulong index=0;
				for (ulong a=0;a<length;a++){
					buffer[a]^=magic_numbers[index];
					index=(index+1)%5;
				}
				return NONS_NO_ERROR;
			}
		case TRANSFORM_THEN_XOR84_ENCRYPTION:
			{
#if !defined(TOOLS_BARE_FILE) && !defined(TOOLS_NSAIO)
				o_stderr
#else
				std::cerr
#endif
				<<"TRANSFORM_THEN_XOR84 (aka mode 4) encryption not implemented for a very good\n"
					"reason. Which I, of course, don\'t need to explain to you. Good day.";
				return NONS_NOT_IMPLEMENTED;
			}
	}
	return NONS_NO_ERROR;
}

#ifdef NONS_SYS_WINDOWS
#ifdef NONS_SYS_WINDOWS
#include <windows.h>
extern HWND mainWindow;
#endif

BOOL CALLBACK findMainWindow_callback(HWND handle,LPARAM lparam){
	const std::wstring *caption=(const std::wstring *)lparam;
	std::wstring buffer=*caption;
	GetWindowText(handle,&buffer[0],buffer.size());
	if (buffer!=*caption)
		return 1;
	mainWindow=handle;
	return 0;
}

void findMainWindow(const wchar_t *caption){
	std::wstring string=caption;
	EnumWindows(findMainWindow_callback,(LPARAM)&string);
}
#endif
#endif