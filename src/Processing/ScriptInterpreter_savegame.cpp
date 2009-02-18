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

#ifndef NONS_SCRIPTINTERPRETER_SAVEGAME_CPP
#define NONS_SCRIPTINTERPRETER_SAVEGAME_CPP

#include "ScriptInterpreter.h"
#include "../Functions.h"
#include "../Globals.h"
#include "../IO_System/FileIO.h"
#include "../IO_System/IOFunctions.h"

extern wchar_t Unicode2SJIS[];

long SJISoffset_to_WCSoffset(wchar_t *buffer,long offset){
	ulong res=0;
	for (;offset;offset--,res++)
		if (Unicode2SJIS[buffer[res]]>=0x80)
			offset--;
	return res;
}

bool NONS_ScriptInterpreter::load(int file){
	char *path=save_directory;
	//this line makes sure that 'filename' has enough space for sprintf()
	char *filename=addStrings(path,"save01.dat");
	sprintf(filename,"%ssave%d.dat",path,file);
	NONS_SaveFile *save=new NONS_SaveFile;
	save->load(filename);
	delete[] filename;
	if (save->error!=NONS_NO_ERROR){
		delete save;
		return 0;
	}
	if (save->format=='O'){
		this->main_font->lineSkip=save->lineSkip;
		this->main_font->spacing=save->spacing;
		//set window
		NONS_ScreenSpace *scr=this->everything->screen;
		NONS_StandardOutput *out=scr->output;
		out->shadeLayer->clip_rect=save->textWindow;
		out->shadeLayer->setShade(save->windowColor.r,save->windowColor.g,save->windowColor.b);
		out->x0=save->windowFrame.x;
		out->y0=save->windowFrame.y;
		out->w=save->windowFrame.w;
		out->h=save->windowFrame.h;
		if (save->fontShadow){
			if (!out->shadowLayer){
				out->shadowLayer=new NONS_Layer(&(out->foregroundLayer->data->clip_rect),0);
				SDL_Color a={0,0,0,0};
				out->shadowLayer->MakeTextLayer(this->main_font,&a,1);
			}
		}else if (out->shadowLayer){
			delete out->shadowLayer;
			out->shadowLayer=0;
		}
		out->foregroundLayer->fontCache->foreground=save->windowTextColor;
		if (*save->arrowCursorString){
			if (this->arrowCursor)
				delete this->arrowCursor;
			this->arrowCursor=new NONS_Cursor(save->arrowCursorString,save->arrowCursorX,save->arrowCursorY,save->arrowCursorAbs);
		}
		if (*save->pageCursorString){
			if (this->pageCursor)
				delete this->pageCursor;
			this->pageCursor=new NONS_Cursor(save->pageCursorString,save->pageCursorX,save->pageCursorY,save->pageCursorAbs);
		}
		out->transition->effect=save->windowTransition;
		out->transition->duration=save->windowTransitionDuration;
		/*if (*save->windowBackgroundImage)
			out->shadeLayer->usePicAsDefaultShade(save->windowBackgroundImage);*/
		//out->shadeLayer->visible=!save->transparentWindow;
		this->hideTextDuringEffect=save->hideWindow;
		out->display_speed=save->textSpeed;
		//screen content
		if (save->monochrome){
			if (!scr->monochrome)
				scr->monochrome=new NONS_GFX();
			scr->monochrome->type=POSTPROCESSING;
			scr->monochrome->color=save->monochromeColor;
			scr->monochrome->effect=0;
		}
		if (save->negative){
			if (!scr->negative)
				scr->negative=new NONS_GFX();
			scr->negative->type=POSTPROCESSING;
			scr->negative->effect=1;
		}
		scr->clearText();
		if (*save->background)
			scr->Background->load(save->background,&scr->screen->virtualScreen->clip_rect,NO_ALPHA);
		else
			scr->Background->Clear();
		if (*save->leftChar){
			long semicolon=instr(save->leftChar,L";");
			semicolon++;
			wchar_t *name=copyWString(save->leftChar+semicolon);
			if (!scr->leftChar)
				scr->leftChar=new NONS_Layer(name,&(scr->screen->virtualScreen->clip_rect),CLOptions.layerMethod);
			else
				scr->leftChar->load(name,&(scr->screen->virtualScreen->clip_rect),CLOptions.layerMethod);
			delete[] name;
		}
		if (*save->righChar){
			long semicolon=instr(save->righChar,L";");
			semicolon++;
			wchar_t *name=copyWString(save->righChar+semicolon);
			if (!scr->rightChar)
				scr->rightChar=new NONS_Layer(name,&(scr->screen->virtualScreen->clip_rect),CLOptions.layerMethod);
			else
				scr->rightChar->load(name,&(scr->screen->virtualScreen->clip_rect),CLOptions.layerMethod);
			delete[] name;
		}
		if (*save->centChar){
			long semicolon=instr(save->centChar,L";");
			semicolon++;
			wchar_t *name=copyWString(save->centChar+semicolon);
			if (!scr->rightChar)
				scr->centerChar=new NONS_Layer(name,&(scr->screen->virtualScreen->clip_rect),CLOptions.layerMethod);
			else
				scr->centerChar->load(name,&(scr->screen->virtualScreen->clip_rect),CLOptions.layerMethod);
			delete[] name;
		}
		for (ulong a=0;a<save->sprites.size();a++){
			NONS_SaveFile::Sprite *spr=save->sprites[a];
			if (!spr)
				continue;
			wchar_t *str=spr->string;
			METHODS method=NO_ALPHA;
			if (*str==':'){
				str++;
				for (;*str && iswhitespace((char)*str);str++);
				switch (*str){
					case 'a':
						method=CLASSIC_METHOD;
						break;
					case 'c':
						method=NO_ALPHA;
						break;
					default:
						break;
				}
				for (;*str && *str!='/' && *str!=';';str++);
			}
			if (*str=='/')
				for (;*str && *str!=';';str++);
			if (*str==';')
				str++;
			str=copyWString(str);
			scr->loadSprite(a,spr->string,str,spr->x,spr->y,0xFF,method,spr->visibility);
		}
		this->everything->screen->sprite_priority=this->everything->screen->layerStack.size()-1;
		//variables
		for (ulong a=0;a<save->variables.size();a++){
			NONS_Variable *var=save->variables[a];
			NONS_Variable *dst=this->store->retrieve(a);
			dst->intValue=var->intValue;
			if (dst->wcsValue)
				delete[] dst->wcsValue;
			if (var->wcsValue && *var->wcsValue)
				dst->wcsValue=copyWString(var->wcsValue);
			else
				dst->wcsValue=0;
		}
		//stack
		//flush
		while (!this->callStack.empty()){
			NONS_StackElement *p=*(this->callStack.end()-1);
			delete p;
			this->callStack.pop_back();
		}
		for (ulong a=0;a<save->stack.size();a++){
			NONS_SaveFile::stackEl *el=save->stack[a];

			if (!el->type){
				NONS_StackElement *push=new NONS_StackElement(SJISoffset_to_WCSoffset(this->script->script,el->offset),0);
				this->callStack.push_back(push);
			}else{
				NONS_StackElement *push=new NONS_StackElement(
					this->store->retrieve(el->variable),
					SJISoffset_to_WCSoffset(this->script->script,el->offset),
					0,el->to,el->step);
				this->callStack.push_back(push);
			}
		}
		{
			ulong pos=0,
				len=this->script->length,
				targetline=save->currentLine-1;
			wchar_t *buffer=this->script->script;
			bool break0=0;
			for (ulong a=0;a<targetline;a++){
				ulong temppos=pos;
				for (;pos<len && buffer[pos]!=10 && buffer[pos]!=13;pos++);
				if (pos==len){
					pos=temppos;
					break0=1;
					break;
				}
				if (buffer[pos]==10 || buffer[pos]==13 && (pos+1>=len || buffer[pos+1]!=10))
					pos++;
				else
					pos+=2;
			}
			if (!break0){
				for (ulong sl=save->currentSubline;sl>0;sl--){
					ulong a=instr(buffer+pos,":");
					if (a<0)
						break;
					pos+=a;
				}
			}
			this->interpreter_position=pos;
			if (!!this->saveGame->currentLabel)
				delete[] this->saveGame->currentLabel;
			this->saveGame->currentLabel=copyWString(this->script->blockFromOffset(this->interpreter_position));
		}
		//audio
		this->everything->audio->stopAllSound();
		if (*save->music || *save->loopBGM0 || *save->loopBGM1 || *save->midi){
			wchar_t *name;
			bool loop=0;
			if (*save->music){
				name=save->music;
				loop=save->loopMp3;
			}else if (*save->loopBGM0){
				name=save->loopBGM0;
				loop=1;
			}else if (*save->loopBGM1){
				name=save->loopBGM1;
				loop=1;
			}else{
				name=save->midi;
				loop=save->loopMidi;
			}
			ulong size;
			char *buffer=(char *)this->everything->archive->getFileBuffer(name,&size);
			if (buffer)
				this->everything->audio->playMusic(name,buffer,size,save->loopMidi);
		}else if (save->musicTrack>=0){
			char temp[12];
			sprintf(temp,"track%02u",save->musicTrack);
			this->everything->audio->playMusic(temp,save->loopMp3);
		}
		//right click
		this->menu->rightClickMode=save->rmode;
		//log
		this->everything->screen->output->log.clear();
		for (ulong a=0;a<save->logPages.size();a++){
			this->everything->screen->output->log.push_back(save->logPages[a]);
		}
	}else{
		//**********************************************************************
		//NONS save file
		//**********************************************************************
		if (save->version>NONS_SAVEFILE_VERSION){
			delete save;
			return 0;
		}
		for (ulong a=0;a<5;a++){
			if (save->hash[a]!=this->script->hash[a]){
				delete save;
				return 0;
			}
		}
		//stack
		//flush
		while (!this->callStack.empty()){
			NONS_StackElement *p=*(this->callStack.end()-1);
			delete p;
			this->callStack.pop_back();
		}
		for (ulong a=0;a<save->stack.size();a++){
			NONS_SaveFile::stackEl *el=save->stack[a];
			NONS_StackElement *push;
			if (!el->type)
				push=new NONS_StackElement(this->script->offsetFromBlock(el->label)+el->offset,0);
			else{
				push=new NONS_StackElement(
					this->store->retrieve(el->variable),
					this->script->offsetFromBlock(el->label)+el->offset,
					0,el->to,el->step);
			}
			this->callStack.push_back(push);
		}
		this->interpreter_position=this->script->offsetFromBlock(save->currentLabel)+save->currentOffset;
		if (!!this->saveGame->currentLabel)
			delete[] this->saveGame->currentLabel;
		this->saveGame->currentLabel=copyWString(save->currentLabel);
		//variables
		for (ulong a=0;a<save->variables.size();a++){
			if (!save->variables[a]){
				NONS_Variable *v=this->store->retrieve(a);
				if (!!v){
					this->store->stack.erase(a);
					delete v;
				}
				continue;
			}
			std::map<ulong,NONS_Variable *>::iterator i=this->store->stack.find(a);
			if (i!=this->store->stack.end()){
				NONS_Variable **b=&(i->second);
				delete *b;
				*b=save->variables[a];
				save->variables[a]=0;
			}else{
				this->store->stack[a]=save->variables[a];
				save->variables[a]=0;
			}
		}
		for (std::map<wchar_t *,NONS_Variable *,wstrCmpCI>::iterator i=this->store->arrayVariables.begin();i!=this->store->arrayVariables.end();i++){
			delete[] i->first;
			delete i->second;
		}
		this->store->arrayVariables.clear();
		for (ulong a=0;a<save->arraynames.size();a++){
			this->store->arrayVariables[save->arraynames[a]]=save->arrays[a];
			save->arraynames[a]=0;
			save->arrays[a]=0;
		}
		//screen
		//window
		NONS_ScreenSpace *scr=this->everything->screen;
		//this->main_font=new NONS_Font("default.ttf",save->fontSize,TTF_STYLE_NORMAL);
		INIT_NONS_FONT(this->main_font,save->fontSize,this->everything->archive)
		scr->resetParameters(&save->textWindow,&save->windowFrame,this->main_font,save->fontShadow);
		NONS_StandardOutput *out=scr->output;
		/*out->shadeLayer->clip_rect=save->textWindow;
		out->x0=save->windowFrame.x;
		out->y0=save->windowFrame.y;
		out->w=save->windowFrame.w;
		out->h=save->windowFrame.h;*/
		out->shadeLayer->setShade(save->windowColor.r,save->windowColor.g,save->windowColor.b);
		out->shadeLayer->Clear();
		out->transition->effect=save->windowTransition;
		out->transition->duration=save->windowTransitionDuration;
		if (out->transition->rule)
			delete[] out->transition->rule;
		out->transition->rule=save->windowTransitionRule;
		save->windowTransitionRule=0;
		this->hideTextDuringEffect=save->hideWindow;
		out->foregroundLayer->fontCache->foreground=save->windowTextColor;
		out->display_speed=save->textSpeed;
		this->main_font->spacing=save->spacing;
		this->main_font->lineSkip=save->lineSkip;
		out->log.clear();
		for (ulong a=0;a<save->logPages.size();a++){
			if (save->logPages[a])
				out->log.push_back(save->logPages[a]);
			else
				out->log.push_back(L"");
		}
		if (save->currentBuffer)
			out->currentBuffer=save->currentBuffer;
		else
			out->currentBuffer=L"";
		out->x=save->textX;
		out->y=save->textY;
		if (!save->arrowCursorString){
			if (this->arrowCursor)
				delete this->arrowCursor;
			this->arrowCursor=0;
		}else{
			if (this->arrowCursor)
				delete this->arrowCursor;
			this->arrowCursor=new NONS_Cursor(save->arrowCursorString,save->arrowCursorX,save->arrowCursorY,save->arrowCursorAbs);
		}
		if (!save->pageCursorString){
			if (this->pageCursor)
				delete this->pageCursor;
			this->pageCursor=0;
		}else{
			if (this->pageCursor)
				delete this->pageCursor;
			this->pageCursor=new NONS_Cursor(save->pageCursorString,save->pageCursorX,save->pageCursorY,save->pageCursorAbs);
		}
		//graphics
		if (save->background)
			scr->Background->load(save->background,&scr->screen->virtualScreen->clip_rect,NO_ALPHA);
		else{
			scr->Background->setShade(save->bgColor.r,save->bgColor.g,save->bgColor.b);
			scr->Background->Clear();
		}
		scr->leftChar->unload();
		if (save->leftChar){
			long semicolon=instr(save->leftChar,L";");
			semicolon++;
			wchar_t *name=copyWString(save->leftChar+semicolon);
			if (!scr->leftChar)
				scr->leftChar=new NONS_Layer(name,&(scr->screen->virtualScreen->clip_rect),CLOptions.layerMethod);
			else
				scr->leftChar->load(name,&(scr->screen->virtualScreen->clip_rect),CLOptions.layerMethod);
			scr->leftChar->clip_rect.x=(this->everything->screen->screen->virtualScreen->w)/-4;
			delete[] name;
		}
		scr->rightChar->unload();
		if (save->righChar){
			long semicolon=instr(save->righChar,L";");
			semicolon++;
			wchar_t *name=copyWString(save->righChar+semicolon);
			if (!scr->rightChar)
				scr->rightChar=new NONS_Layer(name,&(scr->screen->virtualScreen->clip_rect),CLOptions.layerMethod);
			else
				scr->rightChar->load(name,&(scr->screen->virtualScreen->clip_rect),CLOptions.layerMethod);
			delete[] name;
			scr->leftChar->clip_rect.x=(this->everything->screen->screen->virtualScreen->w)/4;
		}
		scr->centerChar->unload();
		if (save->centChar){
			long semicolon=instr(save->centChar,L";");
			semicolon++;
			wchar_t *name=copyWString(save->centChar+semicolon);
			if (!scr->centerChar)
				scr->centerChar=new NONS_Layer(name,&(scr->screen->virtualScreen->clip_rect),CLOptions.layerMethod);
			else
				scr->centerChar->load(name,&(scr->screen->virtualScreen->clip_rect),CLOptions.layerMethod);
			delete[] name;
			scr->leftChar->clip_rect.x=0;
		}
		for (ulong a=0;a<save->sprites.size();a++){
			NONS_SaveFile::Sprite *spr=save->sprites[a];
			if (!spr){
				scr->layerStack[a]->unload();
				continue;
			}
			wchar_t *str=spr->string;
			METHODS method=NO_ALPHA;
			if (*str==':'){
				str++;
				for (;*str && iswhitespace((char)*str);str++);
				switch (*str){
					case 'a':
						method=CLASSIC_METHOD;
						break;
					case 'c':
						method=NO_ALPHA;
						break;
					default:
						break;
				}
				for (;*str && *str!='/' && *str!=';';str++);
			}
			if (*str=='/')
				for (;*str && *str!=';';str++);
			if (*str==';')
				str++;
			str=copyWString(str);
			scr->loadSprite(a,spr->string,str,spr->x,spr->y,0xFF,method,spr->visibility);
		}
		scr->sprite_priority=save->spritePriority;
		if (save->monochrome){
			if (!scr->monochrome)
				scr->monochrome=new NONS_GFX();
			scr->monochrome->type=POSTPROCESSING;
			scr->monochrome->color=save->monochromeColor;
			scr->monochrome->effect=0;
		}else if (!!scr->monochrome){
			delete scr->monochrome;
			scr->monochrome=0;
		}
		if (save->negative){
			if (!scr->negative){
				scr->negative=new NONS_GFX();
			}
			scr->negative->type=POSTPROCESSING;
			scr->negative->effect=1;
		}else if (!!scr->negative){
			delete scr->negative;
			scr->negative=0;
		}
		//Preparations for audio
		NONS_Audio *au=this->everything->audio;
		au->stopAllSound();
		out->ephemeralOut(&out->currentBuffer,0,0,1,0);
		{
			SDL_Surface *srf=SDL_CreateRGBSurface(
				SDL_HWSURFACE|SDL_SRCALPHA,
				scr->screen->virtualScreen->w,
				scr->screen->virtualScreen->h,
				32,rmask,gmask,bmask,amask);
			SDL_FillRect(srf,0,amask);
			NONS_GFX::callEffect(10,1000,0,srf,0,scr->screen);
			SDL_FreeSurface(srf);
		}
		SDL_Delay(1500);
		scr->BlendAll(10,1000,0);
		scr->showText();
		//audio
		if (save->musicTrack>=0){
			char temp[12];
			sprintf(temp,"track%02u",save->musicTrack);
			au->playMusic(temp,save->loopMp3);
		}else if (save->music){
			long size;
			char *buffer=(char *)this->everything->archive->getFileBuffer(save->music,(ulong *)&size);
			if (buffer)
				au->playMusic(save->music,buffer,size,save->loopMp3?-1:0);
			//delete[] buffer;
		}
		au->musicVolume(save->musicVolume);
		for (ushort a=0;a<save->channels.size();a++){
			NONS_SaveFile::Channel *c=save->channels[a];
			if (!c->name)
				continue;
			if (au->bufferIsLoaded(c->name))
				au->playSoundAsync(c->name,0,0,a,c->loop?-1:0);
			else{
				long size;
				char *buffer=(char *)this->everything->archive->getFileBuffer(c->name,(ulong *)&size);
				if (!!buffer)
					this->everything->audio->playSoundAsync(c->name,buffer,size,a,c->loop);
			}
		}
	}
	delete save;
	return 1;
}

