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

#ifndef NONS_STANDARDOUTPUT_CPP
#define NONS_STANDARDOUTPUT_CPP

#include "StandardOutput.h"
#include "../../Functions.h"
#include "../../UTF.h"
#include "../../Globals.h"
#include <cstdlib>

extern std::ofstream textDumpFile;

NONS_StandardOutput::NONS_StandardOutput(NONS_Layer *fgLayer,NONS_Layer *shadowLayer,NONS_Layer *shadeLayer){
	this->foregroundLayer=fgLayer;
	this->shadowLayer=shadowLayer;
	this->shadeLayer=shadeLayer;
	this->x=0;
	this->y=0;
	this->x0=0;
	this->y0=0;
	this->w=fgLayer->data->clip_rect.w;
	this->h=fgLayer->data->clip_rect.h;
	this->display_speed=0;
	this->extraAdvance=0;
	this->visible=0;
	this->transition=new NONS_GFX(1,0,0);
	this->log.reserve(50);
	this->horizontalCenterPolicy=0;
	this->verticalCenterPolicy=0;
	this->lastStart=-1;
}

NONS_StandardOutput::NONS_StandardOutput(NONS_Font *font,SDL_Rect *size,SDL_Rect *frame,bool shadow){
	SDL_Color rgb={255,255,255,0};
	SDL_Color rgb2={0,0,0,0};
	this->foregroundLayer=new NONS_Layer(size,0x00000000);
	this->foregroundLayer->MakeTextLayer(font,&rgb,0);
	if (shadow){
		this->shadowLayer=new NONS_Layer(size,0x00000000);
		this->shadowLayer->MakeTextLayer(font,&rgb2,1);
	}else
		this->shadowLayer=0;
	this->shadeLayer=new NONS_Layer(size,(0x99<<rshift)|(0x99<<gshift)|(0x99<<bshift));
	this->x=frame->x;
	this->y=frame->y;
	this->x0=frame->x;
	this->y0=frame->y;
	this->w=frame->w;
	this->h=frame->h;
	this->display_speed=0;
	this->extraAdvance=0;
	this->visible=0;
	this->transition=new NONS_GFX(1,0,0);
	this->log.reserve(50);
	this->horizontalCenterPolicy=0;
	this->verticalCenterPolicy=0;
	this->lastStart=-1;
}

NONS_StandardOutput::~NONS_StandardOutput(){
	this->Clear();
	delete this->foregroundLayer;
	if (this->shadowLayer)
		delete this->shadowLayer;
	delete this->shadeLayer;
	if (!this->transition->stored)
		delete this->transition;
}

bool NONS_StandardOutput::prepareForPrinting(const std::wstring str){
	long lastSpace=-1;
	int x0=this->x,y0=this->y;
	int wordL=0;
	int lineSkip=this->foregroundLayer->fontCache->font->lineSkip;
	this->resumePrinting=0;
	for (std::wstring::const_iterator i=str.begin(),end=str.end();i!=end;i++){
		wchar_t character=*i;
		NONS_Glyph *glyph=this->foregroundLayer->fontCache->getGlyph(character);
		if (character=='\n'){
			this->cachedText.push_back(character);
			if (x0+wordL>=this->w+this->x0 && lastSpace>=0){
				this->cachedText[lastSpace]='\n';
				y0+=lineSkip;
			}
			lastSpace=-1;
			x0=this->x0;
			y0+=lineSkip;
			wordL=0;
		}else if (isbreakspace(character)){
			if (x0+wordL>this->w+this->x0 && lastSpace>=0){
				this->cachedText[lastSpace]='\n';
				lastSpace=-1;
				x0=this->x0;
				y0+=lineSkip;
			}
			x0+=wordL;
			lastSpace=this->cachedText.size();
			wordL=glyph->getadvance()+this->extraAdvance;
			this->cachedText.push_back(character);
		}else if (character){
			wordL+=glyph->getadvance()+this->extraAdvance;
			this->cachedText.push_back(character);
		}else{
			if (x0+wordL>=this->w+this->x0 && lastSpace>=0)
				this->cachedText[lastSpace]=0;
			break;
		}
	}
	this->printingStarted=1;
	if (this->verticalCenterPolicy>0 && this->currentBuffer.size()>0)
		return 1;
	SDL_Rect frame={this->x0,this->y0,this->w,this->h};
	if (this->verticalCenterPolicy)
		this->y=this->setTextStart(&this->cachedText,&frame,this->verticalCenterPolicy);
	else if (!this->currentBuffer.size())
		this->y=this->y0;
	this->prebufferedText.append(L"<y=");
	this->prebufferedText.append(itoa<wchar_t>(this->y));
	this->prebufferedText.push_back('>');
	return 0;
}

