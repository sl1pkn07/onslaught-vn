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
ErrorCode NONS_ScriptInterpreter::command_nsa(NONS_Statement &stmt){
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_nsadir(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	std::wstring temp;
	_GETWCSVALUE(temp,0,)
	this->nsadir=UniToISO88591(temp);
	tolower(this->nsadir);
	toforwardslash(this->nsadir);
	if (this->nsadir[this->nsadir.size()-1]!='/')
		this->nsadir.push_back('/');
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_return(NONS_Statement &stmt){
	if (this->callStack.empty())
		return NONS_EMPTY_CALL_STACK;
	NONS_StackElement *popped;
	do{
		popped=this->callStack.back();
		this->callStack.pop_back();
	}while (popped->type!=SUBROUTINE_CALL && popped->type!=TEXTGOSUB_CALL);
	this->thread->gotoPair(popped->returnTo.toPair());
	if (popped->type==TEXTGOSUB_CALL){
		this->Printer_support(popped->pages,0,0,0);
		delete popped;
		return NONS_NO_ERROR;
	}
	NONS_ScriptLine &line=popped->interpretAtReturn;
	ErrorCode ret=NONS_NO_ERROR_BUT_BREAK;
	for (ulong a=0;a<line.statements.size();a++){
		ErrorCode error=this->interpretString(*line.statements[a],stmt.lineOfOrigin,stmt.fileOffset);
		if (error==NONS_END)
			return NONS_END;
		if (error==NONS_GOSUB)
			this->callStack.back()->interpretAtReturn=NONS_ScriptLine(line,a+1);
		if (!CHECK_FLAG(error,NONS_NO_ERROR_FLAG)){
			handleErrors(error,-1,"NONS_ScriptInterpreter::command_if",1);
			ret=NONS_UNDEFINED_ERROR;
		}
		if (CHECK_FLAG(error,NONS_BREAK_WORTHY_ERROR))
			break;
	}
	return ret;
}

ErrorCode NONS_ScriptInterpreter::command_mov(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	NONS_VariableMember *var;
	_GETVARIABLE(var,0,)
	if (var->getType()==INTEGER){
		long val;
		_GETINTVALUE(val,1,)
		var->set(val);
	}else{
		std::wstring val;
		_GETWCSVALUE(val,1,)
		var->set(val);
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_len(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	std::wstring src;
	_GETWCSVALUE(src,1,)
	dst->set(src.size());
	return NONS_NO_ERROR;
}

/*
Behavior notes:
rnd %a,%n ;a=[0;n)
rnd2 %a,%min,%max ;a=[min;max]
*/
ErrorCode NONS_ScriptInterpreter::command_rnd(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	long min=0,max;
	if (!stdStrCmpCI(stmt.commandName,L"rnd")){
		_GETINTVALUE(max,1,)
		max--;
	}else{
		MINIMUM_PARAMETERS(3);
		_GETINTVALUE(max,2,)
		_GETINTVALUE(min,1,)
	}
	//lower+int(double(upper-lower+1)*rand()/(RAND_MAX+1.0))
	dst->set(min+(rand()*(max-min))/RAND_MAX);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_play(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	std::wstring name;
	_GETWCSVALUE(name,0,)
	ErrorCode error=NONS_UNDEFINED_ERROR;
	this->mp3_loop=0;
	this->mp3_save=0;
	if (!stdStrCmpCI(stmt.commandName,L"play") || !stdStrCmpCI(stmt.commandName,L"loopbgm"))
		this->mp3_loop=1;
	else if (!stdStrCmpCI(stmt.commandName,L"mp3save"))
		this->mp3_save=1;
	else if (!stdStrCmpCI(stmt.commandName,L"mp3loop") || !stdStrCmpCI(stmt.commandName,L"bgm")){
		this->mp3_loop=1;
		this->mp3_save=1;
	}
	if (name[0]=='*'){
		int track=atoi(UniToISO88591(name.substr(1)).c_str());
		std::wstring temp=L"track";
		temp+=itoa<wchar_t>(track,2);
		error=this->everything->audio->playMusic(&temp,this->mp3_loop?-1:0);
		if (error==NONS_NO_ERROR)
			this->saveGame->musicTrack=track;
		else
			this->saveGame->musicTrack=-1;
	}else{
		ulong size;
		char *buffer=(char *)this->everything->archive->getFileBuffer(name,size);
		this->saveGame->musicTrack=-1;
		if (!buffer)
			error=NONS_FILE_NOT_FOUND;
		else
			error=this->everything->audio->playMusic(name,buffer,size,this->mp3_loop?-1:0);
		if (error==NONS_NO_ERROR)
			this->saveGame->music=name;
		else
			this->saveGame->music.clear();
	}
	return error;
}

ErrorCode NONS_ScriptInterpreter::command_playstop(NONS_Statement &stmt){
	this->mp3_loop=0;
	this->mp3_save=0;
	return this->everything->audio->stopMusic();
}

ErrorCode NONS_ScriptInterpreter::command_mp3vol(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	long vol;
	_GETINTVALUE(vol,0,)
	this->everything->audio->musicVolume(vol);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_mid(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(3);
	NONS_VariableMember *dst;
	_GETSTRVARIABLE(dst,0,)
	long start,len;
	_GETINTVALUE(start,2,)
	std::wstring src;
	_GETWCSVALUE(src,1,)
	len=src.size();
	if ((ulong)start>=src.size()){
		dst->set(L"");
		return NONS_NO_ERROR;
	}
	if (stmt.parameters.size()>3){
		_GETINTVALUE(len,3,)
	}
	if ((ulong)start+len>src.size())
		len=src.size()-start;
	dst->set(src.substr(start,len));
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_movl(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	NONS_VariableMember *dst;
	ErrorCode error;
	dst=this->store->retrieve(stmt.parameters[0],&error);
	if (!CHECK_FLAG(error,NONS_NO_ERROR_FLAG))
		return error;
	if (dst->getType()!=INTEGER_ARRAY)
		return NONS_EXPECTED_ARRAY;
	if (stmt.parameters.size()-1>dst->dimensionSize)
		handleErrors(NONS_TOO_MANY_PARAMETERS,stmt.lineOfOrigin->lineNumber,"NONS_ScriptInterpreter::command_movl",1);
	for (ulong a=0;a<dst->dimensionSize && a<stmt.parameters.size()-1;a++){
		long temp;
		_GETINTVALUE(temp,a+1,)
		dst->dimension[a]->set(temp);
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_jumpf(NONS_Statement &stmt){
	if (!stdStrCmpCI(stmt.commandName,L"jumpb")){
		if (!this->thread->gotoJumpBackwards(stmt.fileOffset))
			return NONS_NO_JUMPS;
		return NONS_NO_ERROR_BUT_BREAK;
	}else{
		if (!this->thread->gotoJumpForward(stmt.fileOffset))
			return NONS_NO_JUMPS;
		return NONS_NO_ERROR_BUT_BREAK;
	}
}

ErrorCode NONS_ScriptInterpreter::command_mp3fadeout(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
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

ErrorCode NONS_ScriptInterpreter::command_mpegplay(NONS_Statement &stmt){
	return NONS_UNIMPLEMENTED_COMMAND;
}

ErrorCode NONS_ScriptInterpreter::command_new_set_window(NONS_Statement &stmt){
	this->legacy_set_window=0;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_ld(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(3);
	std::wstring name;
	_GETWCSVALUE(name,1,)
	NONS_Layer **l=0;
	long off;
	switch (stmt.parameters[0][0]){
		case 'l':
			l=&this->everything->screen->leftChar;
			off=this->everything->screen->screen->virtualScreen->w/4;
			break;
		case 'c':
			l=&this->everything->screen->centerChar;
			off=this->everything->screen->screen->virtualScreen->w/2;
			break;
		case 'r':
			l=&this->everything->screen->rightChar;
			off=this->everything->screen->screen->virtualScreen->w/4*3;
			break;
		default:
			return NONS_INVALID_PARAMETER;
	}
	if (this->hideTextDuringEffect)
		this->everything->screen->hideText();
	if (!*l)
		*l=new NONS_Layer(&name);
	else if (!(*l)->load(&name))
		return NONS_FILE_NOT_FOUND;
	if (!(*l)->data)
		return NONS_FILE_NOT_FOUND;
	(*l)->centerAround(off);
	(*l)->useBaseline(this->everything->screen->char_baseline);
	long number,duration;
	ErrorCode ret;
	_GETINTVALUE(number,2,)
	if (stmt.parameters.size()>3){
		_GETINTVALUE(duration,3,)
		std::wstring rule;
		if (stmt.parameters.size()>4)
			_GETWCSVALUE(rule,4,)
		ret=this->everything->screen->BlendNoCursor(number,duration,&rule);
	}else
		ret=this->everything->screen->BlendNoCursor(number);
	return ret;
}

ErrorCode NONS_ScriptInterpreter::command_lsp(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(4);
	long spriten,x,y,alpha=255;
	std::wstring str;
	_GETINTVALUE(spriten,0,)
	_GETINTVALUE(x,2,)
	_GETINTVALUE(y,3,)
	if (stmt.parameters.size()>4)
		_GETINTVALUE(alpha,4,)
	_GETWCSVALUE(str,1,)
	if (alpha>255)
		alpha=255;
	if (alpha<0)
		alpha=0;
	_HANDLE_POSSIBLE_ERRORS(this->everything->screen->loadSprite(spriten,str,x,y,alpha,!stdStrCmpCI(stmt.commandName,L"lsp")),);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_literal_print(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	std::wstring string=this->convertParametersToString(stmt);
	if (string.size()){
		this->everything->screen->showText();
		if (this->everything->screen->output->prepareForPrinting(string.c_str())){
			if (this->pageCursor->animate(this->menu,this->autoclick)<0)
				return NONS_NO_ERROR;
			this->everything->screen->clearText();
		}
		while (this->everything->screen->output->print(0,string.size(),this->everything->screen->screen)){
			if (this->pageCursor){
				if (this->pageCursor->animate(this->menu,this->autoclick)<0)
					return NONS_NO_ERROR;
			}else
				waitUntilClick();
			this->everything->screen->clearText();
		}
		this->everything->screen->output->endPrinting();
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_print(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	long number,duration;
	ErrorCode ret;
	_GETINTVALUE(number,0,)
	if (stmt.parameters.size()>1){
		_GETINTVALUE(duration,1,)
		std::wstring rule;
		if (stmt.parameters.size()>2)
			_GETWCSVALUE(rule,2,)
		this->everything->screen->hideText();
		ret=this->everything->screen->BlendNoCursor(number,duration,&rule);
	}else{
		this->everything->screen->hideText();
		ret=this->everything->screen->BlendNoCursor(number);
	}
	return ret;
}

ErrorCode NONS_ScriptInterpreter::command_monocro(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	long color;
	if (!stdStrCmpCI(stmt.parameters[0],L"off")){
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

ErrorCode NONS_ScriptInterpreter::command_nega(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	long onoff;
	_GETINTVALUE(onoff,0,)
	if (onoff){
		if (!this->everything->screen->negative)
			this->everything->screen->negative=new NONS_GFX(onoff==1?1:2,0,0);
		else
			this->everything->screen->negative->effect=onoff==1?1:2;
		this->everything->screen->negative->type=POSTPROCESSING;
	}else if (this->everything->screen->negative){
		delete this->everything->screen->negative;
		this->everything->screen->negative=0;
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_next(NONS_Statement &stmt){
	if (this->callStack.empty())
		return NONS_EMPTY_CALL_STACK;
	NONS_StackElement *element=this->callStack.back();
	if (element->type!=FOR_NEST)
		return NONS_UNEXPECTED_NEXT;
	element->var->add(element->step);
	if (element->step>0 && element->var->getInt()>element->to || element->step<0 && element->var->getInt()<element->to){
		delete element;
		this->callStack.pop_back();
		return NONS_NO_ERROR;
	}
	NONS_Statement *cstmt=this->thread->getCurrentStatement();
	element->end.line=cstmt->lineOfOrigin->lineNumber;
	element->end.statement=cstmt->statementNo;
	this->thread->gotoPair(element->returnTo.toPair());
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_locate(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
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

ErrorCode NONS_ScriptInterpreter::command_resettimer(NONS_Statement &stmt){
	this->timer=SDL_GetTicks();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_repaint(NONS_Statement &stmt){
	if (this->everything->screen)
		this->everything->screen->BlendNoCursor(1);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_rmenu(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	if (stmt.parameters.size()%2)
		return NONS_INSUFFICIENT_PARAMETERS;
	std::vector<std::wstring> items;
	for (ulong a=0;a<stmt.parameters.size();a++){
		std::wstring s;
		_GETWCSVALUE(s,a,)
		a++;
		items.push_back(s);
		items.push_back(stmt.parameters[a]);
	}
	this->menu->resetStrings(&items);
	this->menu->rightClickMode=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_menusetwindow(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(7);
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

/*ErrorCode NONS_ScriptInterpreter::command_kidokuskip(NONS_Statement &stmt){
	softwareCtrlIsPressed=1;
	return NONS_NO_ERROR;
}*/

ErrorCode NONS_ScriptInterpreter::command_menuselectcolor(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(3);
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

ErrorCode NONS_ScriptInterpreter::command_menuselectvoice(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(7);
	std::wstring entry,
		cancel,
		mouse,
		click,
		yes,
		no;
	_GETWCSVALUE(entry,0,)
	_GETWCSVALUE(cancel,1,)
	_GETWCSVALUE(mouse,2,)
	_GETWCSVALUE(click,3,)
	_GETWCSVALUE(yes,5,)
	_GETWCSVALUE(no,6,)
	this->menu->voiceEntry=entry;
	this->menu->voiceCancel=cancel;
	this->menu->voiceMO=mouse;
	this->menu->voiceClick=click;
	this->menu->voiceYes=yes;
	this->menu->voiceNo=no;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_rmode(NONS_Statement &stmt){
	if (!stdStrCmpCI(stmt.commandName,L"roff")){
		this->menu->rightClickMode=0;
		return NONS_NO_ERROR;
	}
	MINIMUM_PARAMETERS(1);
	long a;
	_GETINTVALUE(a,0,)
	if (!a)
		this->menu->rightClickMode=0;
	else
		this->menu->rightClickMode=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_reset(NONS_Statement &stmt){
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

ErrorCode NONS_ScriptInterpreter::command_msp(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(4);
	long spriten,x,y,alpha;
	_GETINTVALUE(spriten,0,)
	_GETINTVALUE(x,1,)
	_GETINTVALUE(y,2,)
	_GETINTVALUE(alpha,3,)
	if (ulong(spriten)>this->everything->screen->layerStack.size())
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	NONS_Layer *l=this->everything->screen->layerStack[spriten];
	if (!l)
		return NONS_NO_SPRITE_LOADED_THERE;
	if (stdStrCmpCI(stmt.commandName,L"amsp")){
		l->position.x+=x;
		l->position.y+=y;
		if (long(l->alpha)+alpha>255)
			l->alpha=255;
		else if (long(l->alpha)+alpha<0)
			l->alpha=0;
		else
			l->alpha+=alpha;
	}else{
		l->position.x=x;
		l->position.y=y;
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
	SDL_Surface *copyDst=makeSurface(dst->w,dst->h,32);
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

ErrorCode NONS_ScriptInterpreter::command_quake(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
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

ErrorCode NONS_ScriptInterpreter::command_lookbackbutton(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(4);
	std::wstring A,B,C,D;
	_GETWCSVALUE(A,0,)
	_GETWCSVALUE(B,1,)
	_GETWCSVALUE(C,2,)
	_GETWCSVALUE(D,3,)
	NONS_AnimationInfo anim;
	anim.parse(A);
	A=L":l;";
	A.append(anim.getFilename());
	anim.parse(B);
	B=L":l;";
	B.append(anim.getFilename());
	anim.parse(C);
	C=L":l;";
	C.append(anim.getFilename());
	anim.parse(D);
	D=L":l;";
	D.append(anim.getFilename());
	bool ret=this->everything->screen->lookback->setUpButtons(A,B,C,D);
	return ret?NONS_NO_ERROR:NONS_FILE_NOT_FOUND;
}

ErrorCode NONS_ScriptInterpreter::command_lookbackcolor(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	long a;
	_GETINTVALUE(a,0,)
	SDL_Color col={(a&0xFF0000)>>16,(a&0xFF00)>>8,a&0xFF,0};
	this->everything->screen->lookback->foreground=col;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_lookbackflush(NONS_Statement &stmt){
	this->everything->screen->output->log.clear();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_loadgame(NONS_Statement &stmt){
	long file;
	_GETINTVALUE(file,0,)
	if (file<1)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	return this->load(file);
}

ErrorCode NONS_ScriptInterpreter::command_menu_full(NONS_Statement &stmt){
	this->everything->screen->screen->toggleFullscreen(!stdStrCmpCI(stmt.commandName,L"menu_full"));
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_labellog(NONS_Statement &stmt){
	labellog.commit=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_movN(NONS_Statement &stmt){
	ulong functionVersion=atoi(stmt.commandName.substr(3));
	MINIMUM_PARAMETERS(functionVersion+1);
	NONS_VariableMember *first;
	_GETVARIABLE(first,0,)
	Sint32 index=this->store->getVariableIndex(first);
	if (Sint32(index+functionVersion)>NONS_VariableStore::indexUpperLimit)
		return NONS_NOT_ENOUGH_VARIABLE_INDICES;
	std::vector<long> intvalues;
	std::vector<std::wstring> strvalues;
	for (ulong a=0;a<functionVersion;a++){
		if (first->getType()==INTEGER){
			long val;
			_GETINTVALUE(val,a+1,)
			intvalues.push_back(val);
		}else{
			std::wstring val;
			_GETWCSVALUE(val,a+1,)
			strvalues.push_back(val);
		}
	}
	for (ulong a=0;a<functionVersion;a++){
		if (first->getType()==INTEGER){
			NONS_Variable *var=this->store->retrieve(index+a,0);
			var->intValue->set(intvalues[a]);
		}else{
			NONS_Variable *var=this->store->retrieve(index+a,0);
			var->wcsValue->set(strvalues[a]);
		}
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_ofscopy(NONS_Statement &stmt){
	//TODO: What does this do?
	/*
	LOCKSCREEN;
	manualBlit(this->
	this->everything->screen->Background->load(this->everything->screen->screen->virtualScreen);
	UNLOCKSCREEN;
	*/
	return NONS_UNDOCUMENTED_COMMAND;
}

/*ErrorCode NONS_ScriptInterpreter::command_(NONS_Statement &stmt){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_Statement &stmt){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_Statement &stmt){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_Statement &stmt){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_Statement &stmt){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_Statement &stmt){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_Statement &stmt){
}*/
#endif
