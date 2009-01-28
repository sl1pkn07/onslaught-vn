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

#include "Functions.h"
#ifndef BARE_FILE
#include <bzlib.h>
#include "Globals.h"
#endif

#include "UTF.h"
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

template<typename dst,typename src>
dst *copyString_template(const src *str,ulong len){
	if (!str)
		return 0;
	if (!len)
		for (;str[len];len++);
	dst *res=new dst[len+1];
	res[len]=0;
	for (ulong a=0;a<len;a++)
		res[a]=str[a];
	return res;
}

wchar_t *copyWString(const wchar_t *str,ulong len){
	return copyString_template<wchar_t,wchar_t>(str,len);
}

wchar_t *copyWString(const char *str,ulong len){
	return copyString_template<wchar_t,char>(str,len);
}

char *copyString(const wchar_t *str,ulong len){
	return copyString_template<char,wchar_t>(str,len);
}

char *copyString(const char *str,ulong len){
	return copyString_template<char,char>(str,len);
}

char *addStrings(const char *str1,const char *str2){
	ulong len1=strlen(str1),len2=strlen(str2);
	ulong len3=len1+len2;
	char *res=new char[len3+1];
	sprintf(res,"%s%s",str1,str2);
	return res;
}

wchar_t *addStrings(const wchar_t *str1,const wchar_t *str2){
	ulong len1=wcslen(str1),len2=wcslen(str2);
	ulong len3=len1+len2;
	wchar_t *res=new wchar_t[len3+1];
	wcscpy(res,str1);
	wcscat(res,str2);
	return res;
}

template <typename dst,typename src>
void addStringsInplace_template(dst **str1,const src *str2){
	ulong len1=0,len2=0;
	for (;(*str1)[len1];len1++);
	for (;str2[len2];len2++);
	ulong len3=len1+len2;
	dst *res=new dst[len3+1];
	res[len3]=0;
	for (ulong a=0;a<len1;a++)
		res[a]=(*str1)[a];
	for (ulong a=0;a<len2;a++)
		res[a+len1]=str2[a];
	delete[] *str1;
	*str1=res;
}

void addStringsInplace(wchar_t **str1,const wchar_t *str2){
	addStringsInplace_template<wchar_t,wchar_t>(str1,str2);
}

void addStringsInplace(wchar_t **str1,const char *str2){
	addStringsInplace_template<wchar_t,char>(str1,str2);
}

void addStringsInplace(char **str1,const wchar_t *str2){
	addStringsInplace_template<char,wchar_t>(str1,str2);
}

void addStringsInplace(char **str1,const char *str2){
	addStringsInplace_template<char,char>(str1,str2);
}

std::vector<char *> *getParameterList(const char *string,char delim){
	std::vector<char *> *res=new std::vector<char *>;
	ulong len=strlen(string);
	char tempDelim=delim;
	for (ulong start=0;start<len;){
		ulong end;
		if (string[start]=='\"')
			delim='\"';
		for (end=start+1;string[end] && string[end]!=delim;end++);
		if (delim=='\"')
			end++;
		ulong pl=end-start;
		/*char *el=new char[pl+1];
		el[pl]=0;
		for (ulong a=start;a<end;a++)
			el[a-start]=string[a];*/
		char *el=copyString(string+start,pl);
		res->push_back(el);
		delim=tempDelim;
		for (start=end+1;start<len && string[start]==delim;start++);
	}
	return res;
}

std::vector<wchar_t *> *getParameterList(const wchar_t *string,wchar_t delim){
	std::vector<wchar_t *> *res=new std::vector<wchar_t *>;
	ulong len=wcslen(string);
	wchar_t tempDelim=delim;
	for (ulong start=0;start<len;){
		ulong end;
		if (string[start]=='\"')
			delim='\"';
		for (end=start+1;string[end] && string[end]!=delim;end++);
		if (delim=='\"')
			end++;
		ulong pl=end-start;
		wchar_t *el=copyWString(string+start,pl);
		res->push_back(el);
		delim=tempDelim;
		for (start=end+1;start<len && string[start]==delim;start++);
	}
	return res;
}

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

template<typename T,typename T2>
long instr_template(const T *str0,const T2 *str1){
	for (long res=0;*str0;res++,str0++){
		const T *str2=str0;
		const T2 *str3=str1;
		for (;*str3;str2++,str3++){
			if (!*str2)
				return -1;
			if (*str2!=*str3)
				goto instr_000;
		}
		return res;
instr_000:;
	}
	return -1;
}