bool NONS_ScriptInterpreter::save(int file){
	for (ulong a=0;a<this->saveGame->stack.size();a++)
		delete this->saveGame->stack[a];
	this->saveGame->stack.clear();
	for (ulong a=0;a<this->saveGame->variables.size();a++)
		if (this->saveGame->variables[a])
			delete this->saveGame->variables[a];
	this->saveGame->variables.clear();
	for (ulong a=0;a<this->saveGame->arraynames.size();a++){
		delete[] this->saveGame->arraynames[a];
		delete this->saveGame->arrays[a];
	}
	this->saveGame->arraynames.clear();
	this->saveGame->arrays.clear();
	for (ulong a=0;a<this->saveGame->logPages.size();a++)
		if (this->saveGame->logPages[a])
			delete[] this->saveGame->logPages[a];
	this->saveGame->logPages.clear();
	for (ulong a=0;a<this->saveGame->sprites.size();a++)
		if (this->saveGame->sprites[a])
			delete this->saveGame->sprites[a];
	this->saveGame->sprites.clear();
	if (this->saveGame->currentBuffer)
		delete[] this->saveGame->currentBuffer;
	for (ulong a=0;a<this->saveGame->channels.size();a++)
		if (this->saveGame->channels[a])
			delete this->saveGame->channels[a];
	this->saveGame->channels.clear();
	//stack
	{
		for (std::vector<NONS_StackElement *>::iterator i=this->callStack.begin();i!=this->callStack.end();i++){
			NONS_SaveFile::stackEl *el=new NONS_SaveFile::stackEl();
			NONS_StackElement *el0=*i;
			el->type=(el0->type!=SUBROUTINE_CALL);
			el->label=copyWString(this->script->blockFromOffset(el0->offset));
			el->offset=el0->offset-this->script->offsetFromBlock(el->label);
			if (!el->type){
				el->leftovers=copyWString(el0->first_interpret_string);
			}else{
				el->variable=0;
				for (std::map<ulong,NONS_Variable *>::iterator i=this->store->stack.begin();i!=this->store->stack.end() && !el->variable;i++)
					if (i->second==el0->var)
						el->variable=i->first;
				el->to=el0->to;
				el->step=el0->step;
			}
			this->saveGame->stack.push_back(el);
		}
		if (this->saveGame->currentLabel)
			delete[] this->saveGame->currentLabel;
		this->saveGame->currentLabel=copyWString(this->script->blockFromOffset(this->previous_interpreter_position));
		if (!!this->saveGame->currentLabel)
			this->saveGame->currentOffset=this->previous_interpreter_position-this->script->offsetFromBlock(this->saveGame->currentLabel);
		else
			this->saveGame->currentOffset=0;
	}
	//variables
	{
		std::map<ulong,NONS_Variable *> *varStack=&this->store->stack;
		for (std::map<ulong,NONS_Variable *>::iterator i=varStack->begin();i!=varStack->end() && i->first<200;i++){
			if (!i->second || !i->second->intValue && (!i->second->wcsValue || !*i->second->wcsValue))
				continue;
			if (this->saveGame->variables.size()<=i->first)
				this->saveGame->variables.resize(i->first+1,0);
			NONS_Variable *var=new NONS_Variable();
			var->intValue=i->second->intValue;
			var->wcsValue=copyWString(i->second->wcsValue);
			this->saveGame->variables[i->first]=var;
		}
		for (std::map<wchar_t *,NONS_Variable *,wstrCmpCI>::iterator i=this->store->arrayVariables.begin();
		  i!=this->store->arrayVariables.end();i++){
			this->saveGame->arraynames.push_back(copyWString(i->first));
			NONS_Variable *p=new NONS_Variable(*i->second);
			this->saveGame->arrays.push_back(p);
		}
	}
	//screen
	{
		//window
		NONS_ScreenSpace *scr=this->everything->screen;
		NONS_StandardOutput *out=scr->output;
		this->saveGame->textWindow=out->shadeLayer->clip_rect;
		this->saveGame->windowFrame.x=out->x0;
		this->saveGame->windowFrame.y=out->y0;
		this->saveGame->windowFrame.w=out->w;
		this->saveGame->windowFrame.h=out->h;
		ulong color=out->shadeLayer->defaultShade;
		this->saveGame->windowColor.r=(color>>rshift)&0xFF;
		this->saveGame->windowColor.g=(color>>gshift)&0xFF;
		this->saveGame->windowColor.b=(color>>bshift)&0xFF;
		this->saveGame->windowTransition=out->transition->effect;
		this->saveGame->windowTransitionDuration=out->transition->duration;
		this->saveGame->windowTransitionRule=copyWString(out->transition->rule);
		this->saveGame->hideWindow=this->hideTextDuringEffect;
		this->saveGame->fontSize=this->main_font->getsize();
		this->saveGame->windowTextColor=out->foregroundLayer->fontCache->foreground;
		this->saveGame->textSpeed=out->display_speed;
		this->saveGame->fontShadow=!!out->shadowLayer;
		this->saveGame->spacing=this->main_font->spacing;
		this->saveGame->lineSkip=
			this->main_font->lineSkip==this->main_font->fontLineSkip?
			-1:this->main_font->lineSkip;
		for (ulong a=0;a<out->log.size();a++)
			this->saveGame->logPages.push_back(copyWString((wchar_t *)out->log[a].c_str()));
		this->saveGame->currentBuffer=copyWString((wchar_t *)out->currentBuffer.c_str());
		/*this->saveGame->textX=out->x;
		this->saveGame->textY=out->y;*/
		//graphic
		{
			NONS_Image *i=ImageLoader->elementFromSurface(scr->Background->data);
			if (i){
				if (this->saveGame->background)
					delete[] this->saveGame->background;
				this->saveGame->background=copyWString(i->name);
			}else{
				if (this->saveGame->background)
					delete[] this->saveGame->background;
				this->saveGame->background=0;
				color=scr->Background->defaultShade;
				this->saveGame->bgColor.r=(color>>rshift)&0xFF;
				this->saveGame->bgColor.g=(color>>gshift)&0xFF;
				this->saveGame->bgColor.b=(color>>bshift)&0xFF;
			}
		}
		{
			NONS_Image *i=!!scr->leftChar?ImageLoader->elementFromSurface(scr->leftChar->data):0;
			if (this->saveGame->leftChar)
				delete[] this->saveGame->leftChar;
			if (i)
				this->saveGame->leftChar=copyWString(i->name);
			else
				this->saveGame->leftChar=0;
		}
		{
			NONS_Image *i=!!scr->centerChar?ImageLoader->elementFromSurface(scr->centerChar->data):0;
			if (this->saveGame->centChar)
				delete[] this->saveGame->centChar;
			if (i)
				this->saveGame->centChar=copyWString(i->name);
			else
				this->saveGame->centChar=0;
		}
		{
			NONS_Image *i=!!scr->rightChar?ImageLoader->elementFromSurface(scr->rightChar->data):0;
			if (this->saveGame->righChar)
				delete[] this->saveGame->righChar;
			if (i)
				this->saveGame->righChar=copyWString(i->name);
			else
				this->saveGame->righChar=0;
		}
		//update sprite record
		for (ulong a=0;a<scr->layerStack.size();a++){
			if (this->saveGame->sprites.size()==a)
				this->saveGame->sprites.push_back(0);
			else if (this->saveGame->sprites.size()<a)
				this->saveGame->sprites.resize(a+1,0);
			NONS_SaveFile::Sprite *b=this->saveGame->sprites[a];
			NONS_Layer *c=scr->layerStack[a];
			if (!c || !c->data){
				delete b;
				this->saveGame->sprites[a]=0;
			}else{
				if (!b){
					NONS_SaveFile::Sprite *spr=new NONS_SaveFile::Sprite();
					NONS_Image *i=ImageLoader->elementFromSurface(c->data);
					if (i){
						spr->string=copyWString(i->string);
						this->saveGame->sprites[a]=spr;
						b=spr;
					}else
						delete spr;
				}
				if (b){
					b->x=c->clip_rect.x;
					b->y=c->clip_rect.y;
					b->visibility=c->visible;
					b->alpha=c->alpha;
				}else
					v_stderr <<"NONS_ScriptInterpreter::save(): unresolvable inconsistent internal state."<<std::endl;
			}
		}
		this->saveGame->spritePriority=this->everything->screen->sprite_priority;
		this->saveGame->monochrome=!!scr->monochrome;
		if (this->saveGame->monochrome)
			this->saveGame->monochromeColor=scr->monochrome->color;
		this->saveGame->negative=!!scr->negative;
	}
	{
		NONS_Audio *au=this->everything->audio;
		if (!Mix_PlayingMusic()){
			this->saveGame->musicTrack=-1;
			if (this->saveGame->music)
				delete[] this->saveGame->music;
			this->saveGame->music=0;
		}else
			this->saveGame->loopMp3=this->mp3_loop;
		int vol=au->musicVolume(-1);
		this->saveGame->musicVolume=vol<0?100:vol;
		SDL_LockMutex(au->soundcache->mutex);
		for (std::list<NONS_SoundEffect *>::iterator i=au->soundcache->channelWatch.begin();i!=au->soundcache->channelWatch.end();i++){
			NONS_SoundEffect *ch=*i;
			if (!ch || !ch->sound)
				continue;
			NONS_SaveFile::Channel *cha=new NONS_SaveFile::Channel();
			cha->name=copyWString(ch->sound->name);
			cha->loop=!!ch->loops;
			if (ch->channel>=ch->channel)
				this->saveGame->channels.resize(ch->channel+1,0);
			this->saveGame->channels[ch->channel]=cha;
		}
		SDL_UnlockMutex(au->soundcache->mutex);
	}
	char *path=save_directory;
	char *filename=addStrings(path,"save01.dat");
	sprintf(filename,"%ssave%d.dat",path,file);
	bool ret=this->saveGame->save(filename);
	delete[] filename;
	//Also save user data
	this->store->saveData();
	ImageLoader->filelog.writeOut();
	return ret;
}
#endif