bool NONS_StandardOutput::print(ulong start,ulong end,NONS_VirtualScreen *dst,ulong *printedChars){
	if (start>=this->cachedText.size())
		return 0;
	NONS_EventQueue *queue=InputObserver.attach();
	bool enterPressed=0;
	int x0,y0=this->y;
	SDL_Rect frame={this->x0,this->y0,this->w,this->h};
	if (this->x==this->x0){
		x0=this->setLineStart(&this->cachedText,start,&frame,this->horizontalCenterPolicy);
		if (x0!=this->lastStart){
			this->prebufferedText.append(L"<x=");
			this->prebufferedText.append(itoa<wchar_t>(x0));
			this->prebufferedText.push_back('>');
			this->lastStart=x0;
		}
	}else
		x0=this->x;
	y0=this->y;
	int lineSkip=this->foregroundLayer->fontCache->font->lineSkip;
	int fontLineSkip=this->foregroundLayer->fontCache->font->fontLineSkip;
	ulong t0,t1;
	if (this->resumePrinting)
		start=this->resumePrintingWhere;
	for (ulong a=start;a<end && a<this->cachedText.size();a++){
		t0=SDL_GetTicks();
		NONS_Glyph *glyph=this->foregroundLayer->fontCache->getGlyph(this->cachedText[a]);
		NONS_Glyph *glyph2=0;
		if (this->shadowLayer)
			glyph2=this->shadowLayer->fontCache->getGlyph(this->cachedText[a]);
		if (!glyph){
			if (y0+lineSkip>=this->h+this->y0){
				this->resumePrinting=1;
				this->x=x0;
				this->y=y0;
				this->resumePrinting=1;
				this->resumePrintingWhere=a+1;
				this->currentBuffer.append(this->prebufferedText);
				this->prebufferedText.clear();
				InputObserver.detach(queue);
				return 1;
			}
			if (a<this->cachedText.size()-1){
				x0=this->setLineStart(&this->cachedText,a+1,&frame,this->horizontalCenterPolicy);
				if (x0!=this->lastStart){
					this->prebufferedText.append(L"<x=");
					this->prebufferedText.append(itoa<wchar_t>(x0));
					this->prebufferedText.push_back('>');
					this->lastStart=x0;
				}
			}else
				x0=this->x0;
			y0+=lineSkip;
			this->prebufferedText.push_back(10);
			continue;
		}
		int advance=glyph->getadvance()+this->extraAdvance;
		if (x0+advance>this->w+this->x0){
			if (y0+lineSkip>=this->h+this->y0){
				this->resumePrinting=1;
				this->x=x0;
				this->y=y0;
				this->resumePrinting=1;
				this->resumePrintingWhere=isbreakspace(glyph->codePoint)?a+1:a;
				this->currentBuffer.append(this->prebufferedText);
				this->prebufferedText.clear();
				InputObserver.detach(queue);
				return 1;
			}else{
				x0=this->setLineStart(&this->cachedText,a,&frame,this->horizontalCenterPolicy);
				if (x0!=this->lastStart){
					this->prebufferedText.append(L"<x=");
					this->prebufferedText.append(itoa<wchar_t>(x0));
					this->prebufferedText.push_back('>');
					this->lastStart=x0;
				}
				y0+=lineSkip;
				this->prebufferedText.push_back('\n');
			}
		}
		switch (glyph->codePoint){
			case '\\':
				this->prebufferedText.append(L"\\\\");
				break;
			case '<':
				this->prebufferedText.append(L"\\<");
				break;
			case '>':
				this->prebufferedText.append(L"\\>");
				break;
			default:
				this->prebufferedText.push_back(glyph->codePoint);
		}
		LOCKSCREEN;
		if (glyph2){
			glyph2->putGlyph(this->shadowLayer->data,x0+1-this->shadowLayer->clip_rect.x,y0+1-this->shadowLayer->clip_rect.y,&(this->shadowLayer->fontCache->foreground),1);
			glyph2->putGlyph(dst->virtualScreen,x0+1,y0+1,0);
		}
		glyph->putGlyph(this->foregroundLayer->data,x0-this->foregroundLayer->clip_rect.x,y0-this->foregroundLayer->clip_rect.y,&(this->foregroundLayer->fontCache->foreground),1);
		glyph->putGlyph(dst->virtualScreen,x0,y0,0);
		UNLOCKSCREEN;
		dst->updateScreen(x0,y0,advance+1,fontLineSkip+1);
		if (printedChars)
			(*printedChars)++;
		x0+=advance;
		while (!CURRENTLYSKIPPING && !enterPressed && !queue->data.empty()){
			SDL_Event event=queue->pop();
			if (event.type==SDL_KEYDOWN && (event.key.keysym.sym==SDLK_RETURN || event.key.keysym.sym==SDLK_SPACE))
				enterPressed=1;
		}
		t1=SDL_GetTicks();
		if (!CURRENTLYSKIPPING && !enterPressed && this->display_speed>t1-t0)
			SDL_Delay(this->display_speed-(t1-t0));
	}
	this->x=x0;
	this->y=y0;
	InputObserver.detach(queue);
	this->resumePrinting=0;
	this->resumePrintingWhere=0;
	return 0;
}

