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

#ifndef NONS_MENU_CPP
#define NONS_MENU_CPP

#include "Menu.h"
#include "../../Functions.h"
#include "../IOFunctions.h"
#include "../../Globals.h"
#include "../../Processing/ScriptInterpreter.h"
#include "../SaveFile.h"

NONS_Menu::NONS_Menu(void *interpreter){
	this->interpreter=interpreter;
	this->on.r=0xFF;
	this->on.g=0xFF;
	this->on.b=0xFF;
	this->off.r=0xAA;
	this->off.g=0xAA;
	this->off.b=0xAA;
	this->nofile=this->off;
	this->shadow=1;
	this->font=0;
	this->defaultFont=((NONS_ScriptInterpreter *)interpreter)->main_font;
	this->buttons=0;
	this->files=0;
	NONS_ScreenSpace *scr=((NONS_ScriptInterpreter *)interpreter)->everything->screen;
	this->shade=new NONS_Layer(&(scr->screen->virtualScreen->clip_rect),0xCCCCCCCC|amask);
	this->slots=10;
	this->stringSave=0;
	this->stringLoad=0;
	this->stringSlot=0;
	this->audio=((NONS_ScriptInterpreter *)interpreter)->everything->audio;
	this->archive=((NONS_ScriptInterpreter *)interpreter)->everything->archive;
	this->voiceCancel=0;
	this->voiceClick=0;
	this->voiceEntry=0;
	this->voiceMO=0;
	this->voiceNo=0;
	this->voiceYes=0;
	this->rightClickMode=1;
}

NONS_Menu::NONS_Menu(std::vector<wchar_t *> *options,void *interpreter){
	this->interpreter=interpreter;
	for (ulong a=0;a<options->size();a++){
		this->strings.push_back((*options)[a++]);
		this->commands.push_back((*options)[a]);
	}
	this->on.r=0xFF;
	this->on.g=0xFF;
	this->on.b=0xFF;
	this->off.r=0xA9;
	this->off.g=0xA9;
	this->off.b=0xA9;
	this->shadow=1;
	this->font=0;
	NONS_ScreenSpace *scr=((NONS_ScriptInterpreter *)interpreter)->everything->screen;
	this->defaultFont=((NONS_ScriptInterpreter *)interpreter)->main_font;
	this->buttons=new NONS_ButtonLayer(this->defaultFont,scr,1,0);
	this->files=new NONS_ButtonLayer(this->defaultFont,scr,1,0);
	int w=scr->screen->virtualScreen->w,
		h=scr->screen->virtualScreen->h;
	this->audio=((NONS_ScriptInterpreter *)interpreter)->everything->audio;
	this->archive=((NONS_ScriptInterpreter *)interpreter)->everything->archive;
	this->buttons->makeTextButtons(
		&(this->strings),
		&(this->on),
		&(this->off),
		this->shadow,
		this->voiceEntry,this->voiceMO,this->voiceClick,this->audio,this->archive,
		w,
		h);
	this->x=(w-this->buttons->boundingBox.w)/2;
	this->y=(h-this->buttons->boundingBox.h)/2;
	this->shade=new NONS_Layer(&(scr->screen->virtualScreen->clip_rect),0xCCCCCCCC|amask);
	this->voiceCancel=0;
	this->voiceClick=0;
	this->voiceEntry=0;
	this->voiceMO=0;
	this->voiceNo=0;
	this->voiceYes=0;
	this->rightClickMode=1;
}

NONS_Menu::~NONS_Menu(){
	if (this->buttons)
		delete this->buttons;
	for (ulong a=0;a<this->strings.size();a++){
		delete[] this->strings[a];
		delete[] this->commands[a];
	}
	if (this->font)
		delete this->font;
	delete this->shade;
}