long instr(const wchar_t *str0,const wchar_t *str1){
	return instr_template<wchar_t,wchar_t>(str0,str1);
}

long instr(const wchar_t *str0,const char *str1){
	return instr_template<wchar_t,char>(str0,str1);
}

long instr(const char *str0,const wchar_t *str1){
	return instr_template<char,wchar_t>(str0,str1);
}

long instr(const char *str0,const char *str1){
	return instr_template<char,char>(str0,str1);
}

template<typename T>
long instrB_template(const T *str0,const T *str1){
	long len0,len1;
	for (len0=0;str0[len0];len0++);
	for (len1=0;str1[len1];len1++);
	str0+=len0-1;
	for (long res=len0-1;res>=0;res--,str0--){
		const T *str2=str0;
		const T *str3=str1;
		for (;*str3;str2++,str3++){
			if (!*str2)
				return -1;
			if (*str2!=*str3)
				goto instrB_000;
		}
		return res;
instrB_000:;
	}
	return -1;
}

long instrB(const wchar_t *str0,const wchar_t *str1){
	return instrB_template<wchar_t>(str0,str1);
}

long instrB(const char *str0,const char *str1){
	return instrB_template<char>(str0,str1);
}

template<typename T>
void tolower_template(T *param){
    for (;*param;param++)
		if (*param>='A' && *param<='Z')
			*param=*param|32;
}

void tolower(wchar_t *param){
	return tolower_template<wchar_t>(param);
}

void tolower(char *param){
	return tolower_template<char>(param);
}

integer32 secondsSince1900(){
	return time(0)+2208986640;
}

#ifndef BARE_FILE
#ifndef NONS_PARALLELIZE
void manualBlit(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,uchar alpha){
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
	int w0=srcRect0.w, h0=srcRect0.h;

	SDL_LockSurface(src);
	SDL_LockSurface(dst);

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
	//Unused:
	//bool condition=0;

	for (int y0=0;y0<h0;y0++){
		uchar *pos00=pos0;
		uchar *pos10=pos1;
		for (int x0=0;x0<w0;x0++){
			uchar r0=pos0[Roffset0];
			uchar g0=pos0[Goffset0];
			uchar b0=pos0[Boffset0];
			//uchar a0=pos0[Aoffset0];

			uchar *r1=pos1+Roffset1;
			uchar *g1=pos1+Goffset1;
			uchar *b1=pos1+Boffset1;

			if (alpha0){
				uchar a0=uchar((short(pos0[Aoffset0])*short(alpha))/255);
				/*short deltar=r0-*r1;
				short deltag=g0-*g1;
				short deltab=b0-*b1;
				(*r1)+=(deltar*a0)/255;
				(*g1)+=(deltag*a0)/255;
				(*b1)+=(deltab*a0)/255;*/
				*r1=((255-a0)*(*r1))/255+(a0*r0)/255;
				*g1=((255-a0)*(*g1))/255+(a0*g0)/255;
				*b1=((255-a0)*(*b1))/255+(a0*b0)/255;
				if (alpha1){
					uchar *a1=pos1+Aoffset1;
					short temp=*a1+a0;
					*a1=temp>255?255:temp;
				}
			}else if (alpha<255){
				uchar a0=255-alpha;
				/**r1=(a0*r0)/255+(alpha*(*r1))/255;
				*g1=(a0*g0)/255+(alpha*(*g1))/255;
				*b1=(a0*b0)/255+(alpha*(*b1))/255;*/
				*r1=(a0*(*r1))/255+(alpha*r0)/255;
				*g1=(a0*(*g1))/255+(alpha*g0)/255;
				*b1=(a0*(*b1))/255+(alpha*b0)/255;
				/*short deltar=r0-*r1;
				short deltag=g0-*g1;
				short deltab=b0-*b1;
				(*r1)+=(deltar*a0)/255;
				(*g1)+=(deltag*a0)/255;
				(*b1)+=(deltab*a0)/255;*/
				if (alpha1){
					uchar *a1=pos1+Aoffset1;
					short temp=*a1+a0;
					*a1=temp>255?255:temp;
				}
			}else{
				*r1=r0;
				*g1=g0;
				*b1=b0;
				if (alpha1){
					uchar *a1=pos1+Aoffset1;
					*a1=0xFF;
				}
			}
			pos0+=advance0;
			pos1+=advance1;
		}
		pos0=pos00+src->pitch;
		pos1=pos10+dst->pitch;
	}
	SDL_UnlockSurface(dst);
	SDL_UnlockSurface(src);
}

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
	int w0=srcRect0.w, h0=srcRect0.h;

	SDL_LockSurface(src);
	SDL_LockSurface(dst);

	uchar *pos0=(uchar *)src->pixels;
	uchar *pos1=(uchar *)dst->pixels;

	uchar Roffset0=(src->format->Rshift)>>3;
	uchar Goffset0=(src->format->Gshift)>>3;
	uchar Boffset0=(src->format->Bshift)>>3;
	//Unused:
	//uchar Aoffset0=(src->format->Ashift)>>3;

	uchar Roffset1=(dst->format->Rshift)>>3;
	uchar Goffset1=(dst->format->Gshift)>>3;
	uchar Boffset1=(dst->format->Bshift)>>3;
	//Unused:
	//uchar Aoffset1=(dst->format->Ashift)>>3;

	unsigned advance0=src->format->BytesPerPixel;
	unsigned advance1=dst->format->BytesPerPixel;

	pos0+=src->pitch*srcRect0.y+srcRect0.x*advance0;
	pos1+=dst->pitch*dstRect0.y+dstRect0.x*advance1;

	//Unused:
	//bool alpha0=(Aoffset0!=Roffset0 && Aoffset0!=Goffset0 && Aoffset0!=Boffset0);
	//bool alpha1=(Aoffset1!=Roffset1 && Aoffset1!=Goffset1 && Aoffset1!=Boffset1);
	//bool condition=0;

	for (int y0=0;y0<h0;y0++){
		uchar *pos00=pos0;
		uchar *pos10=pos1;
		for (int x0=0;x0<w0;x0++){
			uchar r0=pos0[Roffset0];
			uchar g0=pos0[Goffset0];
			uchar b0=pos0[Boffset0];
			//uchar a0=pos0[Aoffset0];

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
	SDL_UnlockSurface(dst);
	SDL_UnlockSurface(src);
}
#else
void manualBlit_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,uchar alpha);
int manualBlit_threaded(void *parameters);