void NONS_StandardOutput::endPrinting(){
	if (this->printingStarted)
		this->currentBuffer.append(this->prebufferedText);
	this->prebufferedText.clear();
	this->cachedText.clear();
	this->printingStarted=0;
}

void NONS_StandardOutput::ephemeralOut(std::wstring *str,NONS_VirtualScreen *dst,bool update,bool writeToLayers,SDL_Color *col){
	int x=this->x0,
		y=this->y0;
	int lineSkip=this->foregroundLayer->fontCache->font->lineSkip;
	if (writeToLayers){
		this->foregroundLayer->Clear();
		if (this->shadowLayer)
			this->shadowLayer->Clear();
	}
	long lastStart=this->x0;
	for (ulong a=0;a<str->size();a++){
		wchar_t character=(*str)[a];
		if (character=='<'){
			std::wstring tagname=tagName(*str,a);
			if (tagname.size()){
				if (tagname==L"x"){
					std::wstring tagvalue=tagValue(*str,a);
					if (tagvalue.size())
						lastStart=x=atoi(tagvalue);
				}else if (tagname==L"y"){
					std::wstring tagvalue=tagValue(*str,a);
					if (tagvalue.size())
						y=atoi(tagvalue);
				}
				a=str->find('>',a);
			}
			continue;
		}
		if (character=='\\')
			character=(*str)[++a];
		NONS_Glyph *glyph=this->foregroundLayer->fontCache->getGlyph(character);
		NONS_Glyph *glyph2=0;
		if (this->shadowLayer)
			glyph2=this->shadowLayer->fontCache->getGlyph(character);
		if (character=='\n'){
			x=lastStart;
			y+=lineSkip;
		}else{
			if (writeToLayers){
				if (glyph2){
					glyph2->putGlyph(this->shadowLayer->data,x+1,y+1,&this->shadowLayer->fontCache->foreground,1);
					if (!!dst)
						glyph2->putGlyph(dst->virtualScreen,x+1,y+1,0);
				}
				glyph->putGlyph(this->foregroundLayer->data,x,y,!col?&this->foregroundLayer->fontCache->foreground:col,1);
				if (!!dst){
					if (glyph2)
						glyph2->putGlyph(dst->virtualScreen,x+1,y+1,0);
					glyph->putGlyph(dst->virtualScreen,x,y,0);
				}
			}else if (!!dst){
				if (glyph2)
					glyph2->putGlyph(dst->virtualScreen,x+1,y+1,&this->shadowLayer->fontCache->foreground);
				glyph->putGlyph(dst->virtualScreen,x,y,!col?&this->foregroundLayer->fontCache->foreground:col);
			}
			x+=glyph->getadvance();
		}
	}
	if (update && !!dst)
		dst->updateWholeScreen();
	return;
}

/*int NONS_StandardOutput::setLineStart(std::vector<NONS_Glyph *> *arr,long start,SDL_Rect *frame,float center){
	while (start<arr->size() && !(*arr)[start])
		start++;
	int width=this->predictLineLength(arr,start,frame->w);
	float factor=(center<=.5)?center:1.0-center;
	int pixelcenter=float(frame->w)*factor;
	return (width/2>pixelcenter)?frame->x+(frame->w-width)*(center>.5):frame->x+frame->w*center-width/2;
}*/

int NONS_StandardOutput::setLineStart(std::wstring *arr,ulong start,SDL_Rect *frame,float center){
	while (start<arr->size() && !(*arr)[start])
		start++;
	int width=this->predictLineLength(arr,start,frame->w);
	float factor=(center<=.5)?center:1.0-center;
	int pixelcenter=float(frame->w)*factor;
	return (width/2>pixelcenter)?frame->x+(frame->w-width)*(center>.5):frame->x+frame->w*center-width/2;
}