int NONS_Menu::callMenu(){
	((NONS_ScriptInterpreter *)this->interpreter)->everything->screen->BlendNoText(0);
	multiplyBlend(this->shade->data,0,((NONS_ScriptInterpreter *)this->interpreter)->everything->screen->screenBuffer,0);
	manualBlit(((NONS_ScriptInterpreter *)this->interpreter)->everything->screen->screenBuffer,0,
		((NONS_ScriptInterpreter *)this->interpreter)->everything->screen->screen->virtualScreen,0);
	int choice=this->buttons->getUserInput(this->x,this->y);
	if (choice<0){
		if (this->voiceCancel){
			if (this->audio->bufferIsLoaded(this->voiceCancel))
				this->audio->playSoundAsync(this->voiceCancel,0,0,7,0);
			else{
				long l;
				char *buffer=(char *)this->archive->getFileBuffer(this->voiceCancel,(ulong *)&l);
				if (this->audio->playSoundAsync(this->voiceCancel,buffer,l,7,0)!=NONS_NO_ERROR)
					delete[] buffer;
			}
		}
		return 0;
	}
	return this->call(this->commands[choice]);
}

void NONS_Menu::reset(){
	if (this->buttons)
		delete this->buttons;
	if (this->font)
		delete this->font;
	//this->font=new NONS_Font("default.ttf",this->fontsize,TTF_STYLE_NORMAL);
	INIT_NONS_FONT(this->font,this->fontsize,this->archive)
	this->font->spacing=this->spacing;
	this->font->lineSkip=this->lineskip;
	this->shade->setShade(this->shadeColor.r,this->shadeColor.g,this->shadeColor.b);
	NONS_ScreenSpace *scr=((NONS_ScriptInterpreter *)interpreter)->everything->screen;
	this->buttons=new NONS_ButtonLayer(this->font,scr,1,0);
	int w=scr->screen->virtualScreen->w,
		h=scr->screen->virtualScreen->h;
	this->buttons->makeTextButtons(
		&(this->strings),
		&(this->on),
		&(this->off),
		this->shadow,
		0,0,0,0,0,
		w,h);
	this->x=(w-this->buttons->boundingBox.w)/2;
	this->y=(h-this->buttons->boundingBox.h)/2;
}

void NONS_Menu::resetStrings(std::vector<wchar_t *> *options){
	if (this->buttons)
		delete this->buttons;
	for (ulong a=0;a<this->strings.size();a++){
		delete[] this->strings[a];
		delete[] this->commands[a];
	}
	this->strings.clear();
	this->commands.clear();
	for (ulong a=0;a<options->size();a++){
		this->strings.push_back((*options)[a++]);
		this->commands.push_back((*options)[a]);
	}
	NONS_ScreenSpace *scr=((NONS_ScriptInterpreter *)interpreter)->everything->screen;
	this->buttons=new NONS_ButtonLayer(!this->font?this->defaultFont:this->font,scr,1,0);
	int w=scr->screen->virtualScreen->w,
		h=scr->screen->virtualScreen->h;
	this->buttons->makeTextButtons(
		&(this->strings),
		&(this->on),
		&(this->off),
		this->shadow,
		0,0,0,0,0,
		w,h);
	this->x=(w-this->buttons->boundingBox.w)/2;
	this->y=(h-this->buttons->boundingBox.h)/2;
}

int NONS_Menu::write(wchar_t *txt,int y){
	NONS_FontCache *tempCacheForeground=new NONS_FontCache(this->font?this->font:this->defaultFont,&(this->on),0),
		*tempCacheShadow=0;
	if (this->shadow){
		SDL_Color black={0,0,0,0};
		tempCacheShadow=new NONS_FontCache(this->font?this->font:this->defaultFont,&black,1);
	}
	std::vector<NONS_Glyph *> outputBuffer;
	std::vector<NONS_Glyph *> outputBuffer2;
	long width=0;
	for (wchar_t *str=txt;*str;str++){
		NONS_Glyph *glyph=tempCacheForeground->getGlyph(*str);
		width+=glyph->getadvance();
		outputBuffer.push_back(glyph);
		if (tempCacheShadow)
			outputBuffer2.push_back(tempCacheShadow->getGlyph(*str));
	}
	NONS_ScreenSpace *scr=((NONS_ScriptInterpreter *)interpreter)->everything->screen;
	int w=scr->screen->virtualScreen->w;
	//Unused:
	//	h=scr->screen->virtualScreen->h;
	int x=(w-width)/2;
	for (ulong a=0;a<outputBuffer.size();a++){
		int advance=outputBuffer[a]->getadvance();
		if (tempCacheShadow)
			outputBuffer2[a]->putGlyph(scr->screen->virtualScreen,x+1,y+1,0,0);
		outputBuffer[a]->putGlyph(scr->screen->virtualScreen,x,y,0,0);
		x+=advance;
	}
	delete tempCacheForeground;
	delete tempCacheShadow;
	return (this->font?this->font:this->defaultFont)->lineSkip;
}

