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
	//this line makes sure that 'filename' has enough space for sprintf()
	std::string path=save_directory+"save01.dat";
	sprintf(&path[0],"%ssave%d.dat",save_directory.c_str(),file);
	NONS_SaveFile *save=new NONS_SaveFile;
	save->load(path);
	if (save->error!=NONS_NO_ERROR){
		delete save;
		return 0;
	}
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
		NONS_StackElement *p=this->callStack.back();
		delete p;
		this->callStack.pop_back();
	}
	for (ulong a=0;a<save->stack.size();a++){
		NONS_SaveFile::stackEl *el=save->stack[a];
		NONS_StackElement *push;
		if (!el->type)
			push=new NONS_StackElement(this->script->offsetFromBlock(el->label)+el->offset,0,el->textgosubLevel);
		else{
			push=new NONS_StackElement(
				this->store->retrieve(el->variable,0)->intValue,
				this->script->offsetFromBlock(el->label)+el->offset,
				0,el->to,el->step,el->textgosubLevel);
		}
		this->callStack.push_back(push);
	}
	this->interpreter_position=this->script->offsetFromBlock(save->currentLabel)+save->currentOffset;
	this->saveGame->currentLabel=save->currentLabel;
	//variables
	variables_map_T::iterator first=this->store->variables.begin(),last=first;
	if (first->first<200){
		for (;last!=this->store->variables.end() && last->first<200;last++)
			delete last->second;
		this->store->variables.erase(first,last);
	}
	for (variables_map_T::iterator i=save->variables.begin();i!=save->variables.end();i++){
		NONS_Variable *var=i->second;
		NONS_Variable *dst=this->store->retrieve(i->first,0);
		(*dst)=(*var);
	}
	for (arrays_map_T::iterator i=this->store->arrays.begin();i!=this->store->arrays.end();i++)
		delete i->second;
	this->store->arrays.clear();
	for (arrays_map_T::iterator i=save->arrays.begin();i!=save->arrays.end();i++)
		this->store->arrays[i->first]=new NONS_VariableMember(*(i->second));
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
	out->transition->rule=save->windowTransitionRule;
	this->hideTextDuringEffect=save->hideWindow;
	out->foregroundLayer->fontCache->foreground=save->windowTextColor;
	out->display_speed=save->textSpeed;
	this->main_font->spacing=save->spacing;
	this->main_font->lineSkip=save->lineSkip;
	out->log.clear();
	for (ulong a=0;a<save->logPages.size();a++)
		out->log.push_back(save->logPages[a]);
	out->currentBuffer=save->currentBuffer;
	out->x=save->textX;
	out->y=save->textY;
	if (this->arrowCursor)
		delete this->arrowCursor;
	if (save->arrowCursorString.size())
		this->arrowCursor=0;
	else
		this->arrowCursor=new NONS_Cursor(
			save->arrowCursorString,
			save->arrowCursorX,
			save->arrowCursorY,
			save->arrowCursorAbs,
			this->everything->screen);
	if (!save->pageCursorString.size()){
		if (this->pageCursor)
			delete this->pageCursor;
		this->pageCursor=0;
	}else{
		if (this->pageCursor)
			delete this->pageCursor;
		this->pageCursor=new NONS_Cursor(
			save->pageCursorString,
			save->pageCursorX,
			save->pageCursorY,
			save->pageCursorAbs,
			this->everything->screen);
	}
	//graphics
	if (save->background.size()){
		if (!scr->Background)
			scr->Background=new NONS_Layer(&save->background);
		else
			scr->Background->load(&save->background);
	}else{
		if (!scr->Background){
			unsigned rgb=(save->bgColor.r<<rshift)|(save->bgColor.g<<gshift)|(save->bgColor.b<<bshift);
			scr->Background=new NONS_Layer(&scr->screen->inRect,rgb);
		}else{
			scr->Background->setShade(save->bgColor.r,save->bgColor.g,save->bgColor.b);
			scr->Background->Clear();
		}
	}
	if (save->version>1)
		scr->char_baseline=save->char_baseline;
	scr->leftChar->unload();
	if (save->leftChar.size()){
		if (!scr->leftChar)
			scr->leftChar=new NONS_Layer(&save->leftChar);
		else
			scr->leftChar->load(&save->leftChar);
		scr->leftChar->centerAround(scr->screen->virtualScreen->w/4);
		scr->leftChar->useBaseline(scr->char_baseline);
	}
	scr->rightChar->unload();
	if (save->rightChar.size()){
		if (!scr->rightChar)
			scr->rightChar=new NONS_Layer(&save->rightChar);
		else
			scr->rightChar->load(&save->rightChar);
		scr->rightChar->centerAround(scr->screen->virtualScreen->w/4*3);
		scr->rightChar->useBaseline(scr->char_baseline);
	}
	scr->centerChar->unload();
	if (save->centerChar.size()){
		if (!scr->centerChar)
			scr->centerChar=new NONS_Layer(&save->centerChar);
		else
			scr->centerChar->load(&save->centerChar);
		scr->centerChar->centerAround(scr->screen->virtualScreen->w/2);
		scr->centerChar->useBaseline(scr->char_baseline);
	}
	for (ulong a=0;a<scr->layerStack.size();a++){
		if (!scr->layerStack[a])
			continue;
		scr->layerStack[a]->unload();
	}
	for (ulong a=0;a<save->sprites.size();a++){
		NONS_SaveFile::Sprite *spr=save->sprites[a];
		if (spr)
			scr->loadSprite(a,spr->string,spr->x,spr->y,0xFF,spr->visibility);
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
	scr->BlendNoCursor(10,1000,0);
	scr->showText();
	//audio
	if (save->musicTrack>=0){
		char temp[12];
		sprintf(temp,"track%02u",save->musicTrack);
		au->playMusic(&std::string(temp),save->loopMp3?-1:0);
	}else if (save->music.size()){
		ulong size;
		char *buffer=(char *)this->everything->archive->getFileBuffer(save->music,size);
		if (buffer)
			au->playMusic(UniToISO88591(save->music),buffer,size,save->loopMp3?-1:0);
	}
	au->musicVolume(save->musicVolume);
	for (ushort a=0;a<save->channels.size();a++){
		NONS_SaveFile::Channel *c=save->channels[a];
		if (!c->name.size())
			continue;
		if (au->bufferIsLoaded(c->name))
			au->playSoundAsync(&c->name,0,0,a,c->loop?-1:0);
		else{
			ulong size;
			char *buffer=(char *)this->everything->archive->getFileBuffer(c->name,size);
			if (!!buffer)
				this->everything->audio->playSoundAsync(&c->name,buffer,size,a,c->loop);
		}
	}
	delete save;
	return 1;
}

