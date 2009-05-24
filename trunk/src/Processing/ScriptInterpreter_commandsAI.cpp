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
#include <cctype>

#ifndef NONS_SCRIPTINTERPRETER_COMMANDSAI_CPP
#define NONS_SCRIPTINTERPRETER_COMMANDSAI_CPP
ErrorCode NONS_ScriptInterpreter::command_caption(NONS_ParsedLine &line){
	if (!line.parameters.size())
		SDL_WM_SetCaption("",0);
	else{
		wchar_t *temp0=0;
		_GETWCSVALUE(temp0,0,)
		char *temp1=WChar_to_UTF8(temp0);
		SDL_WM_SetCaption(temp1,0);
		delete[] temp0;
		delete[] temp1;
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_alias(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	/*if (this->interpreter_mode!=DEFINE)
		return NONS_NOT_IN_DEFINE_MODE;*/
	NONS_VariableMember *var=this->store->retrieve(line.parameters[0],0);
	if (!var){
		_CHECK_ID_NAME(line.parameters[0])
		NONS_VariableMember *val;
		if (line.parameters.size()>1){
			if (!wcscmp(line.commandName,L"numalias")){
				long temp;
				_GETINTVALUE(temp,1,)
				val=new NONS_VariableMember(INTEGER);
				val->set(temp);
			}else{
				wchar_t *temp=0;
				_GETWCSVALUE(temp,1,)
				val=new NONS_VariableMember(STRING);
				val->set(temp,1);
			}
			val->makeConstant();
		}
		this->store->constants[copyWString(line.parameters[0])]=val;
		return NONS_NO_ERROR;
	}
	if (var->isConstant())
		return NONS_DUPLICATE_CONSTANT_DEFINITION;
	return NONS_INVALID_ID_NAME;
}

ErrorCode NONS_ScriptInterpreter::command_game(NONS_ParsedLine &line){
	this->interpreter_mode=NORMAL;
	long a=this->script->offsetFromBlock(L"*start");
	if (a==-1)
		return NONS_NO_START_LABEL;
	this->interpreter_position=a;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_goto(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	if (!this->goto_label(line.parameters[0]))
		return NONS_NO_SUCH_BLOCK;
	return NONS_NO_ERROR_BUT_BREAK;
}

ErrorCode NONS_ScriptInterpreter::command_globalon(NONS_ParsedLine &line){
	/*if (this->interpreter_mode!=DEFINE)
		return NONS_NOT_IN_DEFINE_MODE;*/
	this->store->commitGlobals=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_gosub(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	if (!this->gosub_label(line.parameters[0])){
		handleErrors(NONS_NO_SUCH_BLOCK,this->current_line,"NONS_ScriptInterpreter::command_gosub",1);
		return NONS_NO_SUCH_BLOCK;
	}
	return NONS_GOSUB;
}

ErrorCode NONS_ScriptInterpreter::command_if(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	long res=0;
	bool notif=!wcscmp(line.commandName,L"notif");
	ErrorCode ret=this->store->evaluate(line.parameters[0],&res,notif && !this->new_if,0,0,0);
	if (!CHECK_FLAG(ret,NONS_NO_ERROR_FLAG))
		return ret;
	if (notif && this->new_if)
		res=!res;
	if (!res)
		return NONS_NO_ERROR;
	ret=NONS_NO_ERROR;
	wchar_t *ifblock=line.parameters[1];
	if (instr(ifblock,L":")<0)
		return this->interpretString(ifblock);
	ulong len=wcslen(ifblock);
	ulong commandstart=0;
	for (ulong a=0;a<len;a++){
		if (ifblock[a]=='\"' || ifblock[a]=='`'){
			wchar_t quote=ifblock[a];
			a++;
			for (;ifblock[a] && ifblock[a]!=quote;a++);
			if (!ifblock[a])
				return NONS_UNMATCHED_QUOTES;
			a++;
			continue;
		}
		if (ifblock[a]==':' || ifblock[a]==';'){
			wchar_t *copy=copyWString(ifblock+commandstart,a-commandstart);
			ErrorCode error=this->interpretString(copy);
			delete[] copy;
			if (error==NONS_GOSUB && ifblock[a+1])
				(this->callStack.back())->first_interpret_string=copyWString(ifblock+a+1);
			if (!CHECK_FLAG(error,NONS_NO_ERROR_FLAG)){
				handleErrors(error,-1,"NONS_ScriptInterpreter::command_if",1);
				ret=NONS_UNDEFINED_ERROR;
			}
			if (CHECK_FLAG(error,NONS_BREAK_WORTHY_ERROR) || ifblock[a]==';')
				return ret;
			commandstart=a+1;
		}
	}
	wchar_t *copy=copyWString(ifblock+commandstart);
	ErrorCode error=this->interpretString(copy);
	if (error==NONS_END)
		return error;
	if (!CHECK_FLAG(error,NONS_NO_ERROR_FLAG)){
		handleErrors(error,-1,"NONS_ScriptInterpreter::command_if",1);
		ret=NONS_UNDEFINED_ERROR;
	}
	delete[] copy;
	return ret;
}

ErrorCode NONS_ScriptInterpreter::command_add(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	long val;
	_GETINTVALUE(val,1,)
	while (1){
		if (!wcscmp(line.commandName,L"add")){
			var->add(val);
			break;
		}
		if (!wcscmp(line.commandName,L"sub")){
			var->sub(val);
			break;
		}
		if (!wcscmp(line.commandName,L"mul")){
			var->mul(val);
			break;
		}
		if (!wcscmp(line.commandName,L"div")){
			if (!val)
				return NONS_DIVISION_BY_ZERO;
			var->div(val);
			break;
		}
		if (!wcscmp(line.commandName,L"sin")){
			var->set(sin(M_PI*val/180.0)*1000.0);
			break;
		}
		if (!wcscmp(line.commandName,L"cos")){
			var->set(cos(M_PI*val/180.0)*1000.0);
			break;
		}
		if (!wcscmp(line.commandName,L"tan")){
			var->set(tan(M_PI*val/180.0)*1000.0);
			break;
		}
		if (!wcscmp(line.commandName,L"mod")){
			if (!val)
				return NONS_DIVISION_BY_ZERO;
			var->mod(val);
			break;
		}
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_inc(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	if (!wcscmp(line.commandName,L"inc"))
		var->inc();
	else
		var->dec();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_cmp(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	wchar_t *opA=0,*opB=0;
	_GETWCSVALUE(opA,1,)
	_GETWCSVALUE(opB,2,)
	var->set(wcscmp(opA,opB));
	delete[] opA;
	delete[] opB;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_itoa(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *dst;
	_GETSTRVARIABLE(dst,0,)
	long src;
	_GETINTVALUE(src,1,)
	dst->set(src);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_intlimit(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
	/*if (this->interpreter_mode!=DEFINE)
		return NONS_NOT_IN_DEFINE_MODE;*/
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	long lower,upper;
	_GETINTVALUE(lower,1,)
	_GETINTVALUE(upper,2,)
	dst->setlimits(lower,upper);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_end(NONS_ParsedLine &line){
	return NONS_END;
}

ErrorCode NONS_ScriptInterpreter::command_date(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *year,*month,*day;
	_GETINTVARIABLE(year,0,)
	_GETINTVARIABLE(month,0,)
	_GETINTVARIABLE(day,0,)
	time_t t=time(0);
	tm *time=localtime(&t);
	if (wcscmp(line.commandName,L"date2"))
		year->set(time->tm_year%100);
	else
		year->set(time->tm_year+1900);
	month->set(time->tm_mon+1);
	month->set(time->tm_mday);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getmp3vol(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	dst->set(this->everything->audio->musicVolume(-1));
	return 0;
}

ErrorCode NONS_ScriptInterpreter::command_effect(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	/*if (this->interpreter_mode!=DEFINE)
		return NONS_NOT_IN_DEFINE_MODE;*/
	long code,effect,timing=0;
	wchar_t *rule=0;
	_GETINTVALUE(code,0,)
	if (this->gfx_store->retrieve(code))
		return NONS_DUPLICATE_EFFECT_DEFINITION;
	_GETINTVALUE(effect,1,)
	if (effect>255)
		return NONS_EFFECT_CODE_OUT_OF_RANGE;
	if (line.parameters.size()>2)
		_GETINTVALUE(timing,2,)
	if (line.parameters.size()>3)
		_GETWCSVALUE(rule,3,)
	NONS_GFX *gfx=this->gfx_store->add(code,effect,timing,rule);
	gfx->stored=1;
	if (rule)
		delete[] rule;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_dim(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	std::vector<long> indices;
	this->store->evaluate(line.parameters[0],0,0,&indices,0,0);
	if (indices[0]>1)
		return NONS_UNDEFINED_ARRAY;
	for (size_t a=2;a<indices.size();a++)
		if (indices[a]<0)
			return NONS_NEGATIVE_INDEX_IN_ARRAY_DECLARATION;
	this->store->arrays[indices[1]]=new NONS_VariableMember(indices,2);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_atoi(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	char *val=0;
	_GETSTRVALUE(val,1,)
	dst->set(atoi(val));
	delete[] val;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getversion(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *dst;
	_GETVARIABLE(dst,0,)
	if (dst->getType()==INTEGER)
		dst->set(ONSLAUGHT_BUILD_VERSION);
	else
		dst->set(ONSLAUGHT_BUILD_VERSION_WSTR,0);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_dwave(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	long size,channel;
	_GETINTVALUE(channel,0,)
	if (channel<0 || channel>7)
		return NONS_INVALID_CHANNEL_INDEX;
	wchar_t *name=0;
	_GETWCSVALUE(name,1,)
	NONS_tolower(name);
	toforwardslash(name);
	ErrorCode error;
	long loop=!wcscmp(line.commandName,L"dwave")?0:-1;
	if (this->everything->audio->bufferIsLoaded(name))
		error=this->everything->audio->playSoundAsync(name,0,0,channel,loop);
	else{
		char *buffer=(char *)this->everything->archive->getFileBuffer(name,(ulong *)&size);
		if (!buffer){
			delete[] name;
			return NONS_FILE_NOT_FOUND;
		}
		error=this->everything->audio->playSoundAsync(name,buffer,size,channel,loop);
	}
	delete[] name;
	return error;
}

ErrorCode NONS_ScriptInterpreter::command_dwaveload(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	long channel;
	_GETINTVALUE(channel,0,)
	if (channel<0)
		return NONS_INVALID_CHANNEL_INDEX;
	wchar_t *name=0;
	_GETWCSVALUE(name,1,)
	NONS_tolower(name);
	toforwardslash(name);
	//Unused:
	//char *filename=copyString(name);
	ErrorCode error=NONS_NO_ERROR;
	if (!this->everything->audio->bufferIsLoaded(name)){
		long size;
		char *buffer=(char *)this->everything->archive->getFileBuffer(name,(ulong *)&size);
		if (!buffer)
			error=NONS_FILE_NOT_FOUND;
		else
			error=this->everything->audio->loadAsyncBuffer(name,buffer,size,channel);
	}
	delete[] name;
	return error;
}

ErrorCode NONS_ScriptInterpreter::command_getini(NONS_ParsedLine &line){
	if (line.parameters.size()<4)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *dst;
	_GETVARIABLE(dst,0,)
	char *filename=0;
	wchar_t *section=0,*key=0;
	_GETSTRVALUE(filename,0,);
	_GETWCSVALUE(section,1,delete[] filename;)
	_GETWCSVALUE(key,2,delete[] filename; delete[] section;)
	INIcacheType::iterator i=this->INIcache.find(filename);
	INIfile *file;
	if (i==this->INIcache.end()){
		long l;
		wchar_t *temp=copyWString(filename);
		char *buffer=(char *)this->everything->archive->getFileBuffer(temp,(ulong *)&l);
		delete[] temp;
		if (!buffer){
			delete[] filename;
			delete[] section;
			delete[] key;
			return NONS_FILE_NOT_FOUND;
		}
		INIfile *file=new INIfile(buffer,l,CLOptions.scriptencoding);
		this->INIcache[filename]=file;
	}else
		file=i->second;
	INIsection *sec=file->getSection(section);
	if (!section){
		delete[] filename;
		delete[] section;
		delete[] key;
		return NONS_INI_SECTION_NOT_FOUND;
	}
	INIvalue *val=sec->getValue(key);
	if (!val){
		delete[] filename;
		delete[] section;
		delete[] key;
		return NONS_INI_KEY_NOT_FOUND;
	}
	switch (val->getType()){
		case 'i':
			dst->set(val->getIntValue());
			break;
		case 's':
			dst->set(val->getWStrValue(),0);
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_bg(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	/*if (!this->everything->screen)
		this->setDefaultWindow();*/
	NONS_ScreenSpace *scr=this->everything->screen;
	long color=0;
	if (!wcscmp(line.parameters[0],L"white")){
		scr->Background->setShade(-1,-1,-1);
		scr->Background->Clear();
	}else if (!wcscmp(line.parameters[0],L"black")){
		scr->Background->setShade(0,0,0);
		scr->Background->Clear();
	}else if (this->store->getIntValue(line.parameters[0],&color)==NONS_NO_ERROR){
		char r=(color&0xFF0000)>>16,
			g=(color&0xFF00)>>8,
			b=(color&0xFF);
		scr->Background->setShade(r,g,b);
		scr->Background->Clear();
	}else{
		wchar_t *filename=0;
		_GETWCSVALUE(filename,0,);
		NONS_tolower(filename);
		scr->hideText();
		scr->Background->load(filename);
	}
	scr->leftChar->unload();
	scr->rightChar->unload();
	scr->centerChar->unload();
	long number,duration;
	wchar_t *rule=0;
	ErrorCode ret;
	_GETINTVALUE(number,1,)
	if (line.parameters.size()>2){
		_GETINTVALUE(duration,2,)
		if (line.parameters.size()>3)
			_GETWCSVALUE(rule,3,)
		ret=scr->BlendNoCursor(number,duration,rule);
		delete[] rule;
	}else
		ret=scr->BlendNoCursor(number);
	return ret;
}

ErrorCode NONS_ScriptInterpreter::command_br(NONS_ParsedLine &line){
	return this->Printer(L"");
}

ErrorCode NONS_ScriptInterpreter::command_cl(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	switch (line.parameters[0][0]){
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
	wchar_t *rule=0;
	ErrorCode ret;
	_GETINTVALUE(number,1,)
	if (line.parameters.size()>2){
		_GETINTVALUE(duration,2,)
		if (line.parameters.size()>3)
			_GETWCSVALUE(rule,3,)
		ret=this->everything->screen->BlendNoCursor(number,duration,rule);
		if (rule)
			delete[] rule;
	}else
		ret=this->everything->screen->BlendNoCursor(number);
	return ret;
}

ErrorCode NONS_ScriptInterpreter::command_csp(NONS_ParsedLine &line){
	long n=-1;
	if (line.parameters.size()>0)
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

ErrorCode NONS_ScriptInterpreter::command_delay(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long delay;
	_GETINTVALUE(delay,0,)
	waitCancellable(delay);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_erasetextwindow(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long yesno;
	_GETINTVALUE(yesno,0,)
	this->hideTextDuringEffect=!!yesno;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_for(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
	if (*line.parameters[0]!='%')
		return NONS_INVALID_PARAMETER;
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	long from,to,step=1;
	_GETINTVALUE(from,1,)
	_GETINTVALUE(to,2,)
	if (line.parameters.size()>3)
		_GETINTVALUE(step,3,)
	if (!step)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	var->set(from);
	NONS_StackElement *element=new NONS_StackElement(var,this->interpreter_position,from,to,step,this->insideTextgosub());
	this->callStack.push_back(element);
	if (step>0 && from>to || step<0 && from<to)
		return this->command_break(line);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_break(NONS_ParsedLine &line){
	if (this->callStack.empty())
		return NONS_EMPTY_CALL_STACK;
	NONS_StackElement *element=this->callStack.back();
	if (element->type!=FOR_NEST)
		return NONS_UNEXPECTED_NEXT;
	if (element->end!=element->offset){
		this->interpreter_position=element->end;
		delete element;
		this->callStack.pop_back();
		return NONS_NO_ERROR;
	}
	ulong pos=this->interpreter_position,
		len=this->script->length;
	wchar_t *buffer=this->script->script;
	while (1){
		for (;pos<len && iswhitespace((char)buffer[pos]);pos++);
		if (pos>=len)
			return NONS_NO_NEXT;
		if (!instr(buffer+pos,"next") && (multicomparison(buffer[pos+4]," \n\r"))){
			for (;pos<len && buffer[len]!=13 && buffer[len]!=10;len++);
			for (;pos<len && (buffer[len]==13 || buffer[len]==10);len++);
			this->interpreter_position=pos;
			return NONS_NO_ERROR;
		}
		for (;pos<len && buffer[len]!=13 && buffer[len]!=10;len++);
		for (;pos<len && (buffer[len]==13 || buffer[len]==10);len++);
		if (pos>=len)
			return NONS_NO_NEXT;
	}
}

ErrorCode NONS_ScriptInterpreter::command_effectblank(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long a;
	_GETINTVALUE(a,0,)
	if (a<0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	NONS_GFX::effectblank=a;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_defaultspeed(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
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

ErrorCode NONS_ScriptInterpreter::command_clickstr(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	_GETWCSVALUE(this->clickStr,0,)
	if (!*this->clickStr){
		delete[] this->clickStr;
		this->clickStr=0;
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_click(NONS_ParsedLine &line){
	waitUntilClick();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_autoclick(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long ms;
	_GETINTVALUE(ms,0,)
	if (ms<0)
		ms=0;
	this->autoclick=ms;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_gettimer(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	var->set(this->timer);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_btndef(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	if (this->imageButtons)
		delete this->imageButtons;
	this->imageButtons=0;
	if (!wcscmp(line.parameters[0],L"clear"))
		return NONS_NO_ERROR;
	wchar_t *filename=0;
	_GETWCSVALUE(filename,0,)
	if (!*filename){
		delete[] filename;
		SDL_Surface *tmpSrf=SDL_CreateRGBSurface(
			SDL_HWSURFACE|SDL_SRCALPHA,
			this->everything->screen->screen->virtualScreen->w,
			this->everything->screen->screen->virtualScreen->h,
			32,
			rmask,
			gmask,
			bmask,
			amask);
		this->imageButtons=new NONS_ButtonLayer(tmpSrf,this->everything->screen);
		return NONS_NO_ERROR;
	}
	SDL_Surface *img=ImageLoader->fetchSprite(filename);
	if (!img){
		delete[] filename;
		return NONS_FILE_NOT_FOUND;
	}
	/*if (!this->everything->screen)
		this->setDefaultWindow();*/
	this->imageButtons=new NONS_ButtonLayer(img,this->everything->screen);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_btn(NONS_ParsedLine &line){
	if (line.parameters.size()<7)
		return NONS_INSUFFICIENT_PARAMETERS;
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

ErrorCode NONS_ScriptInterpreter::command_btnwait(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	if (!this->imageButtons)
		return NONS_NO_BUTTON_IMAGE;
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	int choice=this->imageButtons->getUserInput(this->imageButtonExpiration);
	var->set(choice+1);
	this->btnTimer=SDL_GetTicks();
	if (choice>=0 && wcscmp(line.commandName,L"btnwait2")){
		delete this->imageButtons;
		this->imageButtons=0;
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getbtntimer(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	var->set(SDL_GetTicks()-this->btnTimer);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_btntime(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long time;
	_GETINTVALUE(time,0,)
	if (time<0)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	this->imageButtonExpiration=time;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_humanz(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long z;
	_GETINTVALUE(z,0,)
	if (z<-1)
		z=-1;
	else if (ulong(z)>=this->everything->screen->layerStack.size())
		z=this->everything->screen->layerStack.size()-1;
	this->everything->screen->sprite_priority=z;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_filelog(NONS_ParsedLine &line){
	ImageLoader->filelog.commit=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_centerh(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long fraction;
	_GETINTVALUE(fraction,0,)
	this->everything->screen->output->setCenterPolicy('h',fraction);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_centerv(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	long fraction;
	_GETINTVALUE(fraction,0,)
	this->everything->screen->output->setCenterPolicy('v',fraction);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_blt(NONS_ParsedLine &line){
	if (line.parameters.size()<8)
		return NONS_INSUFFICIENT_PARAMETERS;
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

ErrorCode NONS_ScriptInterpreter::command_fileexist(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *dst;
	_GETINTVARIABLE(dst,0,)
	wchar_t *filename=0;
	_GETWCSVALUE(filename,1,)
	dst->set(this->everything->archive->exists(filename));
	delete[] filename;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_isdown(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	var->set(CHECK_FLAG(SDL_GetMouseState(0,0),SDL_BUTTON(SDL_BUTTON_LEFT)));
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_isfull(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *var;
	_GETINTVARIABLE(var,0,)
	var->set(this->everything->screen->screen->fullscreen);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getcursorpos(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_VariableMember *x,*y;
	_GETINTVARIABLE(x,0,)
	_GETINTVARIABLE(y,1,)
	x->set(this->everything->screen->output->x);
	y->set(this->everything->screen->output->y);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_ispage(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
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

/*ErrorCode NONS_ScriptInterpreter::command_(NONS_ParsedLine &line){
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