void manualBlit(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,uchar alpha){
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
	if (cpu_count==1 || srcRect0.w*srcRect0.h<1000){
		manualBlit_threaded(src,&srcRect0,dst,&dstRect0,alpha);
		SDL_UnlockSurface(dst);
		SDL_UnlockSurface(src);
		return;
	}
	SDL_Thread **threads=new SDL_Thread *[cpu_count];
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
	for (ushort a=0;a<cpu_count;a++)
		threads[a]=SDL_CreateThread(manualBlit_threaded,parameters+a);
	for (ushort a=0;a<cpu_count;a++)
		SDL_WaitThread(threads[a],0);
	//ulong t1=SDL_GetTicks();
	//v_stdlog <<"Done in "<<t1-t0<<" ms."<<std::endl;
	SDL_UnlockSurface(dst);
	SDL_UnlockSurface(src);
	delete[] threads;
	delete[] rects0;
	delete[] rects1;
	delete[] parameters;
}

int manualBlit_threaded(void *parameters){
	PSF_parameters *p=(PSF_parameters *)parameters;
	manualBlit_threaded(p->src,p->srcRect,p->dst,p->dstRect,p->alpha);
	return 0;
}

void manualBlit_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,uchar alpha){
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

			if (alpha0){
				uchar a0=uchar((short(pos0[Aoffset0])*short(alpha))/255);
				*r1=((255-a0)*(*r1))/255+(a0*r0)/255;
				*g1=((255-a0)*(*g1))/255+(a0*g0)/255;
				*b1=((255-a0)*(*b1))/255+(a0*b0)/255;
				if (alpha1){
					uchar *a1=pos1+Aoffset1;
					short temp=*a1+a0;
					*a1=temp>255?255:temp;
				}
			}else if (alpha<255){
				uchar a0=255-alpha;
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
				if (alpha1){
					uchar *a1=pos1+Aoffset1;
					*a1=0xFF;
				}
			}
			pos0+=advance0;
			pos1+=advance1;
		}
		pos0=pos00+src->pitch;
		pos1=pos10+dst->pitch;
	}
}

void multiplyBlend_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect);
int multiplyBlend_threaded(void *parameters);

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
	SDL_Thread **threads=new SDL_Thread *[cpu_count];
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
	for (ushort a=0;a<cpu_count;a++)
		threads[a]=SDL_CreateThread(multiplyBlend_threaded,parameters+a);
	for (ushort a=0;a<cpu_count;a++)
		SDL_WaitThread(threads[a],0);
	SDL_UnlockSurface(dst);
	SDL_UnlockSurface(src);
	delete[] threads;
	delete[] rects0;
	delete[] rects1;
	delete[] parameters;
}

