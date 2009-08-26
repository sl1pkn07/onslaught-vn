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
#include "../version.h"
#include <cctype>
#undef ABS
#include "../IO_System/Graphics/SDL_bilinear.h"

#ifdef NONS_SYS_WINDOWS
#include <windows.h>
extern HWND mainWindow;
#endif

#ifndef NONS_SCRIPTINTERPRETER_COMMANDSAI_CPP
#define NONS_SCRIPTINTERPRETER_COMMANDSAI_CPP
ErrorCode NONS_ScriptInterpreter::command_caption(NONS_Statement &stmt){
	if (!stmt.parameters.size())
		SDL_WM_SetCaption("",0);
	else{
		std::wstring temp;
		_GETWCSVALUE(temp,0,)
#ifndef NONS_SYS_WINDOWS
		SDL_WM_SetCaption(UniToUTF8(temp).c_str(),0);
#else
		SetWindowText(mainWindow,temp.c_str());
#endif
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_alias(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	/*if (this->interpreter_mode!=DEFINE)
		return NONS_NOT_IN_DEFINE_MODE;*/
	NONS_VariableMember *var=this->store->retrieve(stmt.parameters[0],0);
	if (!var){
		if (!isValidIdentifier(stmt.parameters[0]))
			return NONS_INVALID_ID_NAME;
		NONS_VariableMember *val;
		if (!stdStrCmpCI(stmt.commandName,L"numalias")){
			val=new NONS_VariableMember(INTEGER);
			if (stmt.parameters.size()>1){
				long temp;
				_GETINTVALUE(temp,1,)
				val->set(temp);
			}
		}else{
			val=new NONS_VariableMember(STRING);
			if (stmt.parameters.size()>1){
				std::wstring temp;
				_GETWCSVALUE(temp,1,)
				val->set(temp);
			}
		}
		val->makeConstant();
		this->store->constants[stmt.parameters[0]]=val;
		return NONS_NO_ERROR;
	}
	if (var->isConstant())
		return NONS_DUPLICATE_CONSTANT_DEFINITION;
	return NONS_INVALID_ID_NAME;
}

ErrorCode NONS_ScriptInterpreter::command_game(NONS_Statement &stmt){
	this->interpreter_mode=NORMAL;
	if (!this->thread->gotoLabel(L"start"))
		return NONS_NO_START_LABEL;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_goto(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	std::wstring label;
	_GETLABEL(label,0,)
	if (!this->goto_label(label))
		return NONS_NO_SUCH_BLOCK;
	return NONS_NO_ERROR_BUT_BREAK;
}

ErrorCode NONS_ScriptInterpreter::command_globalon(NONS_Statement &stmt){
	/*if (this->interpreter_mode!=DEFINE)
		return NONS_NOT_IN_DEFINE_MODE;*/
	this->store->commitGlobals=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_gosub(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	std::wstring label;
	_GETLABEL(label,0,)
	if (!this->gosub_label(label)){
		handleErrors(NONS_NO_SUCH_BLOCK,stmt.lineOfOrigin->lineNumber,"NONS_ScriptInterpreter::command_gosub",1);
		return NONS_NO_SUCH_BLOCK;
	}
	return NONS_GOSUB;
}

ErrorCode NONS_ScriptInterpreter::command_if(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	long res=0;
	bool notif=(stmt.commandName==L"notif");
	ErrorCode ret=this->store->evaluate(stmt.parameters[0],&res,notif && !this->new_if,0,0,0);
	if (!CHECK_FLAG(ret,NONS_NO_ERROR_FLAG))
		return ret;
	if (notif && this->new_if)
		res=!res;
	if (!res)
		return NONS_NO_ERROR;
	ret=NONS_NO_ERROR;
	NONS_ScriptLine line(0,stmt.parameters[1],0,1);
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

ErrorCode NONS_ScriptInterpreter::command_add(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	long val;
	_GETINTVALUE(val,1,)
	while (1){
		if (!stdStrCmpCI(stmt.commandName,L"add")){
			var->add(val);
			break;
		}
		if (!stdStrCmpCI(stmt.commandName,L"sub")){
			var->sub(val);
			break;
		}
		if (!stdStrCmpCI(stmt.commandName,L"mul")){
			var->mul(val);
			break;
		}
		if (!stdStrCmpCI(stmt.commandName,L"div")){
			if (!val)
				return NONS_DIVISION_BY_ZERO;
			var->div(val);
			break;
		}
		if (!stdStrCmpCI(stmt.commandName,L"sin")){
			var->set(sin(M_PI*val/180.0)*1000.0);
			break;
		}
		if (!stdStrCmpCI(stmt.commandName,L"cos")){
			var->set(cos(M_PI*val/180.0)*1000.0);
			break;
		}
		if (!stdStrCmpCI(stmt.commandName,L"tan")){
			var->set(tan(M_PI*val/180.0)*1000.0);
			break;
		}
		if (!stdStrCmpCI(stmt.commandName,L"mod")){
			if (!val)
				return NONS_DIVISION_BY_ZERO;
			var->mod(val);
			break;
		}
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_inc(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	if (!stdStrCmpCI(stmt.commandName,L"inc"))
		var->inc();
	else
		var->dec();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_cmp(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(3);
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	std::wstring opA,opB;
	_GETWCSVALUE(opA,1,)
	_GETWCSVALUE(opB,2,)
	var->set(wcscmp(opA.c_str(),opB.c_str()));
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_itoa(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	NONS_VariableMember *dst;
	_GETSTRVARIABLE(dst,0,)
	long src;
	_GETINTVALUE(src,1,)
	std::wstringstream stream;
	stream <<src;
	std::wstring str=stream.str();
	if (!stdStrCmpCI(stmt.commandName,L"itoa2")){
		for (ulong a=0;a<str.size();a++)
			if (NONS_isdigit(str[a]))
				str[a]+=0xFEE0;
	}
	dst->set(str);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_intlimit(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(3);
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	long lower,upper;
	_GETINTVALUE(lower,1,)
	_GETINTVALUE(upper,2,)
	dst->setlimits(lower,upper);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_end(NONS_Statement &stmt){
	return NONS_END;
}

ErrorCode NONS_ScriptInterpreter::command_date(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(3);
	NONS_VariableMember *year,*month,*day;
	_GETINTVARIABLE(year,0,)
	_GETINTVARIABLE(month,0,)
	_GETINTVARIABLE(day,0,)
	time_t t=time(0);
	tm *time=localtime(&t);
	if (stdStrCmpCI(stmt.commandName,L"date2"))
		year->set(time->tm_year%100);
	else
		year->set(time->tm_year+1900);
	month->set(time->tm_mon+1);
	month->set(time->tm_mday);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getmp3vol(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	dst->set(this->everything->audio->musicVolume(-1));
	return 0;
}

ErrorCode NONS_ScriptInterpreter::command_effect(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	long code,effect,timing=0;
	std::wstring rule;
	_GETINTVALUE(code,0,)
	if (this->gfx_store->retrieve(code))
		return NONS_DUPLICATE_EFFECT_DEFINITION;
	_GETINTVALUE(effect,1,)
	if (effect>255)
		return NONS_EFFECT_CODE_OUT_OF_RANGE;
	if (stmt.parameters.size()>2)
		_GETINTVALUE(timing,2,)
	if (stmt.parameters.size()>3)
		_GETWCSVALUE(rule,3,)
	NONS_GFX *gfx=this->gfx_store->add(code,effect,timing,&rule);
	gfx->stored=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_dim(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	std::vector<long> indices;
	this->store->evaluate(stmt.parameters[0],0,0,&indices,0,0);
	if (indices[0]>1)
		return NONS_UNDEFINED_ARRAY;
	for (size_t a=2;a<indices.size();a++)
		if (indices[a]<0)
			return NONS_NEGATIVE_INDEX_IN_ARRAY_DECLARATION;
	this->store->arrays[indices[1]]=new NONS_VariableMember(indices,2);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_atoi(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	std::wstring val;
	_GETWCSVALUE(val,1,)
	dst->atoi(val);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getversion(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	NONS_VariableMember *dst;
	_GETVARIABLE(dst,0,)
	if (dst->getType()==INTEGER)
		dst->set(ONSLAUGHT_BUILD_VERSION);
	else
		dst->set(ONSLAUGHT_BUILD_VERSION_WSTR);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_dwave(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	ulong size;
	long channel;
	_GETINTVALUE(channel,0,)
	if (channel<0 || channel>7)
		return NONS_INVALID_CHANNEL_INDEX;
	std::wstring name;
	_GETWCSVALUE(name,1,)
	tolower(name);
	toforwardslash(name);
	ErrorCode error;
	long loop=!stdStrCmpCI(stmt.commandName,L"dwave")?0:-1;
	if (this->everything->audio->bufferIsLoaded(name))
		error=this->everything->audio->playSoundAsync(&name,0,0,channel,loop);
	else{
		char *buffer=(char *)this->everything->archive->getFileBuffer(name,size);
		if (!buffer)
			return NONS_FILE_NOT_FOUND;
		error=this->everything->audio->playSoundAsync(&name,buffer,size,channel,loop);
	}
	return error;
}

ErrorCode NONS_ScriptInterpreter::command_dwaveload(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	long channel;
	_GETINTVALUE(channel,0,)
	if (channel<0)
		return NONS_INVALID_CHANNEL_INDEX;
	std::wstring name;
	_GETWCSVALUE(name,1,)
	tolower(name);
	toforwardslash(name);
	ErrorCode error=NONS_NO_ERROR;
	if (!this->everything->audio->bufferIsLoaded(name)){
		ulong size;
		char *buffer=(char *)this->everything->archive->getFileBuffer(name,size);
		if (!buffer)
			error=NONS_FILE_NOT_FOUND;
		else
			error=this->everything->audio->loadAsyncBuffer(name,buffer,size,channel);
	}
	return error;
}

ErrorCode NONS_ScriptInterpreter::command_getini(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(4);
	NONS_VariableMember *dst;
	_GETVARIABLE(dst,0,)
	std::wstring section,
		filename,
		key;
	_GETWCSVALUE(filename,0,);
	_GETWCSVALUE(section,1,)
	_GETWCSVALUE(key,2,)
	INIcacheType::iterator i=this->INIcache.find(filename);
	INIfile *file=0;
	if (i==this->INIcache.end()){
		ulong l;
		char *buffer=(char *)this->everything->archive->getFileBuffer(filename,l);
		if (!buffer)
			return NONS_FILE_NOT_FOUND;
		file=new INIfile(buffer,l,CLOptions.scriptencoding);
		this->INIcache[filename]=file;
	}else
		file=i->second;
	INIsection *sec=file->getSection(section);
	if (!sec)
		return NONS_INI_SECTION_NOT_FOUND;
	INIvalue *val=sec->getValue(key);
	if (!val)
		return NONS_INI_KEY_NOT_FOUND;
	switch (dst->getType()){
		case INTEGER:
			dst->set(val->getIntValue());
			break;
		case STRING:
			dst->set(val->getStrValue());
			break;
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_bg(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	NONS_ScreenSpace *scr=this->everything->screen;
	long color=0;
	if (!stdStrCmpCI(stmt.parameters[0],L"white")){
		scr->Background->setShade(-1,-1,-1);
		scr->Background->Clear();
	}else if (!stdStrCmpCI(stmt.parameters[0],L"black")){
		scr->Background->setShade(0,0,0);
		scr->Background->Clear();
	}else if (this->store->getIntValue(stmt.parameters[0],color)==NONS_NO_ERROR){
		char r=(color&0xFF0000)>>16,
			g=(color&0xFF00)>>8,
			b=(color&0xFF);
		scr->Background->setShade(r,g,b);
		scr->Background->Clear();
	}else{
		std::wstring filename;
		_GETWCSVALUE(filename,0,);
		scr->hideText();
		scr->Background->load(&filename);
		scr->Background->position.x=(scr->screen->virtualScreen->w-scr->Background->clip_rect.w)/2;
		scr->Background->position.y=(scr->screen->virtualScreen->h-scr->Background->clip_rect.h)/2;
	}
	scr->leftChar->unload();
	scr->rightChar->unload();
	scr->centerChar->unload();
	long number,duration;
	ErrorCode ret;
	_GETINTVALUE(number,1,)
	if (stmt.parameters.size()>2){
		std::wstring rule;
		_GETINTVALUE(duration,2,)
		if (stmt.parameters.size()>3)
			_GETWCSVALUE(rule,3,)
		ret=scr->BlendNoCursor(number,duration,&rule);
	}else
		ret=scr->BlendNoCursor(number);
	return ret;
}

ErrorCode NONS_ScriptInterpreter::command_br(NONS_Statement &stmt){
	return this->Printer(L"");
}

ErrorCode NONS_ScriptInterpreter::command_cl(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	switch (stmt.parameters[0][0]){
		case 'l':
			if (this->hideTextDuringEffect)
				this->everything->screen->hideText();
			this->everything->screen->leftChar->unload();
			break;
		case 'r':
			if (this->hideTextDuringEffect)
				this->everything->screen->hideText();
			this->everything->screen->rightChar->unload();
			break;
		case 'c':
			if (this->hideTextDuringEffect)
				this->everything->screen->hideText();
			this->everything->screen->centerChar->unload();
			break;
		case 'a':
			if (this->hideTextDuringEffect)
				this->everything->screen->hideText();
			this->everything->screen->leftChar->unload();
			this->everything->screen->rightChar->unload();
			this->everything->screen->centerChar->unload();
			break;
		default:
			return NONS_INVALID_PARAMETER;
	}
	long number,duration;
	ErrorCode ret;
	_GETINTVALUE(number,1,)
	if (stmt.parameters.size()>2){
		std::wstring rule;
		_GETINTVALUE(duration,2,)
		if (stmt.parameters.size()>3)
			_GETWCSVALUE(rule,3,)
		ret=this->everything->screen->BlendNoCursor(number,duration,&rule);
	}else
		ret=this->everything->screen->BlendNoCursor(number);
	return ret;
}

ErrorCode NONS_ScriptInterpreter::command_csp(NONS_Statement &stmt){
	long n=-1;
	if (stmt.parameters.size()>0)
		_GETINTVALUE(n,0,)
	if (n>0 && ulong(n)>=this->everything->screen->layerStack.size())
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	if (n<0){
		for (ulong a=0;a<this->everything->screen->layerStack.size();a++)
			if (this->everything->screen->layerStack[a] && this->everything->screen->layerStack[a]->data)
				this->everything->screen->layerStack[a]->unload();
	}else if (this->everything->screen->layerStack[n] && this->everything->screen->layerStack[n]->data)
		this->everything->screen->layerStack[n]->unload();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_delay(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	long delay;
	_GETINTVALUE(delay,0,)
	waitCancellable(delay);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_erasetextwindow(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	long yesno;
	_GETINTVALUE(yesno,0,)
	this->hideTextDuringEffect=!!yesno;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_for(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(3);
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	long from,to,step=1;
	_GETINTVALUE(from,1,)
	_GETINTVALUE(to,2,)
	if (stmt.parameters.size()>3)
		_GETINTVALUE(step,3,)
	if (!step)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	var->set(from);
	NONS_StackElement *element=new NONS_StackElement(var,this->thread->getNextStatementPair(),from,to,step,this->insideTextgosub());
	this->callStack.push_back(element);
	if (step>0 && from>to || step<0 && from<to)
		return this->command_break(stmt);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_break(NONS_Statement &stmt){
	if (this->callStack.empty())
		return NONS_EMPTY_CALL_STACK;
	NONS_StackElement *element=this->callStack.back();
	if (element->type!=FOR_NEST)
		return NONS_UNEXPECTED_NEXT;
	if (element->end!=element->returnTo){
		this->thread->gotoPair(element->returnTo.toPair());
		delete element;
		this->callStack.pop_back();
		return NONS_NO_ERROR;
	}
	std::pair<ulong,ulong> next=this->thread->getNextStatementPair();
	bool valid=0;
	while (!!(valid=this->thread->advanceToNextStatement())){
		NONS_Statement *pstmt=this->thread->getCurrentStatement();
		pstmt->parse(this->script);
		if (!stdStrCmpCI(pstmt->commandName,L"next"))
			break;
	}
	if (!valid){
		this->thread->gotoPair(next);
		return NONS_NO_NEXT;
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_effectblank(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	long a;
	_GETINTVALUE(a,0,)
	if (a<0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	NONS_GFX::effectblank=a;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_defaultspeed(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(3);
	long slow,med,fast;
	_GETINTVALUE(slow,0,)
	_GETINTVALUE(med,1,)
	_GETINTVALUE(fast,2,)
	if (slow<0 || med<0 || fast<0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	this->default_speed_slow=slow;
	this->default_speed_med=med;
	this->default_speed_fast=fast;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_clickstr(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	_GETWCSVALUE(this->clickStr,0,)
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_click(NONS_Statement &stmt){
	waitUntilClick();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_autoclick(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	long ms;
	_GETINTVALUE(ms,0,)
	if (ms<0)
		ms=0;
	this->autoclick=ms;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_gettimer(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	var->set(this->timer);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_btndef(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	if (this->imageButtons)
		delete this->imageButtons;
	this->imageButtons=0;
	if (!stdStrCmpCI(stmt.parameters[0],L"clear"))
		return NONS_NO_ERROR;
	std::wstring filename;
	_GETWCSVALUE(filename,0,)
	if (!filename.size()){
		SDL_Surface *tmpSrf=makeSurface(
			this->everything->screen->screen->virtualScreen->w,
			this->everything->screen->screen->virtualScreen->h,
			32);
		this->imageButtons=new NONS_ButtonLayer(tmpSrf,this->everything->screen);
		this->imageButtons->inputOptions.Wheel=this->useWheel;
		this->imageButtons->inputOptions.EscapeSpace=this->useEscapeSpace;
		return NONS_NO_ERROR;
	}
	SDL_Surface *img=ImageLoader->fetchSprite(filename);
	if (!img){
		return NONS_FILE_NOT_FOUND;
	}
	this->imageButtons=new NONS_ButtonLayer(img,this->everything->screen);
	this->imageButtons->inputOptions.Wheel=this->useWheel;
	this->imageButtons->inputOptions.EscapeSpace=this->useEscapeSpace;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_btn(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(7);
	if (!this->imageButtons)
		return NONS_NO_BUTTON_IMAGE;
	long index,butX,butY,width,height,srcX,srcY;
	_GETINTVALUE(index,0,)
	_GETINTVALUE(butX,1,)
	_GETINTVALUE(butY,2,)
	_GETINTVALUE(width,3,)
	_GETINTVALUE(height,4,)
	_GETINTVALUE(srcX,5,)
	_GETINTVALUE(srcY,6,)
	if (index<=0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	index--;
	this->imageButtons->addImageButton(index,butX,butY,width,height,srcX,srcY);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_btnwait(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	if (!this->imageButtons)
		return NONS_NO_BUTTON_IMAGE;
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	int choice=this->imageButtons->getUserInput(this->imageButtonExpiration);
	var->set(choice+1);
	this->btnTimer=SDL_GetTicks();
	if (choice>=0 && stdStrCmpCI(stmt.commandName,L"btnwait2")){
		delete this->imageButtons;
		this->imageButtons=0;
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getbtntimer(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	var->set(SDL_GetTicks()-this->btnTimer);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_btntime(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	long time;
	_GETINTVALUE(time,0,)
	if (time<0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	this->imageButtonExpiration=time;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_humanz(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	long z;
	_GETINTVALUE(z,0,)
	if (z<-1)
		z=-1;
	else if (ulong(z)>=this->everything->screen->layerStack.size())
		z=this->everything->screen->layerStack.size()-1;
	this->everything->screen->sprite_priority=z;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_filelog(NONS_Statement &stmt){
	ImageLoader->filelog.commit=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_centerh(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	long fraction;
	_GETINTVALUE(fraction,0,)
	this->everything->screen->output->setCenterPolicy('h',fraction);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_centerv(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	long fraction;
	_GETINTVALUE(fraction,0,)
	this->everything->screen->output->setCenterPolicy('v',fraction);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_blt(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(8);
	if (!this->imageButtons)
		return NONS_NO_BUTTON_IMAGE;
	long screenX,screenY,screenW,screenH,
		imgX,imgY,imgW,imgH;
	_GETINTVALUE(screenX,0,)
	_GETINTVALUE(screenY,1,)
	_GETINTVALUE(screenW,2,)
	_GETINTVALUE(screenH,3,)
	_GETINTVALUE(imgX,4,)
	_GETINTVALUE(imgY,5,)
	_GETINTVALUE(imgW,6,)
	_GETINTVALUE(imgH,7,)
	SDL_Rect dstRect={screenX,screenY,screenW,screenH},
		srcRect={imgX,imgY,imgW,imgH};
	void (*interpolationFunction)(SDL_Surface *,SDL_Rect *,SDL_Surface *,SDL_Rect *,ulong,ulong)=&nearestNeighborInterpolation;
	ulong x_multiplier=1,y_multiplier=1;
	if (imgW==screenW && imgH==screenH){
		LOCKSCREEN;
		manualBlit(this->imageButtons->loadedGraphic,&srcRect,this->everything->screen->screen->virtualScreen,&dstRect);
		UNLOCKSCREEN;
	}else{
		x_multiplier=(screenW<<8)/imgW;
		y_multiplier=(screenH<<8)/imgH;
		LOCKSCREEN;
		interpolationFunction(
			this->imageButtons->loadedGraphic,&srcRect,
			this->everything->screen->screen->virtualScreen,
			&dstRect,x_multiplier,y_multiplier
		);
		UNLOCKSCREEN;
	}
	this->everything->screen->screen->updateScreen(dstRect.x,dstRect.y,dstRect.w,dstRect.h);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_fileexist(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	std::wstring filename;
	_GETWCSVALUE(filename,1,)
	dst->set(this->everything->archive->exists(filename));
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_isdown(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	var->set(CHECK_FLAG(SDL_GetMouseState(0,0),SDL_BUTTON(SDL_BUTTON_LEFT)));
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_isfull(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	var->set(this->everything->screen->screen->fullscreen);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getcursorpos(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	NONS_VariableMember *x,*y;
	_GETINTVARIABLE(x,0,)
	_GETINTVARIABLE(y,1,)
	x->set(this->everything->screen->output->x);
	y->set(this->everything->screen->output->y);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_ispage(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	if (!this->insideTextgosub())
		dst->set(0);
	else{
		std::vector<NONS_StackElement *>::reverse_iterator i=this->callStack.rbegin();
		for (;i!=this->callStack.rend() && (*i)->type!=TEXTGOSUB_CALL;i++);
		dst->set((*i)->textgosubTriggeredBy=='\\');
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_cell(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	long sprt,
		cell;
	_GETINTVALUE(sprt,0,)
	_GETINTVALUE(cell,1,)
	if (sprt<0 || cell<0 || (ulong)sprt>=this->everything->screen->layerStack.size())
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	NONS_Layer *layer=this->everything->screen->layerStack[sprt];
	if (!layer || !layer->data)
		return NONS_NO_SPRITE_LOADED_THERE;
	if ((ulong)cell>=layer->animation.animation_length)
		cell=layer->animation.animation_length-1;
	layer->animation.resetAnimation();
	if (!cell)
		return NONS_NO_ERROR;
	if (layer->animation.frame_ends.size()==1)
		layer->animation.advanceAnimation(layer->animation.frame_ends[0]*cell);
	else
		layer->animation.advanceAnimation(layer->animation.frame_ends[cell-1]);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_bgcopy(NONS_Statement &stmt){
	LOCKSCREEN;
	this->everything->screen->Background->load(this->everything->screen->screen->virtualScreen);
	UNLOCKSCREEN;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_draw(NONS_Statement &stmt){
	LOCKSCREEN;
	this->everything->screen->screen->blitToScreen(this->everything->screen->screenBuffer,0,0);
	UNLOCKSCREEN;
	this->everything->screen->screen->updateWholeScreen();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_drawbg(NONS_Statement &stmt){
	if (!this->everything->screen->Background && !this->everything->screen->Background->data)
		SDL_FillRect(this->everything->screen->screenBuffer,0,this->everything->screen->screenBuffer->format->Amask);
	else if (!stdStrCmpCI(stmt.commandName,L"drawbg"))
		manualBlit(
			this->everything->screen->Background->data,
			0,
			this->everything->screen->screenBuffer,
			&this->everything->screen->Background->position);
	else{
		MINIMUM_PARAMETERS(5);
		long x,y,
			xscale,yscale,
			angle;
		_GETINTVALUE(x,0,)
		_GETINTVALUE(y,1,)
		_GETINTVALUE(xscale,2,)
		_GETINTVALUE(yscale,3,)
		_GETINTVALUE(angle,4,)
		if (!(xscale*yscale))
			SDL_FillRect(this->everything->screen->screenBuffer,0,this->everything->screen->screenBuffer->format->Amask);
		else{
			SDL_Surface *src=this->everything->screen->Background->data;
			bool freeSrc=0;

			if (xscale<0 || yscale<0){
				SDL_Surface *dst=makeSurface(src->w,src->h,32);
				if (yscale>0)
					FlipSurfaceH(src,dst);
				else if (xscale>0)
					FlipSurfaceV(src,dst);
				else
					FlipSurfaceHV(src,dst);
				xscale=ABS(xscale);
				yscale=ABS(yscale);
				src=dst;
				freeSrc=1;
			}

			if (src->format->BitsPerPixel!=32){
				SDL_Surface *dst=makeSurface(src->w,src->h,32);
				manualBlit(src,0,dst,0);
				freeSrc=1;
			}
			SDL_Surface *dst=resizeFunction(src,src->w*xscale/100,src->h*yscale/100);
			if (freeSrc)
				SDL_FreeSurface(src);
			src=dst;
			dst=rotationFunction(src,double(angle)/180*M_PI);
			SDL_FreeSurface(src);
			src=dst;
			SDL_Rect dstR={
				-long(src->clip_rect.w/2)+x,
				-long(src->clip_rect.h/2)+y,
				0,0
			};
			manualBlit(src,0,this->everything->screen->screenBuffer,&dstR);
			SDL_FreeSurface(src);
		}
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_drawclear(NONS_Statement &stmt){
	SDL_FillRect(this->everything->screen->screenBuffer,0,this->everything->screen->screenBuffer->format->Amask);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_drawfill(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(3);
	long r,g,b;
	_GETINTVALUE(r,0,)
	_GETINTVALUE(g,1,)
	_GETINTVALUE(b,2,)
	r=ulong(r)&0xFF;
	g=ulong(g)&0xFF;
	b=ulong(b)&0xFF;
	SDL_Surface *dst=this->everything->screen->screenBuffer;
	Uint32 rmask=dst->format->Rmask,
		gmask=dst->format->Gmask,
		bmask=dst->format->Bmask,
		amask=dst->format->Amask,
		R=r,
		G=g,
		B=b;
	R=R|R<<8|R<<16|R<<24;
	G=G|G<<8|G<<16|G<<24;
	B=B|B<<8|B<<16|B<<24;
	SDL_FillRect(dst,0,R&rmask|G&gmask|B&bmask|amask);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_drawsp(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(5);
	long spriteno,
		cell,
		alpha,
		x,y,
		xscale=0,yscale=0,
		rotation,
		matrix_00=0,matrix_01=0,
		matrix_10=0,matrix_11=0;
	_GETINTVALUE(spriteno,0,)
	_GETINTVALUE(cell,1,)
	_GETINTVALUE(alpha,2,)
	_GETINTVALUE(x,3,)
	_GETINTVALUE(y,4,)
	ulong functionVersion=1;
	if (!stdStrCmpCI(stmt.commandName,L"drawsp2"))
		functionVersion=2;
	else if (!stdStrCmpCI(stmt.commandName,L"drawsp3"))
		functionVersion=3;
	switch (functionVersion){
		case 2:
			MINIMUM_PARAMETERS(8);
			_GETINTVALUE(xscale,5,)
			_GETINTVALUE(yscale,6,)
			_GETINTVALUE(rotation,7,)
			break;
		case 3:
			MINIMUM_PARAMETERS(9);
			_GETINTVALUE(matrix_00,5,)
			_GETINTVALUE(matrix_01,6,)
			_GETINTVALUE(matrix_10,7,)
			_GETINTVALUE(matrix_11,8,)
			break;
	}


	std::vector<NONS_Layer *> &sprites=this->everything->screen->layerStack;
	if (spriteno<0 || (ulong)spriteno>sprites.size())
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	NONS_Layer *sprite=sprites[spriteno];
	if (!sprite || !sprite->data)
		return NONS_NO_SPRITE_LOADED_THERE;
	SDL_Surface *src=sprite->data;
	if (cell<0 || (ulong)cell>=sprite->animation.animation_length)
		return NONS_NO_ERROR;
	if (functionVersion==2 && !(xscale*yscale))
		return NONS_NO_ERROR;


	SDL_Rect srcRect={
		src->w/sprite->animation.animation_length*cell,
		0,
		sprite->clip_rect.w,
		sprite->clip_rect.h
	};
	SDL_Rect dstRect={x,y,0,0};


	bool freeSrc=0;
	if (functionVersion>1){
		SDL_Surface *temp=makeSurface(srcRect.w,srcRect.h,32);
		manualBlit(src,&srcRect,temp,0);
		src=temp;
		freeSrc=1;
	}
	switch (functionVersion){
		case 2:
			{
				SDL_Surface *dst;
				if (xscale<0 || yscale<0){
					dst=makeSurface(srcRect.w,srcRect.h,32);
					if (yscale>0)
						FlipSurfaceH(src,dst);
					else if (xscale>0)
						FlipSurfaceV(src,dst);
					else
						FlipSurfaceHV(src,dst);
					SDL_FreeSurface(src);
					xscale=ABS(xscale);
					yscale=ABS(yscale);
					src=dst;
				}
				if (xscale!=100 || yscale!=100){
					dst=resizeFunction(src,src->w*xscale/100,src->h*yscale/100);
					SDL_FreeSurface(src);
					src=dst;
				}
				if (rotation){
					dst=rotationFunction(src,double(rotation)/180*M_PI);
					SDL_FreeSurface(src);
					src=dst;
				}
			}
			break;
		case 3:
			{
				float matrix[]={
					float(matrix_00)/1000.0f,
					float(matrix_01)/1000.0f,
					float(matrix_10)/1000.0f,
					float(matrix_11)/1000.0f
				};
				SDL_Surface *dst=applyTransformationMatrix(src,matrix);
				if (!dst)
					return NONS_BAD_MATRIX;
				SDL_FreeSurface(src);
				src=dst;
				break;
			}
	}
	if (functionVersion>1){
		srcRect.x=0;
		srcRect.y=0;
		srcRect.w=src->w;
		srcRect.h=src->h;
		dstRect.x-=srcRect.w/2;
		dstRect.y-=srcRect.h/2;
	}


	manualBlit(src,&srcRect,this->everything->screen->screenBuffer,&dstRect,(alpha<-0xFF)?-0xFF:((alpha>0xFF)?0xFF:alpha));


	if (freeSrc)
		SDL_FreeSurface(src);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_drawtext(NONS_Statement &stmt){
	NONS_ScreenSpace *scr=this->everything->screen;
	if (!scr->output->shadeLayer->useDataAsDefaultShade)
		multiplyBlend(
			scr->output->shadeLayer->data,0,
			scr->screenBuffer,
			&scr->output->shadeLayer->clip_rect);
	else
		manualBlit(
			scr->output->shadeLayer->data,0,
			scr->screenBuffer,
			&scr->output->shadeLayer->clip_rect);
	if (scr->output->shadowLayer)
		manualBlit(
			scr->output->shadowLayer->data,0,
			scr->screenBuffer,
			&scr->output->shadowLayer->clip_rect,
			scr->output->shadowLayer->alpha);
	manualBlit(
		scr->output->foregroundLayer->data,0,
		scr->screenBuffer,
		&scr->output->foregroundLayer->clip_rect,
		scr->output->foregroundLayer->alpha);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_allsphide(NONS_Statement &stmt){
	this->everything->screen->blendSprites=!stdStrCmpCI(stmt.commandName,L"allspresume");
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_humanorder(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	std::wstring order;
	_GETWCSVALUE(order,0,)
	std::vector<ulong> porder;
	bool found[3]={0};
	ulong offsets[26];
	offsets['l'-'a']=0;
	offsets['c'-'a']=1;
	offsets['r'-'a']=2;
	for (ulong a=0;a<order.size();a++){
		wchar_t c=NONS_tolower(order[a]);
		switch (c){
			case 'l':
			case 'c':
			case 'r':
				if (found[offsets[c-'a']])
					break;
				porder.push_back(offsets[c-'a']);
				found[offsets[c-'a']]=1;
				break;
			default:;
		}
	}
	std::reverse(porder.begin(),porder.end());
	if (stmt.parameters.size()==1){
		this->everything->screen->charactersBlendOrder=porder;
		return NONS_NO_ERROR;
	}
	long number,duration;
	ErrorCode ret;
	_GETINTVALUE(number,1,)
	if (stmt.parameters.size()>2){
		_GETINTVALUE(duration,2,)
		std::wstring rule;
		if (stmt.parameters.size()>3)
			_GETWCSVALUE(rule,3,)
		this->everything->screen->hideText();
		this->everything->screen->charactersBlendOrder=porder;
		ret=this->everything->screen->BlendNoCursor(number,duration,&rule);
	}else{
		this->everything->screen->hideText();
		this->everything->screen->charactersBlendOrder=porder;
		ret=this->everything->screen->BlendNoCursor(number);
	}
	return ret;
}

ErrorCode NONS_ScriptInterpreter::command_defsub(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	std::wstring name=stmt.parameters[0];
	trim_string(name);
	if (name[0]=='*')
		name=name.substr(name.find_first_not_of('*'));
	if (!isValidIdentifier(name))
		return NONS_INVALID_COMMAND_NAME;
	if (this->commandList.find(name)!=this->commandList.end())
		return NONS_DUPLICATE_COMMAND_DEFINITION_BUILTIN;
	if (this->userCommandList.find(name)!=this->userCommandList.end())
		return NONS_DUPLICATE_COMMAND_DEFINITION_USER;
	if (!this->script->blockFromLabel(name))
		return NONS_NO_SUCH_BLOCK;
	this->userCommandList.insert(name);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command___userCommandCall__(NONS_Statement &stmt){
	if (!this->gosub_label(stmt.commandName))
		return NONS_UNDEFINED_ERROR;
	NONS_StackElement *el=new NONS_StackElement(this->callStack.back(),stmt.parameters);
	delete this->callStack.back();
	this->callStack.back()=el;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getparam(NONS_Statement &stmt){
	std::vector<std::wstring> *parameters=0;
	for (ulong a=this->callStack.size()-1;a<this->callStack.size() && !parameters;a--)
		if (this->callStack[a]->type==USERCMD_CALL)
			parameters=&this->callStack[a]->parameters;
	if (!parameters)
		return NONS_NOT_IN_A_USER_COMMAND_CALL;
	ErrorCode error;
	std::vector<std::pair<NONS_VariableMember *,std::pair<long,std::wstring> > > actions;
	for (ulong a=0;a<parameters->size() && a<stmt.parameters.size();a++){
		wchar_t c=NONS_tolower(stmt.parameters[a][0]);
		if (c=='i' || c=='s'){
			NONS_VariableMember *src=this->store->retrieve((*parameters)[a],&error),
				*dst;

			if (!src)
				return error;
			if (src->isConstant())
				return NONS_EXPECTED_VARIABLE;
			if (src->getType()==INTEGER_ARRAY)
				return NONS_EXPECTED_SCALAR;

			dst=this->store->retrieve(stmt.parameters[a].substr(1),&error);
			if (!dst)
				return error;
			if (dst->isConstant())
				return NONS_EXPECTED_VARIABLE;
			if (dst->getType()==INTEGER_ARRAY)
				return NONS_EXPECTED_SCALAR;
			if (dst->getType()!=INTEGER)
				return NONS_EXPECTED_NUMERIC_VARIABLE;

			Sint32 index=this->store->getVariableIndex(src);
			actions.resize(actions.size()+1);
			actions.back().first=dst;
			actions.back().second.first=index;
		}else{
			NONS_VariableMember *dst=this->store->retrieve(stmt.parameters[a],&error);

			if (!dst)
				return error;
			if (dst->isConstant())
				return NONS_EXPECTED_VARIABLE;
			if (dst->getType()==INTEGER_ARRAY)
				return NONS_EXPECTED_SCALAR;
			if (dst->getType()==INTEGER){
				long val;
				_HANDLE_POSSIBLE_ERRORS(this->store->getIntValue((*parameters)[a],val),)
				actions.resize(actions.size()+1);
				actions.back().first=dst;
				actions.back().second.first=val;
			}else{
				std::wstring val;
				_HANDLE_POSSIBLE_ERRORS(this->store->getWcsValue((*parameters)[a],val),)
				actions.resize(actions.size()+1);
				actions.back().first=dst;
				actions.back().second.second=val;
			}
		}
	}

	for (ulong a=0;a<actions.size();a++){
		if (actions[a].first->getType()==INTEGER)
			actions[a].first->set(actions[a].second.first);
		else
			actions[a].first->set(actions[a].second.second);
	}

	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getcursor(NONS_Statement &stmt){
	if (!this->imageButtons)
		return NONS_NO_BUTTON_IMAGE;
	this->imageButtons->inputOptions.Cursor=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getpage(NONS_Statement &stmt){
	if (!this->imageButtons)
		return NONS_NO_BUTTON_IMAGE;
	this->imageButtons->inputOptions.PageUpDown=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getenter(NONS_Statement &stmt){
	if (!this->imageButtons)
		return NONS_NO_BUTTON_IMAGE;
	this->imageButtons->inputOptions.Enter=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_gettab(NONS_Statement &stmt){
	if (!this->imageButtons)
		return NONS_NO_BUTTON_IMAGE;
	this->imageButtons->inputOptions.Tab=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getfunction(NONS_Statement &stmt){
	if (!this->imageButtons)
		return NONS_NO_BUTTON_IMAGE;
	this->imageButtons->inputOptions.Function=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getinsert(NONS_Statement &stmt){
	if (!this->imageButtons)
		return NONS_NO_BUTTON_IMAGE;
	this->imageButtons->inputOptions.Insert=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getzxc(NONS_Statement &stmt){
	if (!this->imageButtons)
		return NONS_NO_BUTTON_IMAGE;
	this->imageButtons->inputOptions.ZXC=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_indent(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	long indent;
	_GETINTVALUE(indent,0,)
	if (indent<0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	this->everything->screen->output->indentationLevel=indent;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getscreenshot(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	long w,h;
	_GETINTVALUE(w,0,)
	_GETINTVALUE(h,1,)
	if (w<=0 || h<=0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	if (!!this->screenshot)
		SDL_FreeSurface(this->screenshot);
	LOCKSCREEN;
	SDL_Surface *scr=this->everything->screen->screen->virtualScreen;
	if (scr->format->BitsPerPixel<32){
		SDL_Surface *temp=makeSurface(scr->w,scr->h,32);
		manualBlit(scr,0,temp,0);
		UNLOCKSCREEN;
		this->screenshot=SDL_ResizeSmooth(temp,w,h);
		SDL_FreeSurface(temp);
	}else{
		this->screenshot=SDL_ResizeSmooth(scr,w,h);
		UNLOCKSCREEN;
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_deletescreenshot(NONS_Statement &stmt){
	if (!!this->screenshot)
		SDL_FreeSurface(this->screenshot);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_gettext(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(1);
	NONS_VariableMember *dst;
	_GETSTRVARIABLE(dst,0,)
	std::wstring text=removeTags(this->everything->screen->output->currentBuffer);
	dst->set(text);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_checkpage(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	long page;
	_GETINTVALUE(page,1,)
	if (page<0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	dst->set(this->everything->screen->output->log.size()>=(ulong)page);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getlog(NONS_Statement &stmt){
	MINIMUM_PARAMETERS(2);
	NONS_VariableMember *dst;
	_GETSTRVARIABLE(dst,0,)
	long page;
	_GETINTVALUE(page,1,)
	NONS_StandardOutput *out=this->everything->screen->output;
	if (page<0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	if (!page)
		return this->command_gettext(stmt);
	if (out->log.size()<(ulong)page)
		return NONS_NOT_ENOUGH_LOG_PAGES;
	std::wstring text=removeTags(out->log[out->log.size()-page]);
	dst->set(text);
	return NONS_NO_ERROR;
}

/*ErrorCode NONS_ScriptInterpreter::command_(NONS_Statement &stmt){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_Statement &stmt){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_Statement &stmt){
}

ErrorCode NONS_ScriptInterpreter::command_(NONS_Statement &stmt){
}*/
#endif
