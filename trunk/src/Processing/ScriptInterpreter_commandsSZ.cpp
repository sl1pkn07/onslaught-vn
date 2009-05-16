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
//#include "../IO_System/SaveFile.h"
#include <cmath>
#include <sstream>

#ifndef NONS_SCRIPTINTERPRETER_COMMANDSSZ_CPP
#define NONS_SCRIPTINTERPRETER_COMMANDSSZ_CPP
ErrorCode NONS_ScriptInterpreter::command_skip(NONS_ParsedLine &line){
	long count=2;
	if (line.parameters.size()){
		_GETINTVALUE(count,0,)
	}
	if (!count)
		return NONS_ZERO_VALUE_IN_SKIP;
	ulong current_line=this->current_line,target=current_line+count-1;
	long pos=0;
	long len=this->script->length;
	wchar_t *buffer=this->script->script;
	for (ulong a=0;a<target && pos<len;a++){
		long temppos=pos;
		for (;temppos<len && buffer[temppos]!=10 && buffer[temppos]!=13;temppos++);
		if (temppos==len)
			break;
		if (buffer[temppos]==10 || buffer[temppos+1]!=10)
			temppos++;
		else
			temppos+=2;
		if (temppos>=len)
			break;
		pos=temppos;
	}
	this->interpreter_position=pos;
	return NONS_NO_ERROR_BUT_BREAK;
}

ErrorCode NONS_ScriptInterpreter::command_wave(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long size;
	wchar_t *name=0;
	_GETWCSVALUE(name,0,)
	NONS_tolower(name);
	toforwardslash(name);
	ErrorCode error;
	this->wav_loop=!!wcscmp(line.commandName,L"wave");
	if (this->everything->audio->bufferIsLoaded(name))
		error=this->everything->audio->playSoundAsync(name,0,0,0,this->wav_loop?-1:0);
	else{
		char *buffer=(char *)this->everything->archive->getFileBuffer(name,(ulong *)&size);
		error=!buffer?NONS_FILE_NOT_FOUND:this->everything->audio->playSoundAsync(name,buffer,size,0,this->wav_loop?-1:0);
	}
	delete[] name;
	return error;
}

ErrorCode NONS_ScriptInterpreter::command_wavestop(NONS_ParsedLine &line){
	this->wav_loop=0;
	return this->everything->audio->stopSoundAsync(0);
}

