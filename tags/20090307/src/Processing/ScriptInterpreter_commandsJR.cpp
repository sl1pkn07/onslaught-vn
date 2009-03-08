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

#include "ScriptInterpreter.h"
#include "../Functions.h"
#include "../Globals.h"
#include "../IO_System/FileIO.h"
#include "../IO_System/IOFunctions.h"

#ifndef NONS_SCRIPTINTERPRETER_COMMANDSJR_CPP
#define NONS_SCRIPTINTERPRETER_COMMANDSJR_CPP
ErrorCode NONS_ScriptInterpreter::command_nsa(NONS_ParsedLine &line){
	this->errored_lines.insert(this->current_line);
	/*if (!wcscmp(line.line,L"arc")){
		if (line.parameters.size()<1){
			ErrorCode error=this->everything->archive->init("arc.sar",0,1);
			if ((error&NONS_NO_ERROR_FLAG)!=NONS_NO_ERROR_FLAG)
				return this->everything->archive->init("ARC.SAR",0,0);
			return error;
		}
		char *name=copyString(line.parameters[0]);
		ErrorCode ret=this->everything->archive->init(name,0,0);
		delete[] name;
		return ret;
	}else{
		const char *filenames[]={
			"arc.nsa",
			"ARC.NSA",
			"arc1.nsa",
			"ARC1.NSA",
			"arc2.nsa",
			"ARC2.NSA",
			"arc3.nsa",
			"ARC3.NSA",
			"arc4.nsa",
			"ARC4.NSA",
			"arc5.nsa",
			"ARC5.NSA",
			"arc6.nsa",
			"ARC6.NSA",
			"arc7.nsa",
			"ARC7.NSA",
			"arc8.nsa",
			"ARC8.NSA",
			"arc9.nsa",
			"ARC9.NSA",
			0
		};
		for (short a=0;filenames[a];a++)
			this->everything->archive->init(filenames[a],1,1);*/
		return NONS_NO_ERROR;
	//}
}