int NONS_Menu::save(){
	int y0=this->write(this->stringSave?this->stringSave:((wchar_t *)L"~~ Save File ~~"),20);
	NONS_ScreenSpace *scr=((NONS_ScriptInterpreter *)interpreter)->everything->screen;
	if (!this->files){
		this->files=new NONS_ButtonLayer(this->font?this->font:this->defaultFont,scr,1,0);
	}
	std::vector<tm *> *files=existing_files(save_directory);
	int choice;
	while (1){
		std::vector<wchar_t *> strings;
		for (ulong a=0;a<slots;a++){
			wchar_t *txt=new wchar_t[31];
			tm *t=(*files)[a];
			if (t)
#ifndef __MINGW32__
				swprintf(txt,31,L" %2d    %04d-%02d-%02d %02d:%02d:%02d",a+1,t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
#else
				swprintf(txt,L" %2d    %04d-%02d-%02d %02d:%02d:%02d",a+1,t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
#endif
			else
#ifndef __MINGW32__
				swprintf(txt,31,L" %2d    -------------------",a+1);
#else
				swprintf(txt,L" %2d    -------------------",a+1);
#endif
			wchar_t *pusher=copyWString(this->stringSlot?this->stringSlot:((wchar_t *)L"Slot"));
			addStringsInplace(&pusher,txt);
			delete[] txt;
			strings.push_back(pusher);
		}
		int w=scr->screen->virtualScreen->w,
			h=scr->screen->virtualScreen->h;
		this->files->makeTextButtons(&strings,
			&(this->on),
			&(this->off),
			this->shadow,
			this->voiceEntry,
			this->voiceMO,
			this->voiceClick,
			this->audio,
			this->archive,w,h);
		for (ulong a=0;a<strings.size();a++)
			delete[] strings[a];
		choice=this->files->getUserInput((w-this->files->boundingBox.w)/2,y0*2+20);
		if (choice==-2){
			this->slots--;
			continue;
		}
		if (choice<0){
			if (this->voiceCancel){
				if (this->audio->bufferIsLoaded(this->voiceCancel))
					this->audio->playSoundAsync(this->voiceCancel,0,0,7,0);
				else{
					long l;
					char *buffer=(char *)this->archive->getFileBuffer(this->voiceCancel,(ulong *)&l);
					if (this->audio->playSoundAsync(this->voiceCancel,buffer,l,7,0)!=NONS_NO_ERROR)
						delete[] buffer;
				}
			}
		}
		break;
	}
	for (ulong a=0;a<files->size();a++)
		delete (*files)[a];
	delete files;
	return choice+1;
}

int NONS_Menu::load(){
	int y0=this->write(this->stringLoad?this->stringLoad:((wchar_t *)L"~~ Load File ~~"),20);
	NONS_ScreenSpace *scr=((NONS_ScriptInterpreter *)interpreter)->everything->screen;
	if (!this->files)
		this->files=new NONS_ButtonLayer(this->font?this->font:this->defaultFont,scr,1,0);
	std::vector<tm *> *files=existing_files(save_directory);
	int choice;
	while (1){
		std::vector<wchar_t *> strings;
		wchar_t *txt=new wchar_t[31];
		for (ulong a=0;a<slots;a++){
			tm *t=(*files)[a];
			if (t)
#ifndef __MINGW32__
				swprintf(txt,31,L" %2d    %04d-%02d-%02d %02d:%02d:%02d",a+1,t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
#else
				swprintf(txt,L" %2d    %04d-%02d-%02d %02d:%02d:%02d",a+1,t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
#endif
			else
#ifndef __MINGW32__
				swprintf(txt,31,L" %2d    -------------------",a+1);
#else
				swprintf(txt,L" %2d    %04d-%02d-%02d %02d:%02d:%02d",a+1,t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
#endif
			wchar_t *pusher=copyWString(this->stringSlot?this->stringSlot:((wchar_t *)L"Slot"));
			addStringsInplace(&pusher,txt);
			strings.push_back(pusher);
		}
		delete[] txt;
		int w=scr->screen->virtualScreen->w,
			h=scr->screen->virtualScreen->h;
		this->files->makeTextButtons(&strings,
			&(this->on),
			&(this->off),
			this->shadow,
			this->voiceEntry,
			this->voiceMO,
			this->voiceClick,
			this->audio,
			this->archive,
			w,h);
		for (ulong a=0;a<strings.size();a++)
			delete[] strings[a];
		choice=this->files->getUserInput((w-this->files->boundingBox.w)/2,y0*2+20);
		if (choice==-2){
			this->slots--;
			continue;
		}
		if (choice<0 && this->voiceCancel){
			if (this->audio->bufferIsLoaded(this->voiceCancel))
				this->audio->playSoundAsync(this->voiceCancel,0,0,7,0);
			else{
				long l;
				char *buffer=(char *)this->archive->getFileBuffer(this->voiceCancel,(ulong *)&l);
				if (this->audio->playSoundAsync(this->voiceCancel,buffer,l,7,0)!=NONS_NO_ERROR)
					delete[] buffer;
			}
		}
		break;
	}
	for (ulong a=0;a<files->size();a++)
		delete (*files)[a];
	delete files;
	return choice+1;
}

int NONS_Menu::windowerase(){
	((NONS_ScriptInterpreter *)this->interpreter)->everything->screen->BlendNoText(1);
	NONS_EventQueue *queue=InputObserver.attach();
	while (1){
		queue->WaitForEvent();
		while (!queue->data.empty()){
			SDL_Event event=queue->data.front();
			queue->pop();
			if (event.type==SDL_KEYDOWN || event.type==SDL_MOUSEBUTTONDOWN){
				InputObserver.detach(queue);
				return 0;
			}
		}
	}
}

int NONS_Menu::skip(){
	ctrlIsPressed=1;
	return 0;
}

int NONS_Menu::call(wchar_t *string){
	int ret=0;
	if (!wcscmp(string,L"reset"))
		ret=-1;
	if (!wcscmp(string,L"save")){
		int save=this->save();
		if (save>0){
			((NONS_ScriptInterpreter *)this->interpreter)->save(save);
			//ret=-1;
		}
	}else if (!wcscmp(string,L"load")){
		int load=this->load();
		if (load>0 && ((NONS_ScriptInterpreter *)this->interpreter)->load(load))
			ret=-1;
	}else if (!wcscmp(string,L"windowerase")){
		this->windowerase();
	}else if (!wcscmp(string,L"lookback")){
		NONS_ScreenSpace *scr=((NONS_ScriptInterpreter *)this->interpreter)->everything->screen;
		scr->BlendNoText(0);
		manualBlit(scr->screenBuffer,0,scr->screen->virtualScreen,0);
		multiplyBlend(
			scr->output->shadeLayer->data,
			&(scr->output->shadeLayer->clip_rect),
			scr->screen->virtualScreen,
			0);
		scr->lookback->display(scr->screen);
	}else if (!wcscmp(string,L"skip")){
		this->skip();
	}else{
		ErrorCode error=((NONS_ScriptInterpreter *)this->interpreter)->interpretString(string);
		if (error!=NONS_NO_ERROR)
			handleErrors(error,-1,"NONS_Menu::call",1);
	}
	return ret;
}
#endif
