/*
* Copyright (c) 2008-2010, Helios (helios.vmg@gmail.com)
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


//(Parallelized surface function)
struct PSF_parameters{
	SDL_Surface *src;
	SDL_Rect *srcRect;
	SDL_Surface *dst;
	SDL_Rect *dstRect;
	manualBlitAlpha_t alpha;
};

void manualBlit_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,manualBlitAlpha_t alpha);
void manualBlit_threaded(void *parameters);

NONS_DLLexport void manualBlit(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,manualBlitAlpha_t alpha){
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
	NONS_Thread *threads=new NONS_Thread[cpu_count];
#endif
	SDL_Rect *rects0=new SDL_Rect[cpu_count];
	SDL_Rect *rects1=new SDL_Rect[cpu_count];
	PSF_parameters *parameters=new PSF_parameters[cpu_count];
	ulong division=ulong(float(srcRect0.h)/float(cpu_count));
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
	rects0[cpu_count-1].h+=Uint16(srcRect0.h-total);
	//ulong t0=SDL_GetTicks();
	for (ushort a=1;a<cpu_count;a++)
#ifndef USE_THREAD_MANAGER
		threads[a].call(manualBlit_threaded,parameters+a);
#else
		threadManager.call(a-1,manualBlit_threaded,parameters+a);
#endif
	manualBlit_threaded(parameters);
#ifndef USE_THREAD_MANAGER
	for (ushort a=1;a<cpu_count;a++)
		threads[a].join();
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

void manualBlit_threaded(void *parameters){
	PSF_parameters *p=(PSF_parameters *)parameters;
	manualBlit_threaded(p->src,p->srcRect,p->dst,p->dstRect,p->alpha);
}

void manualBlit_threaded(SDL_Surface *src,SDL_Rect *srcRect,SDL_Surface *dst,SDL_Rect *dstRect,manualBlitAlpha_t alpha){
	SDL_Rect &srcRect0=*srcRect,
		&dstRect0=*dstRect;
	int w0=srcRect0.w, h0=srcRect0.h;
	if (srcRect0.w<=0 || srcRect0.h<=0)
		return;

	surfaceData sd[]={
		src,
		dst
	};
	sd[0].pixels+=sd[0].pitch*srcRect0.y+sd[0].advance*srcRect0.x;
	sd[1].pixels+=sd[1].pitch*dstRect0.y+sd[1].advance*dstRect0.x;

	bool negate=(alpha<0);
	if (negate)
		alpha=-alpha;
#define manualBlit_threaded_DO_ALPHA(variable_code){\
	for (int y0=0;y0<h0;y0++){						\
		uchar *pos[]={								\
			sd[0].pixels,							\
			sd[1].pixels							\
		};											\
		for (int x0=0;x0<w0;x0++){					\
			long rgba0[4];							\
			uchar *rgba1[4];						\
			for (int a=0;a<4;a++){					\
				rgba0[a]=pos[0][sd[0].offsets[a]];	\
				rgba1[a]=pos[1]+sd[1].offsets[a];	\
			}										\
													\
			{variable_code}							\
			/*avoid unnecessary jumps*/				\
			if (!(negate && rgba0[3])){				\
				pos[0]+=sd[0].advance;				\
				pos[1]+=sd[1].advance;				\
				continue;							\
			}										\
			for (int a=0;a<3;a++)					\
				*rgba1[a]=~*rgba1[a];				\
		}											\
		sd[0].pixels+=sd[0].pitch;					\
		sd[1].pixels+=sd[1].pitch;					\
	}												\
}
#define manualBlit_threaded_SINGLE_ALPHA_SOURCE(alpha_source)	\
	ulong as=(alpha_source);									\
	if (as){													\
		*rgba1[0]=(uchar)APPLY_ALPHA(rgba0[0],*rgba1[0],as);	\
		*rgba1[1]=(uchar)APPLY_ALPHA(rgba0[1],*rgba1[1],as);	\
		*rgba1[2]=(uchar)APPLY_ALPHA(rgba0[2],*rgba1[2],as);	\
	}
#define manualBlit_threaded_DOUBLE_ALPHA_SOURCE(alpha_source)			\
	ulong as=(alpha_source);											\
	ulong bottom_alpha=													\
	*rgba1[3]=~(uchar)INTEGER_MULTIPLICATION(as^0xFF,*rgba1[3]^0xFF);	\
	ulong composite=integer_division_lookup[as+bottom_alpha*256];		\
	if (composite){														\
		*rgba1[0]=(uchar)APPLY_ALPHA(rgba0[0],*rgba1[0],composite);		\
		*rgba1[1]=(uchar)APPLY_ALPHA(rgba0[1],*rgba1[1],composite);		\
		*rgba1[2]=(uchar)APPLY_ALPHA(rgba0[2],*rgba1[2],composite);		\
	}
	if (alpha==255){
		if (!sd[0].alpha){
			manualBlit_threaded_DO_ALPHA(
				*rgba1[0]=(uchar)rgba0[0];
				*rgba1[1]=(uchar)rgba0[1];
				*rgba1[2]=(uchar)rgba0[2];
				if (sd[1].alpha)
					*rgba1[3]=0xFF;
			)
		}else{
			if (!sd[1].alpha){
				manualBlit_threaded_DO_ALPHA(
					manualBlit_threaded_SINGLE_ALPHA_SOURCE(rgba0[3]);
				)
			}else{
				manualBlit_threaded_DO_ALPHA(
					manualBlit_threaded_DOUBLE_ALPHA_SOURCE(rgba0[3]);
				)
			}
		}
	}else{
		if (!sd[0].alpha){
			if (!sd[1].alpha){
				manualBlit_threaded_DO_ALPHA(
					manualBlit_threaded_SINGLE_ALPHA_SOURCE(alpha);
				)
			}else{
				manualBlit_threaded_DO_ALPHA(
					manualBlit_threaded_DOUBLE_ALPHA_SOURCE(alpha);
				)
			}
		}else{
			if (!sd[1].alpha){
				manualBlit_threaded_DO_ALPHA(
					rgba0[3]=INTEGER_MULTIPLICATION(rgba0[3],alpha);
					manualBlit_threaded_SINGLE_ALPHA_SOURCE(rgba0[3]);
				)
			}else{
				manualBlit_threaded_DO_ALPHA(
					rgba0[3]=INTEGER_MULTIPLICATION(rgba0[3],alpha);
					manualBlit_threaded_DOUBLE_ALPHA_SOURCE(rgba0[3]);
				)
			}
		}
	}
}