ErrorCode NONS_ScriptInterpreter::command_nsadir(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	delete[] this->nsadir;
	this->nsadir=copyString(line.parameters[0]);
	if (this->nsadir[strlen(this->nsadir)-1]!='/')
		addStringsInplace(&this->nsadir,"/");
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_return(NONS_ParsedLine &line){
	if (this->callStack.empty())
		return NONS_EMPTY_CALL_STACK;
	NONS_StackElement *popped;
	do{
		popped=*(this->callStack.end()-1);
		this->callStack.pop_back();
	}while (popped->type!=SUBROUTINE_CALL);
	this->interpreter_position=popped->offset;
	if (!popped->first_interpret_string){
		delete popped;
		return NONS_NO_ERROR_BUT_BREAK;
	}
	wchar_t *str=copyWString(popped->first_interpret_string);
	delete popped;
	if (instr(str,L":")<0){
		ErrorCode error=this->interpretString(str);
		delete str;
		return error;
	}
	long len=wcslen(str);
	ulong commandstart=0;
	for (ulong a=0;a<len;a++){
		if (str[a]=='\"' || str[a]=='`'){
			for (wchar_t quote=str[a];str[a] && str[a]!=quote;a++);
			if (!str[a]){
				delete str;
				return NONS_UNMATCHED_QUOTES;
			}
			a++;
			continue;
		}
		if (str[a]==':' || str[a]==';'){
			wchar_t *copy=copyWString(str+commandstart,a-commandstart);
			ErrorCode error=this->interpretString(copy);
			delete[] copy;
			if (error==NONS_GOSUB && str[a+1])
				(*(this->callStack.end()-1))->first_interpret_string=copyWString(str+a+1);
			if (!CHECK_FLAG(error,NONS_NO_ERROR_FLAG))
				handleErrors(error,-1,"NONS_ScriptInterpreter::command_return");
			if (CHECK_FLAG(error,NONS_BREAK_WORTHY_ERROR) || str[a]==';'){
				delete[] str;
				return NONS_NO_ERROR_BUT_BREAK;
			}
			commandstart=a+1;
		}
	}
	wchar_t *copy=copyWString(str+commandstart);
	ErrorCode error=this->interpretString(copy);
	if (!CHECK_FLAG(error,NONS_NO_ERROR_FLAG))
		handleErrors(error,-1,"NONS_ScriptInterpreter::command_return");
	delete[] copy;
	delete[] str;
	return NONS_NO_ERROR_BUT_BREAK;
}

ErrorCode NONS_ScriptInterpreter::command_mov(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *var;
	_GETVARIABLE(var,0,)
	if (var->getType()=='%'){
		long val;
		_GETINTVALUE(val,1,)
		var->set(val);
	}else{
		wchar_t *val=0;
		_GETWCSVALUE(val,1,)
		var->set(val,1);
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_len(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	wchar_t *src=0;
	_GETWCSVALUE(src,1,)
	dst->set(wcslen(src));
	delete[] src;
	return NONS_NO_ERROR;
}

/*
Behavior notes:
rnd %a,%n ;a=[0;n)
rnd2 %a,%min,%max ;a=[min;max]
*/
ErrorCode NONS_ScriptInterpreter::command_rnd(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	long min=0,max;
	if (!wcscmp(line.line,L"rnd")){
		_GETINTVALUE(max,1,)
		max--;
	}else{
		if (line.parameters.size()<3)
			return NONS_INSUFFICIENT_PARAMETERS;
		_GETINTVALUE(max,2,)
		_GETINTVALUE(min,1,)
	}
	//lower+int(double(upper-lower+1)*rand()/(RAND_MAX+1.0))
	dst->set(min+(rand()*(max-min))/RAND_MAX);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_play(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	wchar_t *name=0;
	_GETWCSVALUE(name,0,)
	ErrorCode error=NONS_UNDEFINED_ERROR;
	this->mp3_loop=0;
	this->mp3_save=0;
	if (!wcscmp(line.line,L"play") || !wcscmp(line.line,L"loopbgm"))
		this->mp3_loop=1;
	else if (!wcscmp(line.line,L"mp3save"))
		this->mp3_save=1;
	else if (!wcscmp(line.line,L"mp3loop") || !wcscmp(line.line,L"bgm")){
		this->mp3_loop=1;
		this->mp3_save=1;
	}/*else if (!wcscmp(line.line,L"playonce") || !wcscmp(line.line,L"mp3"))
		times=1;*/
	if (*name=='*'){
		char temp[12];
		char *copy=copyString(name+1);
		/*if (!isdigit(copy)){
			delete[] name;
			delete[] copy;
			return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
		}*/
		int track=atoi(copy);
		delete[] copy;

		sprintf(temp,"track%02u",track);
		error=this->everything->audio->playMusic(temp,this->mp3_loop?-1:0);
		if (error==NONS_NO_ERROR)
			this->saveGame->musicTrack=track;
		else
			this->saveGame->musicTrack=-1;
	}else{
		ulong size;
		char *buffer=(char *)this->everything->archive->getFileBuffer(name,&size);
		this->saveGame->musicTrack=-1;
		//error=!buffer?NONS_FILE_NOT_FOUND:this->everything->audio->playMusic(copy,buffer,size,this->mp3_loop?-1:0);
		if (!buffer)
			error=NONS_FILE_NOT_FOUND;
		else
			error=this->everything->audio->playMusic(name,buffer,size,this->mp3_loop?-1:0);
		if (this->saveGame->music)
			delete[] this->saveGame->music;
		if (error==NONS_NO_ERROR)
			this->saveGame->music=copyWString(name);
		else
			this->saveGame->music=0;
	}
	delete[] name;
	return error;
}

ErrorCode NONS_ScriptInterpreter::command_playstop(NONS_ParsedLine &line){
	this->mp3_loop=0;
	this->mp3_save=0;
	return this->everything->audio->stopMusic();
}

ErrorCode NONS_ScriptInterpreter::command_mp3vol(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long vol;
	_GETINTVALUE(vol,0,)
	this->everything->audio->musicVolume(vol);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_mid(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	long start,len0,len;
	_GETINTVALUE(start,2,)
	wchar_t *src=0;
	_GETWCSVALUE(src,1,)
	len0=wcslen(src);
	len=len0;
	if (start>=len0){
		delete[] src;
		dst->set(L"",0);
		return NONS_NO_ERROR;
	}
	if (line.parameters.size()>3){
		_GETINTVALUE(len,3,delete[] src;)
	}
	if (start+len>len0)
		len=len0-start;
	dst->set(copyWString(src+start,len),1);
	delete[] src;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_onslaught_language_extensions(NONS_ParsedLine &line){
	if (!wcscmp(line.line,L"enable_onslaught_language_extensions"))
		this->language_extensions=1;
	else
		this->language_extensions=0;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_movl(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *dst;
	_HANDLE_POSSIBLE_ERRORS(this->store->resolveIndexing(line.parameters[0],&dst),)
	if (dst->getType()!='?')
		return NONS_EXPECTED_ARRAY;
	if (line.parameters.size()-1>dst->dimensionSize)
		handleErrors(NONS_TOO_MANY_PARAMETERS,line.lineNo,"NONS_ScriptInterpreter::command_movl");
	for (ulong a=0;a<dst->dimensionSize && a<line.parameters.size()-1;a++){
		long temp;
		_GETINTVALUE(temp,a+1,)
		dst->dimension[a]->set(temp);
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_jumpf(NONS_ParsedLine &line){
	std::vector<ulong> &jumps=this->everything->script->jumps;
	if (!jumps.size())
		return NONS_NO_JUMPS;
	if (!wcscmp(line.line,L"jumpb")){
		for (long a=jumps.size()-1;a>=0;a--){
			if (jumps[a]<this->interpreter_position){
				this->interpreter_position=jumps[a];
				return NONS_NO_ERROR_BUT_BREAK;
			}
		}
	}else{
		for (ulong a=0;a<jumps.size();a++){
			if (jumps[a]>this->interpreter_position){
				this->interpreter_position=jumps[a];
				return NONS_NO_ERROR_BUT_BREAK;
			}
		}
	}
	return NONS_NO_JUMPS;
}

ErrorCode NONS_ScriptInterpreter::command_mp3fadeout(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long ms;
	_GETINTVALUE(ms,0,)
	if (ms<25){
		this->everything->audio->stopMusic();
		return NONS_NO_ERROR;
	}
	float original_vol=this->everything->audio->musicVolume(-1);
	float advance=original_vol/(float(ms)/25.0);
	float current_vol=original_vol;
	while (current_vol>0){
		SDL_Delay(25);
		current_vol-=advance;
		if (current_vol<0)
			current_vol=0;
		this->everything->audio->musicVolume((int)current_vol);
	}
	_HANDLE_POSSIBLE_ERRORS(this->everything->audio->stopMusic(),)
	this->everything->audio->musicVolume(original_vol);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_mpegplay(NONS_ParsedLine &line){
/*#ifndef __MINGW32__
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	if (!this->everything->screen)
		this->setDefaultWindow();
	wchar_t *temp=0;
	long click;
	_GETWCSVALUE(temp,0,)
	_GETINTVALUE(click,1,)
	char *filename=copyString(temp);
	SDL_RWops *rwops=0;
	if (fileExists(filename))
		rwops=SDL_RWFromFile(filename,"r");
	else{
		long l;
		char *buffer=(char *)this->everything->archive->getFileBuffer(temp,(ulong *)&l);
		if (!buffer){
			delete[] temp;
			delete[] filename;
			return NONS_FILE_NOT_FOUND;
		}
		rwops=SDL_RWFromMem((char *)buffer,l);
	}
	delete[] temp;
	delete[] filename;
	_HANDLE_POSSIBLE_ERRORS(this->everything->audio->stopAllSound(),)
	SDL_Surface *screen=this->everything->screen->screen->realScreen;
	{
		SMPEG_Info info;
		SMPEG *mpeg=SMPEG_new_rwops(rwops,&info,1);
		SMPEG_enableaudio(mpeg, 1);
		SMPEG_enablevideo(mpeg, 1);
		SMPEG_setvolume(mpeg, 100);
		//Unused:
		//const SDL_VideoInfo *video_info=SDL_GetVideoInfo();
		SMPEG_setdisplay(mpeg, screen, 0, 0);
		SMPEG_scaleXY(mpeg, screen->w, screen->h);
		SMPEG_play(mpeg);
		NONS_EventQueue *queue=InputObserver.attach();
		bool killvideo=0;
		while (SMPEG_status(mpeg)==SMPEG_PLAYING && !killvideo){
			if (click){
				while (!queue->data.empty() && !killvideo){
					SDL_Event event=queue->pop();
					if (event.type==SDL_MOUSEBUTTONDOWN){
						killvideo=1;
						SMPEG_stop(mpeg);
					}
				}
			}
			SDL_Delay(50);
		}
		InputObserver.detach(queue);
		SMPEG_delete(mpeg);
	}
#endif*/
	return NONS_UNIMPLEMENTED_COMMAND;
}

ErrorCode NONS_ScriptInterpreter::command_new_set_window(NONS_ParsedLine &line){
	this->legacy_set_window=0;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_ld(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
	/*if (!this->everything->screen)
		this->setDefaultWindow();*/
	wchar_t *temp=0;
	_GETWCSVALUE(temp,1,)
	long semicolon=instr(temp,L";");
	if (semicolon++<0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	wchar_t *name=copyWString(temp+semicolon);
	delete[] temp;
	NONS_Layer **l=0;
	long off;
	switch (line.parameters[0][0]){
		case 'l':
			l=&(this->everything->screen->leftChar);
			off=(this->everything->screen->screen->virtualScreen->w)/-4;
			break;
		case 'c':
			l=&(this->everything->screen->centerChar);
			off=0;
			break;
		case 'r':
			l=&(this->everything->screen->rightChar);
			off=(this->everything->screen->screen->virtualScreen->w)/4;
			break;
		default:
			return NONS_INVALID_PARAMETER;
	}
	tolower(name);
	toforwardslash(name);
	if (this->hideTextDuringEffect)
		this->everything->screen->hideText();
	if (!*l)
		*l=new NONS_Layer(name,&(this->everything->screen->screen->virtualScreen->clip_rect),CLOptions.layerMethod);
	else{
		if (!(*l)->load(name,&(this->everything->screen->screen->virtualScreen->clip_rect),CLOptions.layerMethod)){
			delete[] name;
			return NONS_FILE_NOT_FOUND;
		}
	}
	if (!(*l)->data)
		return NONS_FILE_NOT_FOUND;
	(*l)->clip_rect.x=off;
	long number,duration;
	wchar_t *rule=0;
	ErrorCode ret;
	_GETINTVALUE(number,2,)
	if (line.parameters.size()>3){
		_GETINTVALUE(duration,3,)
		if (line.parameters.size()>4)
			_GETWCSVALUE(rule,4,)
		ret=this->everything->screen->BlendAll(number,duration,rule);
		if (rule)
			delete[] rule;
	}else
		ret=this->everything->screen->BlendAll(number);
	return ret;
}

ErrorCode NONS_ScriptInterpreter::command_lsp(NONS_ParsedLine &line){
	if (line.parameters.size()<4)
		return NONS_INSUFFICIENT_PARAMETERS;
	long spriten,x,y,alpha=255;
	wchar_t *str=0;
	_GETINTVALUE(spriten,0,)
	_GETINTVALUE(x,2,)
	_GETINTVALUE(y,3,)
	if (line.parameters.size()>4)
		_GETINTVALUE(alpha,4,)
	_GETWCSVALUE(str,1,)
	wchar_t *string=str;
	ErrorCode error=NONS_NO_ERROR;
	METHODS method=NO_ALPHA;
	if (*str==':'){
		str++;
		for (;*str && iswhitespace((char)*str);str++);
		switch (*str){
			case 'a':
				method=CLASSIC_METHOD;
				break;
			case 'l':
			case 'r':
				error=NONS_INVALID_TRANSPARENCY_METHOD;
				break;
			case 'c':
				method=NO_ALPHA;
				break;
			default:
				error=NONS_TRANSPARENCY_METHOD_UNIMPLEMENTED;
				break;
		}
		for (;*str && *str!='/' && *str!=';';str++);
	}
	if (error!=NONS_NO_ERROR){
		delete[] string;
		return error;
	}
	if (*str=='/')
		for (;*str && *str!=';';str++);
	if (*str==';')
		str++;
	str=copyWString(str);
	wchar_t *name=str;
	if (alpha>255)
		alpha=255;
	if (alpha<0)
		alpha=0;
	tolower(name);
	_HANDLE_POSSIBLE_ERRORS(this->everything->screen->loadSprite(spriten,string,name,x,y,alpha,method,!wcscmp(line.line,L"lsp")),delete[] name;);
	delete[] string;
	delete[] name;
	//this->everything->screen->BlendAll();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_literal_print(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	/*if (!this->everything->screen)
		this->setDefaultWindow();*/
	std::wstring string;
	this->convertParametersToString(line,string);
	if (string.size()){
		this->everything->screen->showText();
		if (this->everything->screen->output->prepareForPrinting(string.c_str())){
			if (this->pageCursor->animate(this->everything->screen,this->menu,this->autoclick)<0)
				return NONS_NO_ERROR;
			this->everything->screen->clearText();
		}
		while (this->everything->screen->output->print(0,string.size(),this->everything->screen->screen)){
			if (this->pageCursor){
				if (this->pageCursor->animate(this->everything->screen,this->menu,this->autoclick)<0)
					return NONS_NO_ERROR;
			}else
				waitUntilClick();
			this->everything->screen->clearText();
		}
		this->everything->screen->output->endPrinting();
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_print(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long number,duration;
	wchar_t *rule=0;
	ErrorCode ret;
	_GETINTVALUE(number,0,)
	if (line.parameters.size()>1){
		_GETINTVALUE(duration,1,)
		if (line.parameters.size()>2)
			_GETWCSVALUE(rule,2,)
		ret=this->everything->screen->BlendAll(number,duration,rule);
		if (rule)
			delete[] rule;
	}else
		ret=this->everything->screen->BlendAll(number);
	return ret;
}

ErrorCode NONS_ScriptInterpreter::command_monocro(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long color;
	if (!wcscmp(line.parameters[0],L"off")){
		/*if (!this->everything->screen)
			this->setDefaultWindow();*/
		if (this->everything->screen->monochrome){
			delete this->everything->screen->monochrome;
			this->everything->screen->monochrome=0;
		}
		return NONS_NO_ERROR;
	}
	_GETINTVALUE(color,0,)
	uchar r=(color&0xFF0000)>>16,
		g=(color&0xFF00)>>8,
		b=color&0xFF;
	/*if (!this->everything->screen)
		this->setDefaultWindow();*/

	if (!this->everything->screen->monochrome){
		this->everything->screen->monochrome=new NONS_GFX();
		this->everything->screen->monochrome->type=POSTPROCESSING;
	}
	this->everything->screen->monochrome->effect=0;
	this->everything->screen->monochrome->color.r=r;
	this->everything->screen->monochrome->color.g=g;
	this->everything->screen->monochrome->color.b=b;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_nega(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long onoff;
	_GETINTVALUE(onoff,0,)
	if (onoff){
		/*if (!this->everything->screen)
			this->setDefaultWindow();*/
		if (!this->everything->screen->negative)
			this->everything->screen->negative=new NONS_GFX(onoff==1?1:2,0,0);
		else
			this->everything->screen->negative->effect=onoff==1?1:2;
		this->everything->screen->negative->type=POSTPROCESSING;
	}else{
		/*if (!this->everything->screen)
			this->setDefaultWindow();*/
		if (this->everything->screen->negative){
			delete this->everything->screen->negative;
			this->everything->screen->negative=0;
		}
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_next(NONS_ParsedLine &line){
	if (this->callStack.empty())
		return NONS_EMPTY_CALL_STACK;
	/*std::vector<NONS_StackElement *>::iterator top=;
	top--;*/
	NONS_StackElement *element=*(this->callStack.end()-1);
	if (element->type!=FOR_NEST)
		return NONS_UNEXPECTED_NEXT;
	element->var->add(element->step);
	if (element->step>0 && element->var->getInt()>element->to || element->step<0 && element->var->getInt()<element->to){
		delete element;
		this->callStack.pop_back();
		return NONS_NO_ERROR;
	}
	element->end=this->interpreter_position;
	this->interpreter_position=element->offset;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_locate(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	long x,y;
	_GETINTVALUE(x,0,)
	_GETINTVALUE(y,1,)
	if (x<0 || y<0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	/*if (!this->everything->screen)
		this->setDefaultWindow();*/
	this->everything->screen->output->setPosition(x,y);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_resettimer(NONS_ParsedLine &line){
	this->timer=SDL_GetTicks();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_repaint(NONS_ParsedLine &line){
	if (this->everything->screen)
		this->everything->screen->BlendAll(1);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_rmenu(NONS_ParsedLine &line){
	if (line.parameters.size()<2 || line.parameters.size()%2)
		return NONS_INSUFFICIENT_PARAMETERS;
	std::vector<wchar_t *> items;
	for (ulong a=0;a<line.parameters.size();a++){
		wchar_t *s=0;
		_GETWCSVALUE(s,a,)
		a++;
		items.push_back(s);
		items.push_back(copyWString(line.parameters[a]));
	}
	/*if (!this->everything->screen)
		this->setDefaultWindow();*/
	this->menu->resetStrings(&items);
	this->menu->rightClickMode=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_menusetwindow(NONS_ParsedLine &line){
	if (line.parameters.size()<7)
		return NONS_INSUFFICIENT_PARAMETERS;
	//bold is unused
	long fontX,fontY,spacingX,spacingY,
		//bold,
		shadow,hexcolor;
	_GETINTVALUE(fontX,0,)
	_GETINTVALUE(fontY,1,)
	_GETINTVALUE(spacingX,2,)
	_GETINTVALUE(spacingY,3,)
	_GETINTVALUE(shadow,5,)
	_GETINTVALUE(hexcolor,6,)
	SDL_Color color={(hexcolor&0xFF0000)>>16,(hexcolor&0xFF00)>>8,hexcolor&0xFF,0};
	this->menu->fontsize=fontX;
	this->menu->lineskip=fontY+spacingY;
	this->menu->spacing=spacingX;
	this->menu->shadow=!!shadow;
	this->menu->shadeColor=color;
	this->menu->reset();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_kidokuskip(NONS_ParsedLine &line){
	softwareCtrlIsPressed=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_menuselectcolor(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
	long on,off,nofile;
	_GETINTVALUE(on,0,)
	_GETINTVALUE(off,1,)
	_GETINTVALUE(nofile,2,)
	SDL_Color coloron={(on&0xFF0000)>>16,(on&0xFF00)>>8,on&0xFF,0},
		coloroff={(off&0xFF0000)>>16,(off&0xFF00)>>8,off&0xFF,0},
		colornofile={(nofile&0xFF0000)>>16,(nofile&0xFF00)>>8,nofile&0xFF,0};
	this->menu->on=coloron;
	this->menu->off=coloroff;
	this->menu->nofile=colornofile;
	this->menu->reset();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_menuselectvoice(NONS_ParsedLine &line){
	if (line.parameters.size()<7)
		return NONS_INSUFFICIENT_PARAMETERS;
	wchar_t *entry=0,
		*cancel=0,
		*mouse=0,
		*click=0,
		*yes=0,
		*no=0;
	_GETWCSVALUE(entry,0,)
	_GETWCSVALUE(cancel,1,
		delete[] entry;)
	_GETWCSVALUE(mouse,2,
		delete[] entry;
		delete[] cancel;)
	_GETWCSVALUE(click,3,
		delete[] entry;
		delete[] cancel;
		delete[] mouse;)
	_GETWCSVALUE(yes,5,
		delete[] entry;
		delete[] cancel;
		delete[] mouse;
		delete[] click;)
	_GETWCSVALUE(no,6,
		delete[] entry;
		delete[] cancel;
		delete[] mouse;
		delete[] click;
		delete[] yes;)
	if (!*entry){
		delete[] entry;
		entry=0;
	}
	if (!*cancel){
		delete[] cancel;
		entry=0;
	}
	if (!*mouse){
		delete[] mouse;
		entry=0;
	}
	if (!*click){
		delete[] click;
		entry=0;
	}
	if (!*yes){
		delete[] yes;
		entry=0;
	}
	if (!*no){
		delete[] no;
		entry=0;
	}
	if (this->menu->voiceEntry)
		delete[] this->menu->voiceEntry;
	if (this->menu->voiceCancel)
		delete[] this->menu->voiceCancel;
	if (this->menu->voiceMO)
		delete[] this->menu->voiceMO;
	if (this->menu->voiceClick)
		delete[] this->menu->voiceClick;
	if (this->menu->voiceYes)
		delete[] this->menu->voiceYes;
	if (this->menu->voiceNo)
		delete[] this->menu->voiceNo;
	this->menu->voiceEntry=entry;
	this->menu->voiceCancel=cancel;
	this->menu->voiceMO=mouse;
	this->menu->voiceClick=click;
	this->menu->voiceYes=yes;
	this->menu->voiceNo=no;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_rmode(NONS_ParsedLine &line){
	if (!wcscmp(line.line,L"roff")){
		this->menu->rightClickMode=0;
		return NONS_NO_ERROR;
	}
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long a;
	_GETINTVALUE(a,0,)
	if (!a)
		this->menu->rightClickMode=0;
	else
		this->menu->rightClickMode=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_reset(NONS_ParsedLine &line){
	this->uninit();
	this->init();
	//if (this->everything->screen){
	this->everything->screen->clear();
	delete this->gfx_store;
	this->gfx_store=new NONS_GFXstore();
	this->everything->screen->gfx_store=this->gfx_store;
	//}
	this->everything->audio->stopAllSound();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_msp(NONS_ParsedLine &line){
	if (line.parameters.size()<4)
		return NONS_INSUFFICIENT_PARAMETERS;
	long spriten,x,y,alpha;
	_GETINTVALUE(spriten,0,)
	_GETINTVALUE(x,1,)
	_GETINTVALUE(y,2,)
	_GETINTVALUE(alpha,3,)
	if (spriten>this->everything->screen->layerStack.size())
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	NONS_Layer *l=this->everything->screen->layerStack[spriten];
	if (!l)
		return NONS_NO_SPRITE_LOADED_THERE;
	if (wcscmp(line.line,L"amsp")){
		l->clip_rect.x+=x;
		l->clip_rect.y+=y;
		if (long(l->alpha)+alpha>255)
			l->alpha=255;
		else if (long(l->alpha)+alpha<0)
			l->alpha=0;
		else
			l->alpha+=alpha;
	}else{
		l->clip_rect.x=x;
		l->clip_rect.y=y;
		if (alpha>255)
			alpha=255;
		else if (alpha<0)
			alpha=0;
		l->alpha=alpha;
	}
	return NONS_NO_ERROR;
}

void shake(SDL_Surface *dst,long amplitude,ulong duration){
	SDL_Rect srcrect=dst->clip_rect,
		dstrect=srcrect;
	SDL_Surface *copyDst=SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,dst->w,dst->h,32,rmask,gmask,bmask,amask);
	manualBlit(dst,0,copyDst,0);
	ulong start=SDL_GetTicks();
	SDL_Rect last=dstrect;
	while (SDL_GetTicks()-start<duration){
		SDL_FillRect(dst,&srcrect,0);
		do{
			dstrect.x=(rand()%2)?amplitude:-amplitude;
			dstrect.y=(rand()%2)?amplitude:-amplitude;
		}while (dstrect.x==last.x && dstrect.y==last.y);
		last=dstrect;
		manualBlit(copyDst,&srcrect,dst,&dstrect);
		SDL_UpdateRect(dst,0,0,0,0);
	}
	manualBlit(copyDst,0,dst,0);
	SDL_UpdateRect(dst,0,0,0,0);
	SDL_FreeSurface(copyDst);
}

ErrorCode NONS_ScriptInterpreter::command_quake(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	long amplitude,duration;
	_GETINTVALUE(amplitude,0,)
	_GETINTVALUE(duration,1,)
	if (amplitude<0 || duration<0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	amplitude*=2;
	amplitude=this->everything->screen->screen->convertW(amplitude);
	shake(this->everything->screen->screen->realScreen,amplitude,duration);
	return 0;
}

ErrorCode NONS_ScriptInterpreter::command_lookbackbutton(NONS_ParsedLine &line){
	if (line.parameters.size()<4)
		return NONS_INSUFFICIENT_PARAMETERS;
	wchar_t *A=0,
		*B=0,
		*C=0,
		*D=0;
	_GETWCSVALUE(A,0,)
	_GETWCSVALUE(B,1,)
	_GETWCSVALUE(C,2,)
	_GETWCSVALUE(D,3,)
	bool ret=this->everything->screen->lookback->setUpButtons(A,B,C,D);
	delete[] A;
	delete[] B;
	delete[] C;
	delete[] D;
	return ret?NONS_NO_ERROR:NONS_FILE_NOT_FOUND;
}

ErrorCode NONS_ScriptInterpreter::command_lookbackcolor(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long a;
	_GETINTVALUE(a,0,)
	SDL_Color col={(a&0xFF0000)>>16,(a&0xFF00)>>8,a&0xFF,0};
	this->everything->screen->lookback->foreground=col;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_lookbackflush(NONS_ParsedLine &line){
	this->everything->screen->output->log.clear();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_loadgame(NONS_ParsedLine &line){
	long file;
	_GETINTVALUE(file,0,)
	if (file<1)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	return this->load(file)?NONS_NO_ERROR:NONS_UNDEFINED_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_menu_full(NONS_ParsedLine &line){
	this->everything->screen->screen->toggleFullscreen(!wcscmp(line.line,L"menu_full"));
	return NONS_NO_ERROR;
}

/*ErrorCode NONS_ScriptInterpreter::command_(NONS_ParsedLine &line){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_ParsedLine &line){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_ParsedLine &line){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_ParsedLine &line){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_ParsedLine &line){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_ParsedLine &line){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_ParsedLine &line){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_ParsedLine &line){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_ParsedLine &line){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_ParsedLine &line){
}*/
#endif