ErrorCode NONS_ScriptInterpreter::command_wait(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	//if( skip_flag || draw_one_page_flag || ctrl_pressed_status || skip_to_wait ) return RET_CONTINUE;
	long ms;
	_GETINTVALUE(ms,0,)
	waitNonCancellable(ms);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_time(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *h,*m,*s;
	_GETINTVARIABLE(h,0,)
	_GETINTVARIABLE(m,1,)
	_GETINTVARIABLE(s,2,)
	time_t t=time(0);
	tm *time=localtime(&t);
	h->set(time->tm_hour);
	m->set(time->tm_min);
	s->set(time->tm_sec);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_stop(NONS_ParsedLine &line){
	this->mp3_loop=0;
	this->mp3_save=0;
	this->wav_loop=0;
	return this->everything->audio->stopAllSound();
}

ErrorCode NONS_ScriptInterpreter::command_setwindow(NONS_ParsedLine &line){
	long frameXstart,
		frameYstart,
		frameXend,
		frameYend,
		fontsize,
		spacingX,
		spacingY,
		speed,
		bold,
		shadow,
		color,
		windowXstart,
		windowYstart,
		windowXend,
		windowYend;
	wchar_t *filename=0;
	bool syntax=0;
	int forceLineSkip=0;
	if (this->legacy_set_window){
		long fontsizeY;
		if (line.parameters.size()<14)
			return NONS_INSUFFICIENT_PARAMETERS;
		_GETINTVALUE(frameXstart,0,)
		_GETINTVALUE(frameYstart,1,)
		_GETINTVALUE(frameXend,2,)
		_GETINTVALUE(frameYend,3,)
		_GETINTVALUE(fontsize,4,)
		_GETINTVALUE(fontsizeY,5,)
		_GETINTVALUE(spacingX,6,)
		_GETINTVALUE(spacingY,7,)
		_GETINTVALUE(speed,8,)
		_GETINTVALUE(bold,9,)
		_GETINTVALUE(shadow,10,)
		_GETINTVALUE(windowXstart,12,)
		_GETINTVALUE(windowYstart,13,)
		if (this->store->getIntValue(line.parameters[11],&color)!=NONS_NO_ERROR){
			syntax=1;
			_GETWCSVALUE(filename,11,)
			windowXend=windowXstart+1;
			windowYend=windowYstart+1;
		}else{
			_GETINTVALUE(windowXend,14,)
			_GETINTVALUE(windowYend,15,)
		}
		frameXend*=fontsize+spacingX;
		frameXend+=frameXstart;
		fontsize=this->defaultfs;
		forceLineSkip=fontsizeY+spacingY;
		frameYend*=fontsizeY+spacingY;
		frameYend+=frameYstart;
	}else{
		if (line.parameters.size()<15)
			return NONS_INSUFFICIENT_PARAMETERS;
		_GETINTVALUE(frameXstart,0,)
		_GETINTVALUE(frameYstart,1,)
		_GETINTVALUE(frameXend,2,)
		_GETINTVALUE(frameYend,3,)
		_GETINTVALUE(fontsize,4,)
		_GETINTVALUE(spacingX,5,)
		_GETINTVALUE(spacingY,6,)
		_GETINTVALUE(speed,7,)
		_GETINTVALUE(bold,8,)
		_GETINTVALUE(shadow,9,)
		_GETINTVALUE(windowXstart,11,)
		_GETINTVALUE(windowYstart,12,)
		_GETINTVALUE(windowXend,13,)
		_GETINTVALUE(windowYend,14,)
		if (this->store->getIntValue(line.parameters[10],&color)!=NONS_NO_ERROR){
			syntax=1;
			_GETWCSVALUE(filename,10,)
		}
	}
	bold=0;
	if (windowXstart<0 || windowXend<0 || windowXstart<0 || windowYend<0 ||
			frameXstart<0 || frameXend<0 || frameXstart<0 || frameYend<0 ||
			windowXstart>=windowXend ||
			windowYstart>=windowYend ||
			frameXstart>=frameXend ||
			frameYstart>=frameYend ||
			fontsize<1){
		if (syntax)
			delete[] filename;
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	}
	SDL_Rect windowRect={windowXstart,windowYstart,windowXend-windowXstart,windowYend-windowYstart};
	SDL_Rect frameRect={frameXstart,frameYstart,frameXend-frameXstart,frameYend-frameYstart};
	{
		SDL_Surface *scr=this->everything->screen->screen->virtualScreen;
		if (frameRect.x+frameRect.w>scr->w || frameRect.y+frameRect.h>scr->h)
			v_stderr <<"Warning: The text frame is larger than the screen"<<std::endl;
		if (this->everything->screen->output->shadeLayer->useDataAsDefaultShade){
			ImageLoader->unfetchImage(this->everything->screen->output->shadeLayer->data);
			this->everything->screen->output->shadeLayer->data=0;
		}
		if (fontsize!=this->main_font->getsize()){
			delete this->main_font;
			//this->main_font=new NONS_Font("default.ttf",fontsize,/*bold!=0?TTF_STYLE_BOLD:*/TTF_STYLE_NORMAL);
			INIT_NONS_FONT(this->main_font,fontsize,this->everything->archive)
			this->main_font->spacing=spacingX;
			this->everything->screen->output->foregroundLayer->fontCache->font=this->main_font;
			this->everything->screen->output->foregroundLayer->fontCache->refreshCache();
			this->everything->screen->output->shadowLayer->fontCache->font=this->main_font;
			this->everything->screen->output->shadowLayer->fontCache->refreshCache();
		}/*else
			this->main_font->setStyle(bold!=0?TTF_STYLE_BOLD:TTF_STYLE_NORMAL);*/
		SDL_Surface *pic;
		if (!syntax){
			this->everything->screen->resetParameters(&windowRect,&frameRect,this->main_font,shadow!=0);
			this->everything->screen->output->shadeLayer->setShade((color&0xFF0000)>>16,(color&0xFF00)>>8,color&0xFF);
			this->everything->screen->output->shadeLayer->Clear();
		}else{
			long f=instr(filename,";");
			pic=ImageLoader->fetchImage(
				filename+(f>=0?f+1:0),
				&(this->everything->screen->screen->virtualScreen->clip_rect),
				CLOptions.layerMethod
			);
			windowRect.w=pic->w;
			windowRect.h=pic->h;
			this->everything->screen->resetParameters(&windowRect,&frameRect,this->main_font,shadow!=0);
		}
		if (!!syntax)
			this->everything->screen->output->shadeLayer->usePicAsDefaultShade(pic);
	}
	this->everything->screen->output->extraAdvance=spacingX;
	//this->everything->screen->output->extraLineSkip=0;
	if (forceLineSkip)
		this->main_font->lineSkip=forceLineSkip;
	this->default_speed_slow=speed*2;
	this->default_speed_med=speed;
	this->default_speed_fast=speed/2;
	switch (this->current_speed_setting){
		case 0:
			this->default_speed=this->default_speed_slow;
			break;
		case 1:
			this->default_speed=this->default_speed_med;
			break;
		case 2:
			this->default_speed=this->default_speed_fast;
			break;
	}
	this->everything->screen->output->display_speed=this->default_speed;
	if (syntax)
		delete[] filename;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_set_default_font_size(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long a;
	_GETINTVALUE(a,0,)
	this->defaultfs=a;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_setcursor(NONS_ParsedLine &line){
	if (line.parameters.size()<4)
		return NONS_INSUFFICIENT_PARAMETERS;
	long which,
		x,y;
	wchar_t *string=0;
	_GETINTVALUE(which,0,)
	_GETINTVALUE(x,2,)
	_GETINTVALUE(y,3,)
	_GETWCSVALUE(string,1,)
	bool absolute=wcscmp(line.commandName,L"abssetcursor")==0;
	if (!which){
		if (this->arrowCursor)
			delete this->arrowCursor;
		this->arrowCursor=new NONS_Cursor(string,x,y,absolute);
		if (this->saveGame->arrowCursorString)
			delete[] this->saveGame->arrowCursorString;
		this->saveGame->arrowCursorString=copyWString(string);
		this->saveGame->arrowCursorAbs=absolute;
		this->saveGame->arrowCursorX=x;
		this->saveGame->arrowCursorY=y;
	}else{
		if (this->pageCursor)
			delete this->pageCursor;
		this->pageCursor=new NONS_Cursor(string,x,y,absolute);
		if (this->saveGame->pageCursorString)
			delete[] this->saveGame->pageCursorString;
		this->saveGame->pageCursorString=copyWString(string);
		this->saveGame->pageCursorAbs=absolute;
		this->saveGame->pageCursorX=x;
		this->saveGame->pageCursorY=y;
	}

	delete[] string;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_tal(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	long newalpha;
	_GETINTVALUE(newalpha,1,)
	switch (line.parameters[0][0]){
		case 'l':
			if (this->hideTextDuringEffect)
				this->everything->screen->hideText();
			this->everything->screen->leftChar->alpha=newalpha;
			break;
		case 'r':
			if (this->hideTextDuringEffect)
				this->everything->screen->hideText();
			this->everything->screen->rightChar->alpha=newalpha;
			break;
		case 'c':
			if (this->hideTextDuringEffect)
				this->everything->screen->hideText();
			this->everything->screen->centerChar->alpha=newalpha;
			break;
		case 'a':
			if (this->hideTextDuringEffect)
				this->everything->screen->hideText();
			this->everything->screen->leftChar->alpha=newalpha;
			this->everything->screen->rightChar->alpha=newalpha;
			this->everything->screen->centerChar->alpha=newalpha;
			break;
		default:
			return NONS_INVALID_PARAMETER;
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_unimplemented(NONS_ParsedLine &line){
	return NONS_UNIMPLEMENTED_COMMAND;
}

ErrorCode NONS_ScriptInterpreter::command_undocumented(NONS_ParsedLine &line){
	return NONS_UNDOCUMENTED_COMMAND;
}

ErrorCode NONS_ScriptInterpreter::command_unalias(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	wchar_t *name=line.parameters[0];
	std::map<wchar_t *,NONS_VariableMember *,wstrCmpCI>::iterator i=this->store->constants.find(name);
	if (i==this->store->constants.end())
		return NONS_UNDEFINED_CONSTANT;
	delete[] i->first;
	delete i->second;
	this->store->constants.erase(i);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_vsp(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	long n=-1,visibility;
	_GETINTVALUE(n,0,)
	_GETINTVALUE(visibility,1,)
	if (n>0 && ulong(n)>=this->everything->screen->layerStack.size())
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	if (this->everything->screen->layerStack[n] && this->everything->screen->layerStack[n]->data)
		this->everything->screen->layerStack[n]->visible=!!visibility;
	//this->everything->screen->BlendAll();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_windoweffect(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	/*if (!this->everything->screen)
		this->setDefaultWindow();*/
	long number,duration;
	wchar_t *rule=0;
	_GETINTVALUE(number,0,)
	if (line.parameters.size()>1){
		_GETINTVALUE(duration,1,)
		if (line.parameters.size()>2)
			_GETWCSVALUE(rule,2,)
		if (!this->everything->screen->output->transition->stored)
			delete this->everything->screen->output->transition;
		this->everything->screen->output->transition=new NONS_GFX(number,duration,rule);
	}else{
		NONS_GFX *effect=this->gfx_store->retrieve(number);
		if (!effect)
			return NONS_UNDEFINED_EFFECT;
		if (!this->everything->screen->output->transition->stored)
			delete this->everything->screen->output->transition;
		this->everything->screen->output->transition=effect;
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_textonoff(NONS_ParsedLine &line){
	/*if (!this->everything->screen)
		this->setDefaultWindow();*/
	if (!wcscmp(line.commandName,L"texton"))
		this->everything->screen->showText();
	else
		this->everything->screen->hideText();
	return NONS_NO_ERROR;
}

extern std::ofstream textDumpFile;

ErrorCode NONS_ScriptInterpreter::command_select(NONS_ParsedLine &line){
	bool selnum;
	if (!wcscmp(line.commandName,L"selnum")){
		if (line.parameters.size()<3 || !(line.parameters.size()%2))
			return NONS_INSUFFICIENT_PARAMETERS;
		selnum=1;
	}else{
		if (line.parameters.size()<2 || line.parameters.size()%2)
			return this->bad_select(line);
		selnum=0;
	}
	NONS_VariableMember *var;
	if (selnum)
		_GETINTVARIABLE(var,0,)
	std::vector<wchar_t *> strings,jumps;
	for (ulong a=selnum;a<line.parameters.size();a++){
		wchar_t *temp=0;
		_GETWCSVALUE(temp,a,
			for (ulong b=0;b<strings.size();b++){
				delete[] strings[b];
			})
		strings.push_back(temp);
		jumps.push_back(line.parameters[++a]);
	}
	/*if (!this->everything->screen)
		this->setDefaultWindow();*/
	NONS_ButtonLayer layer(this->main_font,this->everything->screen,0,(void *)this->menu);
	layer.makeTextButtons(
		&strings,
		&(this->selectOn),
		&(this->selectOff),
		!!this->everything->screen->output->shadowLayer,
		this->selectVoiceEntry,
		this->selectVoiceMouseOver,
		this->selectVoiceClick,
		this->everything->audio,
		this->everything->archive,
		this->everything->screen->output->w,
		this->everything->screen->output->h);
	ctrlIsPressed=0;
	//softwareCtrlIsPressed=0;
	this->everything->screen->showText();
	int choice=layer.getUserInput(this->everything->screen->output->x,this->everything->screen->output->y);
	if (choice==-2){
		this->everything->screen->clearText();
		choice=layer.getUserInput(this->everything->screen->output->x,this->everything->screen->output->y);
		if (choice==-2){
			for (ulong b=0;b<strings.size();b++){
				delete[] strings[b];
			}
			return NONS_SELECT_TOO_BIG;
		}
	}
	if (choice==-3){
		for (ulong b=0;b<strings.size();b++)
			delete[] strings[b];
		return NONS_NO_ERROR;
	}
	this->everything->screen->clearText();
	if (textDumpFile.is_open()){
		char *copy=WChar_to_UTF8(strings[choice]);
		textDumpFile <<"    "<<copy<<std::endl;
		delete[] copy;
	}
	for (ulong b=0;b<strings.size();b++)
		delete[] strings[b];
	if (selnum)
		var->set(choice);
	else{
		long offset=this->script->offsetFromBlock(jumps[choice]);
		if (offset<0)
			return NONS_NO_SUCH_BLOCK;
		if (!wcscmp(line.commandName,L"selgosub")){
			NONS_StackElement *p=new NONS_StackElement(this->interpreter_position,0,this->insideTextgosub());
			this->callStack.push_back(p);
		}
		this->interpreter_position=offset;
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::bad_select(NONS_ParsedLine &line){
	v_stderr <<"NONS_ScriptInterpreter::bad_select(): WARNING: a ";
	v_stderr.writeWideString(line.commandName);
	v_stderr <<" without regular parameters was found on line "<<line.lineNo<<". This may indicate that you're using "
		"the irregular syntax. If that's the case, you should consider migrating to the regular syntax.\n"
		"    I'm going to try and parse the command all the same, so don't be surprised if there are errors.\n"
		"    Oh, and this is still considered a syntax error, so I'm going to return INSUFFICIENT_PARAMETERS."<<std::endl;
	ulong pos=this->previous_interpreter_position;
	wchar_t *script=this->script->script;
	for (;!iswhitespace(script[pos]);pos++);
	while (1){
		for (;iswhitespace(script[pos]);pos++);
		ulong start=pos,l=1;
		if (script[start]=='\"' || script[start]=='`'){
			for (;script[start+l]!=script[start];l++);
			l++;
		}else
			for (;!iswhitespace(script[start+l]);l++);
		line.parameters.push_back(copyWString(script+start,l));
		pos+=l;
		for (;iswhitespace(script[pos]);pos++);
		if (script[pos]!=',')
			break;
		for (pos++;iswhitespace(script[pos]);pos++);
	};
	this->interpreter_position=pos;
	this->command_select(line);
	return NONS_INSUFFICIENT_PARAMETERS;
}

ErrorCode NONS_ScriptInterpreter::command_selectcolor(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	long on,off;
	_GETINTVALUE(on,0,)
	_GETINTVALUE(off,1,)
	this->selectOn.r=(on&0xFF0000)>>16;
	this->selectOn.g=(on&0xFF00)>>8;
	this->selectOn.b=on&0xFF;
	this->selectOff.r=(off&0xFF0000)>>16;
	this->selectOff.g=(off&0xFF00)>>8;
	this->selectOff.b=off&0xFF;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_selectvoice(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
	wchar_t *entry=0,
		*mouseover=0,
		*click=0;
	_GETWCSVALUE(entry,0,)
	_GETWCSVALUE(mouseover,1,delete[] entry;)
	_GETWCSVALUE(click,2,delete[] entry; delete[] mouseover;)
	NONS_tolower(entry);
	NONS_tolower(mouseover);
	NONS_tolower(click);
	uchar *buffer;
	if (*entry){
		long l;
		buffer=this->everything->archive->getFileBuffer(entry,(ulong *)&l);
		if (!buffer){
			delete[] entry;
			delete[] mouseover;
			delete[] click;
			return NONS_FILE_NOT_FOUND;
		}
		delete[] buffer;
	}else{
		delete[] entry;
		entry=0;
	}
	if (*mouseover){
		long l;
		uchar *buffer=this->everything->archive->getFileBuffer(mouseover,(ulong *)&l);
		if (!buffer){
			delete[] entry;
			delete[] mouseover;
			delete[] click;
			return NONS_FILE_NOT_FOUND;
		}
		delete[] buffer;
	}else{
		delete[] mouseover;
		mouseover=0;
	}
	if (*click){
		long l;
		uchar *buffer=this->everything->archive->getFileBuffer(click,(ulong *)&l);
		if (!buffer){
			delete[] entry;
			delete[] mouseover;
			delete[] click;
			return NONS_FILE_NOT_FOUND;
		}
		delete[] buffer;
	}else{
		delete[] click;
		click=0;
	}
	if (this->selectVoiceEntry)
		delete[] this->selectVoiceEntry;
	this->selectVoiceEntry=entry;
	if (this->selectVoiceMouseOver)
		delete[] this->selectVoiceMouseOver;
	this->selectVoiceMouseOver=mouseover;
	if (this->selectVoiceClick)
		delete[] this->selectVoiceClick;
	this->selectVoiceClick=click;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_trap(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	if (!wcscmp(line.parameters[0],L"off")){
		if (!trapFlag)
			return NONS_NO_TRAP_SET;
		trapFlag=0;
		this->trapLabel=0;
		return NONS_NO_ERROR;
	}
	long kind;
	if (!wcscmp(line.commandName,L"trap"))
		kind=1;
	else if (!wcscmp(line.commandName,L"lr_trap"))
		kind=2;
	else if (!wcscmp(line.commandName,L"trap2"))
		kind=3;
	else
		kind=4;
	long a=this->script->offsetFromBlock(line.parameters[0]);
	if (a<0)
		return NONS_NO_SUCH_BLOCK;
	this->trapLabel=a;
	trapFlag=kind;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_textclear(NONS_ParsedLine &line){
	if (this->everything->screen)
		this->everything->screen->clearText();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_textspeed(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long speed;
	_GETINTVALUE(speed,0,);
	if (speed<0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	/*if (!this->everything->screen)
		this->setDefaultWindow();*/
	this->everything->screen->output->display_speed=speed;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_waittimer(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long ms;
	_GETINTVALUE(ms,0,)
	if (ms<0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	ulong now=SDL_GetTicks();
	if (ulong(ms)>now-this->timer)
		SDL_Delay(ms-(now-this->timer));
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_savename(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
	wchar_t *save=0,
		*load=0,
		*slot=0;
	_GETWCSVALUE(save,0,)
	_GETWCSVALUE(load,1,delete[] save;)
	_GETWCSVALUE(slot,2,delete[] save; delete[] load;)
	if (this->menu->stringSave)
		delete[] this->menu->stringSave;
	this->menu->stringSave=save;
	if (this->menu->stringLoad)
		delete[] this->menu->stringLoad;
	this->menu->stringLoad=load;
	if (this->menu->stringSlot)
		delete[] this->menu->stringSlot;
	this->menu->stringSlot=slot;
	return NONS_NO_ERROR;
}

/*ErrorCode NONS_ScriptInterpreter::command_skipoff(NONS_ParsedLine &line){
	softwareCtrlIsPressed=0;
	return NONS_NO_ERROR;
}*/

ErrorCode NONS_ScriptInterpreter::command_savenumber(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long n;
	_GETINTVALUE(n,0,)
	if (n<1 || n>20)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	this->menu->slots=n;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_systemcall(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	if (!wcscmp(line.parameters[0],L"rmenu"))
		this->menu->callMenu();
	else
		this->menu->call(line.parameters[0]);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_use_new_if(NONS_ParsedLine &line){
	this->new_if=1;
	return NONS_NO_ERROR;
}

void quake(SDL_Surface *dst,char axis,ulong amplitude,ulong duration){
	float length=duration,
		amp=amplitude;
	SDL_Rect srcrect=dst->clip_rect,
		dstrect=srcrect;
	SDL_Surface *copyDst=SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA,dst->w,dst->h,32,rmask,gmask,bmask,amask);
	manualBlit(dst,0,copyDst,0);
	ulong start=SDL_GetTicks();
	while (1){
		float x=SDL_GetTicks()-start;
		if (x>duration)
			break;
		float y=sin(x*(20/length)*M_PI)*((amp/-length)*x+amplitude);
		SDL_FillRect(dst,&srcrect,0);
		if (axis=='x')
			dstrect.x=y;
		else
			dstrect.y=y;
		manualBlit(copyDst,&srcrect,dst,&dstrect);
		SDL_UpdateRect(dst,0,0,0,0);
	}
	SDL_FreeSurface(copyDst);
}

ErrorCode NONS_ScriptInterpreter::command_sinusoidal_quake(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	long amplitude,duration;
	_GETINTVALUE(amplitude,0,)
	_GETINTVALUE(duration,1,)
	if (amplitude<0 || duration<0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	amplitude*=10;
	if (line.commandName[5]=='x')
		amplitude=this->everything->screen->screen->convertW(amplitude);
	else
		amplitude=this->everything->screen->screen->convertH(amplitude);
	quake(this->everything->screen->screen->realScreen,line.commandName[5],amplitude,duration);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_savegame(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long file;
	_GETINTVALUE(file,0,)
	if (file<1)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	return this->save(file)?NONS_NO_ERROR:NONS_UNDEFINED_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_savefileexist(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	long file;
	_GETINTVALUE(file,1,)
	if (file<1)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	std::stringstream stream;
	//std::string path;
	stream <<save_directory<<"save"<<file<<".dat";
	//std::getline(stream,path);
	dst->set(fileExists(stream.str().c_str()));
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_savescreenshot(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	char *filename;
	_GETSTRVALUE(filename,0,)
	LOCKSCREEN;
	SDL_SaveBMP(this->everything->screen->screen->virtualScreen,filename);
	UNLOCKSCREEN;
	delete[] filename;
	return NONS_INSUFFICIENT_PARAMETERS;
}

ErrorCode NONS_ScriptInterpreter::command_savetime(NONS_ParsedLine &line){
	if (line.parameters.size()<5)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *month,*day,*hour,*minute;
	_GETINTVARIABLE(month,1,)
	_GETINTVARIABLE(day,2,)
	_GETINTVARIABLE(hour,3,)
	_GETINTVARIABLE(minute,4,)
	long file;
	_GETINTVALUE(file,0,)
	if (file<1)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	std::string path;
	{
		std::stringstream stream;
		stream <<save_directory<<"save"<<file<<".dat";
		path=stream.str();
	}
	if (!fileExists(path.c_str())){
		day->set(0);
		month->set(0);
		hour->set(0);
		minute->set(0);
	}else{
		tm *date=getDate(path.c_str());
		day->set(date->tm_mon+1);
		month->set(date->tm_mday);
		hour->set(date->tm_hour);
		minute->set(date->tm_min);
		delete date;
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_savetime2(NONS_ParsedLine &line){
	if (line.parameters.size()<7)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *year,*month,*day,*hour,*minute,*second;
	_GETINTVARIABLE(year,1,)
	_GETINTVARIABLE(month,2,)
	_GETINTVARIABLE(day,3,)
	_GETINTVARIABLE(hour,4,)
	_GETINTVARIABLE(minute,5,)
	_GETINTVARIABLE(second,6,)
	long file;
	_GETINTVALUE(file,0,)
		if (file<1)
			return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	std::string path;
	{
		std::stringstream stream;
		stream <<save_directory<<"save"<<file<<".dat";
		path=stream.str();
	}
	if (!fileExists(path.c_str())){
		year->set(0);
		month->set(0);
		day->set(0);
		hour->set(0);
		minute->set(0);
		second->set(0);
	}else{
		tm *date=getDate(path.c_str());
		year->set(date->tm_year+1900);
		month->set(date->tm_mday);
		day->set(date->tm_mon+1);
		hour->set(date->tm_hour);
		minute->set(date->tm_min);
		second->set(date->tm_sec);
		delete date;
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_split(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	wchar_t *srcStr=0,*searchStr=0;
	_GETWCSVALUE(srcStr,0,)
	_GETWCSVALUE(searchStr,1,delete[] srcStr;)
	std::vector <NONS_VariableMember *> dsts;
	for (ulong a=2;a<line.parameters.size();a++){
		NONS_VariableMember *var;
		_GETVARIABLE(var,a,delete[] srcStr; delete[] searchStr;)
		if (var->getType()==INTEGER)
			var->set(0);
		else
			var->set(L"",0);
		dsts.push_back(var);
	}
	wchar_t *middle=srcStr;
	ulong size=wcslen(searchStr);
	for (ulong a=0;a<dsts.size();a++){
		long next=instr(middle,searchStr);
		if (next<0){
			dsts[a]->set(middle,0);
			break;
		}
		wchar_t *copy=copyWString(middle,next);
		middle+=next+size;
		dsts[a]->set(copy,1);
	}
	delete[] srcStr;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_textgosub(NONS_ParsedLine &line){
	if (!line.parameters.size()){
		if (!!this->textgosub)
			delete[] this->textgosub;
		return NONS_NO_ERROR;
	}
	if (this->everything->script->offsetFromBlock(line.parameters[0])<0)
		return NONS_NO_SUCH_BLOCK;
	if (!!this->textgosub)
		delete[] this->textgosub;
	if (line.parameters.size()<2)
		this->textgosubRecurses=0;
	else{
		long rec;
		_GETINTVALUE(rec,1,)
		this->textgosubRecurses=(rec!=0);
	}
	this->textgosub=copyWString(line.parameters[0]);
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
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_ParsedLine &line){
}*/
#endif
