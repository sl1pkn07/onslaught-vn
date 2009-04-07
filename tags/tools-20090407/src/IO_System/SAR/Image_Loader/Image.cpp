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

#ifndef NONS_IMAGE_CPP
#define NONS_IMAGE_CPP

#include "Image.h"
#include "../../../Globals.h"
#include "../../../Functions.h"

NONS_Image::NONS_Image(){
	this->age=0;
	this->image=0;
	this->name=0;
	this->refCount=0;
	this->string=0;
}

NONS_Image::~NONS_Image(){
	if (this->image){
		if (!this->refCount)
			SDL_FreeSurface(this->image);
		if (this->name)
			delete[] this->name;
		if (this->string)
			delete[] this->string;
	}
}

/*
Note: For the classic method, only the red component is used. Not only this is
useful for creating human-understandable images, but it also greatly
simplifies the code.
*/
SDL_Surface *NONS_Image::LoadLayerImage(const wchar_t *name,uchar *buffer,ulong bufferSize,SDL_Rect *screen,int method=CLASSIC_METHOD){
	if (!name || !buffer || !bufferSize || !screen || this->image && this->refCount)
		return 0;
	if (this->image)
		SDL_FreeSurface(this->image);
	SDL_RWops *rwops=SDL_RWFromMem((void *)buffer,bufferSize);
	SDL_Surface *surface=IMG_Load_RW(rwops,1);
	switch (method){
		/*case NO_ALPHA:
			{
				int width=surface->clip_rect.w, height=surface->clip_rect.h;
				int screenWidth=screen->screen->clip_rect.w, screenHeight=screen->screen->clip_rect.h;
				int xoffset=(width<screenWidth)?0:(screenWidth-width)/2,
					yoffset=(height<screenHeight)?0:(screenHeight-height)/2;
				this->image=new NONS_Layer(&(screen->screen->clip_rect),0);
				SDL_Rect rect={xoffset,yoffset,0,0};
				SDL_BlitSurface(surface,0,this->image->data,&rect);
			}
			break;*/
		case CLASSIC_METHOD:
			{
				int originalWidth=surface->clip_rect.w;
				int width=originalWidth/2, height=surface->clip_rect.h;
				int screenWidth=screen->w, screenHeight=screen->h;
				SDL_Rect srcrect={
					width<screenWidth?0:(width-screenWidth)/2,
					height<screenHeight?0:(height-screenHeight)/2,
					0,0
				};
				srcrect.w=width<screenWidth?width:screenWidth;
				srcrect.h=height<screenHeight?height:screenHeight;
				SDL_Rect dstrect={
					(width>=screenWidth)?0:(screenWidth-width)/2,
					(height>=screenHeight)?0:screenHeight-height,
					0,0
				};
				SDL_Rect alpharect=srcrect;
				alpharect.x+=width;
				//alpharect.y+=height;
				this->image=SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,screenWidth,screenHeight,32,rmask,gmask,bmask,amask);
				SDL_BlitSurface(surface,&srcrect,this->image,&dstrect);
				SDL_Surface *src=surface;
				SDL_Surface *dst=this->image;
				uchar Roffset0=(src->format->Rshift)>>3;
				uchar Aoffset1=(dst->format->Ashift)>>3;
				unsigned advance0=src->format->BytesPerPixel;
				unsigned advance1=dst->format->BytesPerPixel;
				uchar *pos0=(uchar *)src->pixels+alpharect.x*advance0;
				uchar *pos1=(uchar *)dst->pixels+dstrect.x*advance1+dstrect.y*dst->pitch;
				width=dstrect.w;
				height=dstrect.h;
				for (int y=0;y<height;y++){
					uchar *temp0=pos0;
					uchar *temp1=pos1;
					for (int x=0;x<width;x++){
						pos1[Aoffset1]=255-pos0[Roffset0];
						pos0+=advance0;
						pos1+=advance1;
					}
					pos0=temp0+src->pitch;
					pos1=temp1+dst->pitch;
				}
			}
			break;
		case NO_ALPHA:
		case ALPHA_METHOD:
			{
				int width=surface->clip_rect.w, height=surface->clip_rect.h;
				int screenWidth=screen->w, screenHeight=screen->h;
				int xoffset=(width<screenWidth)?0:(screenWidth-width)/2,
					yoffset=(height<screenHeight)?0:screenHeight-height;
				this->image=SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,screenWidth,screenHeight,32,rmask,gmask,bmask,amask);
				SDL_Rect rect={xoffset,yoffset,0,0};
				SDL_BlitSurface(surface,0,this->image,&rect);
			}
			break;
		default:
			name=0;
			break;
	}
	SDL_FreeSurface(surface);
	//SDL_FreeRW(rwops);
	this->name=name?copyWString(name):0;
	tolower(this->name);
	toforwardslash(this->name);
	this->refCount=0;
	return this->image;
}

/*
This function is probably very expensive, but since it's not inside a
time-critical loop and the caller function will probably execute just a few
times at the beginning of the program, it's okay.
*/
bool isChroma(uchar r,uchar g,uchar b){
	uchar max=r>=g && r>=b?r:(g>=r && g>=b?g:b);
	uchar min=r<=g && r<=b?r:(g<=r && g<=b?g:b);
	float ret;
	if (max==min)
		ret=0;
	else if (max==r){
		ret=60*(float(g-b)/float(max-min));
	}else if (max==g)
		ret=60*(float(b-r)/float(max-min))+120;
	else
		ret=60*(float(r-g)/float(max-min))+240;
	while (ret<0)
		ret+=360;
	while (ret>=360)
		ret-=360;
	return (ret==300.0);
}