int multiplyBlend_threaded(void *parameters){
	PSF_parameters *p=(PSF_parameters *)parameters;
	multiplyBlend_threaded(p->src,p->srcRect,p->dst,p->dstRect);
	return 0;
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
#endif
#endif

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

template<typename T>
void toforwardslash_template(T *param){
	for (;*param;param++)
		if (*param=='\\')
			*param='/';
}

void toforwardslash(wchar_t *param){
	toforwardslash_template(param);
}

void toforwardslash(char *param){
	toforwardslash_template(param);
}

bool isanumber(char *a){
	for (;*a;a++)
		if (!isdigit(*a))
			return 0;
	return 1;
}

bool isanumber(wchar_t *a){
	for (;*a;a++)
		if (!isdigit(*a))
			return 0;
	return 1;
}

integer8 readByte(char *buffer,long *offset){
	integer8 r=buffer[*offset];
	(*offset)++;
	return r;
}

Sinteger16 readSignedWord(char *buffer,long *offset){
	Sinteger16 r=0;
	for (char a=2;a>=0;a--){
		r<<=8;
		r|=(uchar)(buffer[*offset+a]);
	}
	(*offset)+=2;
	return r;
}

integer16 readWord(char *buffer,long *offset){
	integer16 r=0;
	for (char a=2;a>=0;a--){
		r<<=8;
		r|=(uchar)buffer[*offset+a];
	}
	(*offset)+=2;
	return r;
}

Sinteger32 readSignedDWord(char *buffer,long *offset){
	Sinteger32 r=0;
	for (char a=3;a>=0;a--){
		r<<=8;
		r|=(uchar)(buffer[*offset+a]);
	}
	(*offset)+=4;
	return r;
}

integer32 readDWord(char *buffer,long *offset){
	integer32 r=0;
	for (char a=3;a>=0;a--){
		r<<=8;
		r|=(uchar)buffer[*offset+a];
	}
	(*offset)+=4;
	return r;
}

char *readString(char *buffer,long *offset){
	char *r=copyString(buffer+*offset);
	(*offset)+=strlen(r)+1;
	return r;
}

void writeByte(integer8 a,std::string *str,long offset){
	if (offset<0)
		str->push_back(a&0xFF);
	else
		(*str)[offset]=a&0xFF;
}

void writeWord(integer16 a,std::string *str,long offset){
	if (offset<0)
		offset=str->size();
	for (char b=0;b<2;b++,offset++){
		if (str->size()>offset)
			(*str)[offset]=a&0xFF;
		else
			str->push_back(a&0xFF);
		a>>=8;
	}
}

void writeDWord(integer32 a,std::string *str,long offset){
	if (offset<0)
		offset=str->size();
	for (char b=0;b<4;b++,offset++){
		if (str->size()>offset)
			(*str)[offset]=a&0xFF;
		else
			str->push_back(a&0xFF);
		a>>=8;
	}
}

void writeString(wchar_t *a,std::string *str){
	char *utf8=WChar_to_UTF8(a);
	str->append(utf8);
	str->push_back(0);
	delete[] utf8;
}

#ifndef BARE_FILE
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

template <typename T>
T *tagName_template(const T *string){
	if (!string || *string!='<')
		return 0;
	for (string++;*string && iswhitespace(*string);string++);
	if (multicomparison(*string,"\0>="))
		return 0;
	ulong l=0;
	for (;string[l] && !iswhitespace(string[l]) && string[l]!='=';l++);
	if (!string[l])
		return 0;
	return copyString_template<T>(string,l);
}

wchar_t *tagName(const wchar_t *string){
	return tagName_template<wchar_t>(string);
}

char *tagName(const char *string){
	return tagName_template<char>(string);
}

template <typename T>
T *tagValue_template(const T *string){
	if (!string || *string!='<')
		return 0;
	for (string++;;string++){
		if (!*string || *string=='>')
			return 0;
		if (*string=='=')
			break;
	}
	for (string++;*string && iswhitespace(*string);string++)
	if (!*string || *string=='>')
		return 0;
	ulong l=0;
	for (;string[l] && !iswhitespace(string[l]) && string[l]!='>';l++);
	if (!string[l])
		return 0;
	return copyString_template<T>(string,l);
}

wchar_t *tagValue(const wchar_t *string){
	return tagValue_template<wchar_t>(string);
}

char *tagValue(const char *string){
	return tagValue_template<char>(string);
}
#endif
