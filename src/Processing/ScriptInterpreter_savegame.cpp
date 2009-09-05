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
#include <iomanip>

ErrorCode NONS_ScriptInterpreter::load(int file){
	NONS_SaveFile save;
	save.load(save_directory+L"save"+itoa<wchar_t>(file)+L".dat");
	if (save.error!=NONS_NO_ERROR)
		return NONS_NO_SUCH_SAVEGAME;
	//**********************************************************************
	//NONS save file
	//**********************************************************************
	if (save.version>NONS_SAVEFILE_VERSION)
		return NONS_UNSUPPORTED_SAVEGAME_VERSION;
	for (ulong a=0;a<5;a++)
		if (save.hash[a]!=this->script->hash[a])
			return NONS_HASH_DOES_NOT_MATCH;
	//stack
	//flush
	while (!this->callStack.empty()){
		NONS_StackElement *p=this->callStack.back();
		delete p;
		this->callStack.pop_back();
	}
	for (ulong a=0;a<save.stack.size();a++){
		NONS_SaveFile::stackEl *el=save.stack[a];
		NONS_StackElement *push;
		std::pair<ulong,ulong> pair(this->script->blockFromLabel(el->label)->first_line+el->linesBelow,el->statementNo);
		switch (el->type){
			case SUBROUTINE_CALL:
				push=new NONS_StackElement(
					pair,
					NONS_ScriptLine(0,el->leftovers,0,1),
					0,
					el->textgosubLevel);
				break;
			case FOR_NEST:
				push=new NONS_StackElement(
					this->store->retrieve(el->variable,0)->intValue,
					pair,
					0,
					el->to,
					el->step,
					el->textgosubLevel);
				break;
			//To be implemented in the future:
			/*case TEXTGOSUB_CALL:
				push=new NONS_StackElement(el->pages,el->trigger,el->textgosubLevel);*/
			case USERCMD_CALL:
				push=new NONS_StackElement(
					pair,
					NONS_ScriptLine(0,el->leftovers,0,1),
					0,
					el->textgosubLevel);
				{
					NONS_StackElement *temp=new NONS_StackElement(push,el->parameters);
					delete push;
					push=temp;
				}
		}
		this->callStack.push_back(push);
	}
	std::pair<ulong,ulong> pair(this->script->blockFromLabel(save.currentLabel)->first_line+save.linesBelow,save.statementNo);
	this->thread->gotoPair(pair);
	this->saveGame->currentLabel=save.currentLabel;
	this->loadgosub=save.loadgosub;
	//variables
	variables_map_T::iterator first=this->store->variables.begin(),last=first;
	if (first->first<200){
		for (;last!=this->store->variables.end() && last->first<200;last++)
			delete last->second;
		this->store->variables.erase(first,last);
	}
	for (variables_map_T::iterator i=save.variables.begin();i!=save.variables.end();i++){
		NONS_Variable *var=i->second;
		NONS_Variable *dst=this->store->retrieve(i->first,0);
		(*dst)=(*var);
	}
	for (arrays_map_T::iterator i=this->store->arrays.begin();i!=this->store->arrays.end();i++)
		delete i->second;
	this->store->arrays.clear();
	for (arrays_map_T::iterator i=save.arrays.begin();i!=save.arrays.end();i++)
		this->store->arrays[i->first]=new NONS_VariableMember(*(i->second));
	//screen
	//window
	NONS_ScreenSpace *scr=this->everything->screen;
	//this->main_font=new NONS_Font("default.ttf",save.fontSize,TTF_STYLE_NORMAL);
	INIT_NONS_FONT(this->main_font,save.fontSize,this->everything->archive)
	scr->resetParameters(&save.textWindow,&save.windowFrame,this->main_font,save.fontShadow);
	NONS_StandardOutput *out=scr->output;
	/*out->shadeLayer->clip_rect=save.textWindow;
	out->x0=save.windowFrame.x;
	out->y0=save.windowFrame.y;
	out->w=save.windowFrame.w;
	out->h=save.windowFrame.h;*/
	out->shadeLayer->setShade(save.windowColor.r,save.windowColor.g,save.windowColor.b);
	out->shadeLayer->Clear();
	out->transition->effect=save.windowTransition;
	out->transition->duration=save.windowTransitionDuration;
	out->transition->rule=save.windowTransitionRule;
	this->hideTextDuringEffect=save.hideWindow;
	out->foregroundLayer->fontCache->foreground=save.windowTextColor;
	out->display_speed=save.textSpeed;
	if (save.version>2){
		out->shadowPosX=save.shadowPosX;
		out->shadowPosY=save.shadowPosY;
	}else{
		out->shadowPosX=1;
		out->shadowPosY=1;
	}
	this->main_font->spacing=save.spacing;
	this->main_font->lineSkip=save.lineSkip;
	out->log.clear();
	for (ulong a=0;a<save.logPages.size();a++)
		out->log.push_back(save.logPages[a]);
	out->currentBuffer=save.currentBuffer;
	out->indentationLevel=save.indentationLevel;
	out->x=save.textX;
	out->y=save.textY;
	if (this->arrowCursor)
		delete this->arrowCursor;
	if (this->pageCursor)
		delete this->pageCursor;
	if (!save.arrow.string.size())
		//this->arrowCursor=new NONS_Cursor(this->everything->screen);
		this->arrowCursor=new NONS_Cursor(L":l/3,160,2;cursor0.bmp",0,0,0,this->everything->screen);
	else
		this->arrowCursor=new NONS_Cursor(
			save.arrow.string,
			save.arrow.x,
			save.arrow.y,
			save.arrow.absolute,
			this->everything->screen);
	if (!save.page.string.size())
		//this->pageCursor=new NONS_Cursor(this->everything->screen);
		this->pageCursor=new NONS_Cursor(L":l/3,160,2;cursor1.bmp",0,0,0,this->everything->screen);
	else
		this->pageCursor=new NONS_Cursor(
			save.page.string,
			save.page.x,
			save.page.y,
			save.page.absolute,
			this->everything->screen);
	//graphics
	if (save.background.size()){
		if (!scr->Background)
			scr->Background=new NONS_Layer(&save.background);
		else
			scr->Background->load(&save.background);
	}else{
		if (!scr->Background){
			unsigned rgb=(save.bgColor.r<<rshift)|(save.bgColor.g<<gshift)|(save.bgColor.b<<bshift);
			scr->Background=new NONS_Layer(&scr->screen->inRect,rgb);
		}else{
			scr->Background->setShade(save.bgColor.r,save.bgColor.g,save.bgColor.b);
			scr->Background->Clear();
		}
	}
	if (save.version>1)
		scr->char_baseline=save.char_baseline;
	else
		scr->char_baseline=scr->screenBuffer->clip_rect.h-1;
	NONS_Layer **characters[]={&scr->leftChar,&scr->centerChar,&scr->rightChar};
	for (int a=0;a<3;a++){
		(*characters[a])->unload();
		if (!save.characters[a].string.size())
			continue;
		if (!*characters[a])
			*characters[a]=new NONS_Layer(&save.characters[a].string);
		else
			(*characters[a])->load(&save.characters[a].string);
		(*characters[a])->position.x=(Sint16)save.characters[a].x;
		(*characters[a])->position.y=(Sint16)save.characters[a].y;
		(*characters[a])->visible=save.characters[a].visibility;
		(*characters[a])->alpha=save.characters[a].alpha;
		if ((*characters[a])->animated())
			(*characters[a])->animation.animation_time_offset=save.characters[a].animOffset;
	}
	if (save.version>2){
		scr->charactersBlendOrder.clear();
		for (ulong a=0;a<3 && save.charactersBlendOrder[a]!=255;a++)
			scr->charactersBlendOrder.push_back(save.charactersBlendOrder[a]);
	}

	scr->blendSprites=save.blendSprites;
	for (ulong a=0;a<scr->layerStack.size();a++){
		if (!scr->layerStack[a])
			continue;
		scr->layerStack[a]->unload();
	}
	for (ulong a=0;a<save.sprites.size();a++){
		NONS_SaveFile::Sprite *spr=save.sprites[a];
		if (spr)
			scr->loadSprite(a,spr->string,spr->x,spr->y,0xFF,spr->visibility);
	}
	scr->sprite_priority=save.spritePriority;
	if (save.monochrome){
		if (!scr->monochrome)
			scr->monochrome=new NONS_GFX();
		scr->monochrome->type=POSTPROCESSING;
		scr->monochrome->color=save.monochromeColor;
		scr->monochrome->effect=0;
	}else if (!!scr->monochrome){
		delete scr->monochrome;
		scr->monochrome=0;
	}
	if (save.negative){
		if (!scr->negative)
			scr->negative=new NONS_GFX();
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
		SDL_Surface *srf=makeSurface(scr->screen->virtualScreen->w,scr->screen->virtualScreen->h,32);
		SDL_FillRect(srf,0,amask);
		NONS_GFX::callEffect(10,1000,0,srf,0,scr->screen);
		SDL_FreeSurface(srf);
	}
	SDL_Delay(1500);
	scr->BlendNoCursor(10,1000,0);
	scr->showText();
	//audio
	if (save.musicTrack>=0){
		std::wstring temp=L"track";
		temp+=itoa<wchar_t>(save.musicTrack,2);
		au->playMusic(&temp,save.loopMp3?-1:0);
	}else if (save.music.size()){
		ulong size;
		char *buffer=(char *)this->everything->archive->getFileBuffer(save.music,size);
		if (buffer)
			au->playMusic(save.music,buffer,size,save.loopMp3?-1:0);
	}
	au->musicVolume(save.musicVolume);
	for (ushort a=0;a<save.channels.size();a++){
		NONS_SaveFile::Channel *c=save.channels[a];
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
	if (this->loadgosub.size())
		this->gosub_label(this->loadgosub);
	return NONS_NO_ERROR;
}

bool NONS_ScriptInterpreter::save(int file){
	if (this->insideTextgosub())
		return 0;
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
			el->type=el0->type;
			NONS_ScriptBlock *block=this->script->blockFromLine(el0->returnTo.line);
			el->label=block->name;
			el->linesBelow=el0->returnTo.line-block->first_line;
			el->statementNo=el0->returnTo.statement;
			el->textgosubLevel=el0->textgosubLevel;
			switch (el->type){
				case SUBROUTINE_CALL:
					el->leftovers=el0->interpretAtReturn.toString();
					break;
				case FOR_NEST:
					el->variable=0;
					for (variables_map_T::iterator i=this->store->variables.begin();i!=this->store->variables.end() && !el->variable;i++)
						if (i->second->intValue==el0->var)
							el->variable=i->first;
					el->to=el0->to;
					el->step=el0->step;
					break;
				/*case TEXTGOSUB_CALL:
					break;*/
				case USERCMD_CALL:
					el->leftovers=el0->interpretAtReturn.toString();
					el->parameters=el0->parameters;
					break;
			}
			this->saveGame->stack.push_back(el);
		}
		{
			const NONS_ScriptBlock *block=this->thread->currentBlock;
			this->saveGame->currentLabel=block->name;
			NONS_Statement *stmt=this->thread->getCurrentStatement();
			this->saveGame->linesBelow=stmt->lineOfOrigin->lineNumber-block->first_line;
			this->saveGame->statementNo=stmt->statementNo;
		}
		this->saveGame->loadgosub=this->loadgosub;
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
		this->saveGame->shadowPosX=out->shadowPosX;
		this->saveGame->shadowPosY=out->shadowPosY;
		this->saveGame->spacing=this->main_font->spacing;
		this->saveGame->lineSkip=this->main_font->lineSkip;
		for (ulong a=0;a<out->log.size();a++)
			this->saveGame->logPages.push_back(out->log[a]);
		this->saveGame->currentBuffer=out->currentBuffer;
		this->saveGame->indentationLevel=out->indentationLevel;
		if (!this->arrowCursor || !this->arrowCursor->data)
			this->saveGame->arrow.string.clear();
		else{
			this->saveGame->arrow.string=this->arrowCursor->data->animation.getString();
			this->saveGame->arrow.x=this->arrowCursor->xpos;
			this->saveGame->arrow.y=this->arrowCursor->ypos;
			this->saveGame->arrow.absolute=this->arrowCursor->absolute;
		}
		if (!this->pageCursor || !this->pageCursor->data)
			this->saveGame->page.string.clear();
		else{
			this->saveGame->page.string=this->pageCursor->data->animation.getString();
			this->saveGame->page.x=this->pageCursor->xpos;
			this->saveGame->page.y=this->pageCursor->ypos;
			this->saveGame->page.absolute=this->pageCursor->absolute;
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
		NONS_Layer **characters[]={&scr->leftChar,&scr->centerChar,&scr->rightChar};
		for (int a=0;a<3;a++){
			if (!!*characters[a] && !!(*characters[a])->data){
				this->saveGame->characters[a].string=(*characters[a])->animation.getString();
				this->saveGame->characters[a].x=(*characters[a])->position.x;
				this->saveGame->characters[a].y=(*characters[a])->position.y;
				this->saveGame->characters[a].visibility=(*characters[a])->visible;
				this->saveGame->characters[a].alpha=(*characters[a])->alpha;
			}else
				this->saveGame->characters[a].string.clear();
		}
		std::copy(scr->charactersBlendOrder.begin(),scr->charactersBlendOrder.end(),this->saveGame->charactersBlendOrder);
		std::fill(this->saveGame->charactersBlendOrder+scr->charactersBlendOrder.size(),this->saveGame->charactersBlendOrder+3,255);
		//update sprite record
		this->saveGame->blendSprites=scr->blendSprites;
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
	bool ret=this->saveGame->save(save_directory+L"save"+itoa<wchar_t>(file)+L".dat");
	//Also save user data
	this->store->saveData();
	ImageLoader->filelog.writeOut();
	return ret;
}
#endif