/*
Note: For the classic method, the chroma hue is 300° (magenta hue).
*/
SDL_Surface *NONS_Image::LoadCursorImage(uchar *buffer,ulong bufferSize,int method){
	if (!buffer || !bufferSize)
		return 0;
	SDL_RWops *rwops=SDL_RWFromMem((void *)buffer,bufferSize);
	SDL_Surface *surface=IMG_Load_RW(rwops,0);
	if (!surface){
		SDL_FreeRW(rwops);
		return 0;
	}
	switch (method){
		case CLASSIC_METHOD:
			{
				int width=surface->clip_rect.w, height=surface->clip_rect.h;
				this->image=SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,width,height,32,rmask,gmask,bmask,amask);
				SDL_BlitSurface(surface,0,this->image,0);
				SDL_Surface *src=this->image;
				SDL_Surface *dst=this->image;
				uchar Roffset0=(src->format->Rshift)>>3;
				uchar Goffset0=(src->format->Gshift)>>3;
				uchar Boffset0=(src->format->Bshift)>>3;
				uchar Roffset1=(dst->format->Rshift)>>3;
				uchar Goffset1=(dst->format->Gshift)>>3;
				uchar Boffset1=(dst->format->Bshift)>>3;
				uchar Aoffset1=(dst->format->Ashift)>>3;
				unsigned advance0=src->format->BytesPerPixel;
				unsigned advance1=dst->format->BytesPerPixel;
				uchar *pos0=(uchar *)src->pixels;
				uchar *pos1=(uchar *)dst->pixels;
				for (int y=0;y<height;y++){
					for (int x=0;x<width;x++){
						uchar r=pos0[Roffset0];
						uchar g=pos0[Goffset0];
						uchar b=pos0[Boffset0];
						if (isChroma(r,g,b))
							pos1[Aoffset1]=0;
						else{
							pos1[Roffset1]=r;
							pos1[Goffset1]=g;
							pos1[Boffset1]=b;
							pos1[Aoffset1]=0xFF;
						}
						pos0+=advance0;
						pos1+=advance1;
					}
				}
			}
			break;
		case NO_ALPHA:
		case ALPHA_METHOD:
			{
				int width=surface->clip_rect.w, height=surface->clip_rect.h;
				this->image=SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,width,height,32,rmask,gmask,bmask,amask);
				SDL_BlitSurface(surface,0,this->image,0);
			}
			break;
		default:
			SDL_FreeSurface(surface);
			return 0;
	}
	SDL_FreeSurface(surface);
	SDL_FreeRW(rwops);
	this->name=copyWString(name);
	this->refCount=0;
	return this->image;
}

SDL_Surface *NONS_Image::LoadSpriteImage(const wchar_t *string,const wchar_t *name,uchar *buffer,ulong bufferSize,int method){
	if (!buffer || !bufferSize)
		return 0;
	SDL_RWops *rwops=SDL_RWFromMem((void *)buffer,bufferSize);
	SDL_Surface *surface;
	surface=IMG_Load_RW(rwops,0);

	switch (method){
		case CLASSIC_METHOD:
			{
				int originalWidth=surface->clip_rect.w;
				int width=originalWidth/2, height=surface->clip_rect.h;
				this->image=SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,width,height,32,rmask,gmask,bmask,amask);
				SDL_Rect srcrect={0,0,width,height};
				SDL_BlitSurface(surface,&srcrect,this->image,&srcrect);
				SDL_Surface *src=surface;
				SDL_Surface *dst=this->image;
				uchar Roffset0=(src->format->Rshift)>>3;
				uchar Aoffset1=(dst->format->Ashift)>>3;
				unsigned advance0=src->format->BytesPerPixel;
				unsigned advance1=dst->format->BytesPerPixel;
				uchar *pos0=(uchar *)src->pixels+width*advance0;
				uchar *pos1=(uchar *)dst->pixels;
				for (int y=0;y<height;y++){
					//pos0+=width*advance0;
					uchar *temp0=pos0;
					uchar *temp1=pos1;
					//uchar *temp1=pos1;
					for (int x=0;x<width;x++){
						pos1[Aoffset1]=255-pos0[Roffset0];
						pos0+=advance0;
						pos1+=advance1;
					}
					pos0=temp0+src->pitch;
					pos1=temp1+dst->pitch;
					//pos1=temp1+dst->pitch;
				}
			}
			break;
		case NO_ALPHA:
		case ALPHA_METHOD:
			{
				int width=surface->clip_rect.w, height=surface->clip_rect.h;
				this->image=SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,width,height,32,rmask,gmask,bmask,amask);
				SDL_BlitSurface(surface,0,this->image,0);
			}
			break;
		default:
			SDL_FreeSurface(surface);
			return 0;
	}
	SDL_FreeSurface(surface);
	SDL_FreeRW(rwops);
	this->name=copyWString(name);
	toforwardslash(this->name);
	tolower(this->name);
	this->string=copyWString(string);
	toforwardslash(this->string);
	tolower(this->string);
	this->refCount=0;
	return this->image;
}
#endif