bool NONS_ScriptInterpreter::save(int file){
	for (ulong a=0;a<this->saveGame->stack.size();a++)
		delete this->saveGame->stack[a];
	this->saveGame->stack.clear();
	for (variables_map_T::iterator i=this->saveGame->variables.begin();i!=this->saveGame->variables.end();i++)
		delete i->second;
	this->saveGame->variables.clear();
	for (arrays_map_T::iterator i=this->saveGame->arrays.begin();i!=this->saveGame->arrays.end();i++)
		delete i->second;
	this->saveGame->arrays.clear();
	this->saveGame->logPages.clear();
	for (ulong a=0;a<this->saveGame->sprites.size();a++)
		if (this->saveGame->sprites[a])
			delete this->saveGame->sprites[a];
	this->saveGame->sprites.clear();
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
			el->label=this->script->blockFromOffset(el0->offset);
			el->offset=el0->offset-this->script->offsetFromBlock(el->label);
			el->textgosubLevel=el0->textgosubLevel;
			if (!el->type){
				el->leftovers=el0->first_interpret_string;
			}else{
				el->variable=0;
				for (variables_map_T::iterator i=this->store->variables.begin();i!=this->store->variables.end() && !el->variable;i++)
					if (i->second->intValue==el0->var)
						el->variable=i->first;
				el->to=el0->to;
				el->step=el0->step;
			}
			this->saveGame->stack.push_back(el);
		}
		this->saveGame->currentLabel=this->script->blockFromOffset(this->previous_interpreter_position);
		if (this->saveGame->currentLabel.size())
			this->saveGame->currentOffset=this->previous_interpreter_position-this->script->offsetFromBlock(this->saveGame->currentLabel);
		else
			this->saveGame->currentOffset=0;
	}
	//variables
	{
		variables_map_T *varStack=&this->store->variables;
		for (variables_map_T::iterator i=varStack->begin();i!=varStack->end() && i->first<200;i++)
			if (!VARIABLE_HAS_NO_DATA(i->second))
				this->saveGame->variables[i->first]=new NONS_Variable(*i->second);;
		for (arrays_map_T::iterator i=this->store->arrays.begin();
				i!=this->store->arrays.end();i++){
			this->saveGame->arrays[i->first]=new NONS_VariableMember(*(i->second));
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
		this->saveGame->windowTransitionRule=out->transition->rule;
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
		if (!this->arrowCursor || !this->arrowCursor->data)
			this->saveGame->arrowCursorString.clear();
		else{
			this->saveGame->arrowCursorString=this->arrowCursor->data->animation.getString();
			this->saveGame->arrowCursorX=this->arrowCursor->xpos;
			this->saveGame->arrowCursorY=this->arrowCursor->ypos;
			this->saveGame->arrowCursorAbs=this->arrowCursor->absolute;
		}
		if (!this->pageCursor || !this->pageCursor->data)
			this->saveGame->pageCursorString.clear();
		else{
			this->saveGame->pageCursorString=this->pageCursor->data->animation.getString();
			this->saveGame->pageCursorX=this->pageCursor->xpos;
			this->saveGame->pageCursorY=this->pageCursor->ypos;
			this->saveGame->pageCursorAbs=this->pageCursor->absolute;
		}
		//graphic
		{
			NONS_Image *i=ImageLoader->elementFromSurface(scr->Background->data);
			if (i){
				this->saveGame->background=i->animation.getString();
			}else{
				this->saveGame->background.clear();
				color=scr->Background->defaultShade;
				this->saveGame->bgColor.r=(color>>rshift)&0xFF;
				this->saveGame->bgColor.g=(color>>gshift)&0xFF;
				this->saveGame->bgColor.b=(color>>bshift)&0xFF;
			}
		}
		this->saveGame->char_baseline=scr->char_baseline;
		{
			NONS_Image *i=!!scr->leftChar?ImageLoader->elementFromSurface(scr->leftChar->data):0;
			if (i)
				this->saveGame->leftChar=i->animation.getString();
			else
				this->saveGame->leftChar.clear();
		}
		{
			NONS_Image *i=!!scr->centerChar?ImageLoader->elementFromSurface(scr->centerChar->data):0;
			if (i)
				this->saveGame->centerChar=i->animation.getString();
			else
				this->saveGame->centerChar.clear();
		}
		{
			NONS_Image *i=!!scr->rightChar?ImageLoader->elementFromSurface(scr->rightChar->data):0;
			if (i)
				this->saveGame->rightChar=i->animation.getString();
			else
				this->saveGame->rightChar.clear();
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
						spr->string=i->animation.getString();
						this->saveGame->sprites[a]=spr;
						b=spr;
					}else
						delete spr;
				}
				if (b){
					b->x=c->position.x;
					b->y=c->position.y;
					b->visibility=c->visible;
					b->alpha=c->alpha;
				}else
					o_stderr <<"NONS_ScriptInterpreter::save(): unresolvable inconsistent internal state.\n";
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
			this->saveGame->music.clear();
		}else
			this->saveGame->loopMp3=this->mp3_loop;
		int vol=au->musicVolume(-1);
		this->saveGame->musicVolume=vol<0?100:vol;
		if (au->isInitialized()){
			SDL_LockMutex(au->soundcache->mutex);
			for (std::list<NONS_SoundEffect *>::iterator i=au->soundcache->channelWatch.begin();i!=au->soundcache->channelWatch.end();i++){
				NONS_SoundEffect *ch=*i;
				if (!ch || !ch->sound || !ch->isplaying)
					continue;
				NONS_SaveFile::Channel *cha=new NONS_SaveFile::Channel();
				cha->name=ch->sound->name;
				cha->loop=!!ch->loops;
				if (ch->channel>=ch->channel)
					this->saveGame->channels.resize(ch->channel+1,0);
				this->saveGame->channels[ch->channel]=cha;
			}
			SDL_UnlockMutex(au->soundcache->mutex);
		}
	}
	std::string path=save_directory+"save01.dat";
	sprintf(&path[0],"%ssave%d.dat",save_directory,file);
	bool ret=this->saveGame->save(path);
	//Also save user data
	this->store->saveData();
	ImageLoader->filelog.writeOut();
	return ret;
}
#endif