/*int NONS_StandardOutput::predictLineLength(std::vector<NONS_Glyph *> *arr,long start,int width){
	int res=0;
	for (ulong a=start;a<arr->size() && (*arr)[a];a++){
		if (res+(*arr)[a]->getadvance()+this->extraAdvance>=width)
			break;
		res+=(*arr)[a]->getadvance()+this->extraAdvance;
	}
	return res;
}*/

int NONS_StandardOutput::predictLineLength(std::wstring *arr,long start,int width){
	int res=0;
	for (ulong a=start;a<arr->size() && (*arr)[a];a++){
		NONS_Glyph *glyph=this->foregroundLayer->fontCache->getGlyph((*arr)[a]);
		if (!glyph || res+glyph->getadvance()+this->extraAdvance>=width)
			break;
		res+=glyph->getadvance()+this->extraAdvance;
	}
	return res;
}

int NONS_StandardOutput::predictTextHeight(std::wstring *arr){
	int lines=1;
	for (ulong a=0;a<arr->size() && (*arr)[a];a++){
		wchar_t char0=(*arr)[a];
		if (char0==10 || char0==13)
			lines++;
	}
	if ((*arr)[arr->size()-1]==13 || (*arr)[arr->size()-1]==10)
		lines--;
	return this->foregroundLayer->fontCache->font->lineSkip*lines;
}

int NONS_StandardOutput::setTextStart(std::wstring *arr,SDL_Rect *frame,float center){
	int height=this->predictTextHeight(arr);
	float factor=(center<=.5)?center:1.0-center;
	int pixelcenter=float(frame->h)*factor;
	return (height/2>pixelcenter)?frame->y+(frame->h-height)*(center>.5):frame->y+frame->h*center-height/2;
}

void NONS_StandardOutput::Clear(bool eraseBuffer){
	this->foregroundLayer->Clear();
	if (this->shadowLayer)
		this->shadowLayer->Clear();
	this->x=this->x0;
	this->y=this->y0;
	if (eraseBuffer){
		if (this->printingStarted){
			this->currentBuffer.append(this->prebufferedText);
			this->prebufferedText.clear();
		}
		if (this->currentBuffer.size()>0){
			if (textDumpFile.is_open()){
				textDumpFile <<UniToUTF8(this->currentBuffer)<<std::endl;
				textDumpFile.flush();
			}
			this->log.push_back(this->currentBuffer);
			this->currentBuffer.clear();
		}
	}
	if (this->verticalCenterPolicy>0 && this->cachedText.size()){
		SDL_Rect frame={this->x0,this->y0,this->w,this->h};
		this->y=this->setTextStart(&this->cachedText,&frame,this->verticalCenterPolicy);
		this->prebufferedText.append(L"<y=");
		this->prebufferedText.append(itoa<wchar_t>(this->y));
		this->prebufferedText.push_back('>');
	}
}

void NONS_StandardOutput::setPosition(int x,int y){
	this->x=this->x0+x;
	this->y=this->y0+y;
	this->currentBuffer.append(L"<x=");
	this->currentBuffer.append(itoa<wchar_t>(this->x));
	this->currentBuffer.append(L"><y=");
	this->currentBuffer.append(itoa<wchar_t>(this->y));
	this->currentBuffer.push_back(L'>');
}

float NONS_StandardOutput::getCenterPolicy(char which){
	which=tolower(which);
	return (which=='v')?this->verticalCenterPolicy:this->horizontalCenterPolicy;
}

void NONS_StandardOutput::setCenterPolicy(char which,float val){
	which=tolower(which);
	if (val<0)
		val=-val;
	if (val>1){
		ulong val2=val;
		val-=val2;
	}
	if (which=='v')
		this->verticalCenterPolicy=val;
	else
		this->horizontalCenterPolicy=val;
}

void NONS_StandardOutput::setCenterPolicy(char which,long val){
	this->setCenterPolicy(which,float(val)/100);
}

bool NONS_StandardOutput::NewLine(){
	int skip=this->foregroundLayer->fontCache->font->lineSkip;
	if (this->y+skip>=this->y0+this->h)
		return 1;
	this->y+=skip;
	this->x=this->x0;
	if (this->printingStarted)
		this->prebufferedText.append(L"\n");
	else
		this->currentBuffer.append(L"\n");
	return 0;
}
#endif
