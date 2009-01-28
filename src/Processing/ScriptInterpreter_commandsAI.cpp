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
	if (this->interpreter_mode!=DEFINE && !this->language_extensions)
		return NONS_NOT_IN_DEFINE_MODE;
	if (!this->store->retrieve(line.parameters[0])){
		_CHECK_VARIABLE_NAME(line.parameters[0])
		NONS_Variable *val=new NONS_Variable();
		if (line.parameters.size()>1){
			if (!wcscmp(line.line,L"numalias")){
				_GETINTVALUE(val->intValue,1,)
				val->type='%';
			}else{
				_GETWCSVALUE(val->wcsValue,1,)
				val->type='$';
			}
		}
		this->store->variables[copyWString(line.parameters[0])]=val;
		return NONS_NO_ERROR;
	}else
		return NONS_DUPLICATE_VARIABLE_DEFINITION;
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
	long temp=this->script->offsetFromBlock(line.parameters[0]);
	if (temp<0)
		return NONS_NO_SUCH_BLOCK;
	labelsUsed.insert(copyWString(line.parameters[0]));
	this->interpreter_position=temp;
	return NONS_NO_ERROR_BUT_BREAK;
}

ErrorCode NONS_ScriptInterpreter::command_globalon(NONS_ParsedLine &line){
	if (this->interpreter_mode!=DEFINE && !this->language_extensions)
		return NONS_NOT_IN_DEFINE_MODE;
	this->store->commitGlobals=1;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_gosub(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_StackElement *el=new NONS_StackElement(this->interpreter_position,0);
	this->callStack.push_back(el);
	ErrorCode error=this->command_goto(line);
	if (!(error&NONS_NO_ERROR_FLAG)){
		this->callStack.pop_back();
		handleErrors(error,this->current_line,"NONS_ScriptInterpreter::command_gosub");
		return error;
	}else
		return NONS_GOSUB;
}

ErrorCode NONS_ScriptInterpreter::command_if(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	long res=0;
	bool notif=!wcscmp(line.line,L"notif");
	ErrorCode ret=this->store->evaluate(line.parameters[0],&res,notif && !this->new_if);
	if (!(ret&NONS_NO_ERROR_FLAG))
		return ret;
	if (notif && this->new_if)
		res=!res;
	if (!res)
		return NONS_NO_ERROR;
	ret=NONS_NO_ERROR;
	wchar_t *ifblock=line.parameters[1];
	if (instr(ifblock,L":")<0)
		return this->interpretString(ifblock);
	long len=wcslen(ifblock);
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
				(*(this->callStack.end()-1))->first_interpret_string=copyWString(ifblock+a+1);
			if (!(error&NONS_NO_ERROR_FLAG)){
				handleErrors(error,-1,"NONS_ScriptInterpreter::command_if");
				ret=NONS_UNDEFINED_ERROR;
			}
			if (error&NONS_BREAK_WORTHY_ERROR || ifblock[a]==';')
				return ret;
			commandstart=a+1;
		}
	}
	wchar_t *copy=copyWString(ifblock+commandstart);
	ErrorCode error=this->interpretString(copy);
	if (error==NONS_END)
		return error;
	if (!(error&NONS_NO_ERROR_FLAG)){
		handleErrors(error,-1,"NONS_ScriptInterpreter::command_if");
		ret=NONS_UNDEFINED_ERROR;
	}
	delete[] copy;
	return ret;
}

ErrorCode NONS_ScriptInterpreter::command_add(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_Variable *var=this->store->retrieve(line.parameters[0]);
	if (!var)
		return NONS_UNDEFINED_VARIABLE;
	long val;
	_GETINTVALUE(val,1,)
	while (1){
		if (!wcscmp(line.line,L"add")){
			var->add(val);
			break;
		}
		if (!wcscmp(line.line,L"sub")){
			var->sub(val);
			var->intValue-=val;
			break;
		}
		if (!wcscmp(line.line,L"mul")){
			var->mul(val);
			break;
		}
		if (!wcscmp(line.line,L"div")){
			if (!val)
				return NONS_DIVISION_BY_ZERO;
			var->div(val);
			break;
		}
		if (!wcscmp(line.line,L"sin")){
			var->set(sin(M_PI*val/180.0)*1000.0);
			break;
		}
		if (!wcscmp(line.line,L"cos")){
			var->set(cos(M_PI*val/180.0)*1000.0);
			break;
		}
		if (!wcscmp(line.line,L"tan")){
			var->set(tan(M_PI*val/180.0)*1000.0);
			break;
		}
		if (!wcscmp(line.line,L"mod")){
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
	NONS_Variable *var=this->store->retrieve(line.parameters[0]);
	if (!var)
		return NONS_UNDEFINED_VARIABLE;
	if (!wcscmp(line.line,L"inc"))
		var->inc();
	else
		var->dec();
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_cmp(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_Variable *var=this->store->retrieve(line.parameters[0]);
	if (!var)
		return NONS_UNDEFINED_VARIABLE;
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
	NONS_Variable *dst=this->store->retrieve(line.parameters[0]);
	if (!dst)
		return NONS_UNDEFINED_VARIABLE;
	long src;
	_GETINTVALUE(src,1,)
	char temp[100];
	sprintf(temp,"%d",src);
	if (dst->wcsValue)
		delete[] dst->wcsValue;
	dst->wcsValue=copyWString(temp);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_intlimit(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
	if (this->interpreter_mode!=DEFINE && !this->language_extensions)
		return NONS_NOT_IN_DEFINE_MODE;
	NONS_Variable *dst=this->store->retrieve(line.parameters[0]);
	if (!dst)
		return NONS_UNDEFINED_VARIABLE;
	long lower,upper;
	_GETINTVALUE(lower,1,)
	_GETINTVALUE(upper,2,)
	dst->_long_lower_limit=lower;
	dst->_long_upper_limit=upper;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_end(NONS_ParsedLine &line){
	return NONS_END;
}

ErrorCode NONS_ScriptInterpreter::command_date(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_Variable *year=this->store->retrieve(line.parameters[0]);
	NONS_Variable *month=this->store->retrieve(line.parameters[1]);
	NONS_Variable *day=this->store->retrieve(line.parameters[2]);
	if (!year || !month || !day)
		return NONS_UNDEFINED_VARIABLE;
	time_t t=time(0);
	tm *time=localtime(&t);
	if (wcscmp(line.line,L"date2"))
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
	NONS_Variable *dst=this->store->retrieve(line.parameters[0]);
	if (!dst)
		return NONS_UNDEFINED_VARIABLE;
	dst->set(this->everything->audio->musicVolume(-1));
	return 0;
}

ErrorCode NONS_ScriptInterpreter::command_effect(NONS_ParsedLine &line){
	if (line.parameters.size()<3)
		return NONS_INSUFFICIENT_PARAMETERS;
	if (this->interpreter_mode!=DEFINE && !this->language_extensions)
		return NONS_NOT_IN_DEFINE_MODE;
	long code,effect,timing;
	wchar_t *rule=0;
	_GETINTVALUE(code,0,)
	if (this->gfx_store->retrieve(code))
		return NONS_DUPLICATE_EFFECT_DEFINITION;
	_GETINTVALUE(effect,1,)
	if (effect<2 || effect>255)
		return NONS_EFFECT_CODE_OUT_OF_RANGE;
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
	if (line.parameters[0][0]!='?' && !this->language_extensions)
		return NONS_MISSING_Q_IN_ARRAY_DECLARATION;
	wchar_t *name=line.parameters[0];
	wchar_t *string=name;
	for (;*name!='?';name++);
	for (;*name=='?';name++);
	if (*name<=' ' || *name=='[')
		return NONS_INVALID_VARIABLE_NAME;
	long f=instr(string,L"[");
	if (f<0)
		return NONS_MISSING_B_IN_ARRAY_DECLARATION;
	name=copyWString(name,f-(name-string));
	if (this->store->arrayVariables.find(name)!=this->store->arrayVariables.end()){
		delete[] name;
		return NONS_DUPLICATE_VARIABLE_DEFINITION;
	}
	_CHECK_VARIABLE_NAME(name)
	std::vector<ulong> indices;
	do{
		f++;
		ulong start=f;
		ulong nesting=1;
		for (;string[f];f++){
			switch (string[f]){
				case '[':
					nesting++;
					break;
				case ']':
					nesting--;
			}
			if (!nesting)
				break;
		}
		if (nesting)
			return NONS_UNMATCHED_BRAKETS;
		wchar_t *copy=copyWString(string+start,f-start);
		long pusher;
		_HANDLE_POSSIBLE_ERRORS(this->store->evaluate(copy,&pusher),delete[] copy;)
		f++;
		f=instr(copy+f,L"[");
		delete[] copy;
		indices.push_back(pusher);
	}while (f>0);
	ulong size=indices.size();
	ulong *dimensions=new ulong[size];
	for (ulong a=0;a<size;a++)
		dimensions[a]=indices[a];
	NONS_Variable *var=new NONS_Variable(dimensions,size);
	this->store->arrayVariables[name]=var;
	delete[] dimensions;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_atoi(NONS_ParsedLine &line){
	if (line.parameters.size()<2)
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_Variable *dst=this->store->retrieve(line.parameters[0]);
	if (!dst)
		return NONS_UNDEFINED_VARIABLE;
	char *val=0;
	_GETSTRVALUE(val,1,)
	dst->set(atoi(val));
	delete[] val;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getversion(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_Variable *dst=this->store->retrieve(line.parameters[0]);
	if (!dst)
		return NONS_UNDEFINED_VARIABLE;
	dst->set(ONSLAUGHT_BUILD_VERSION);
	if (dst->wcsValue)
		delete[] dst->wcsValue;
	dst->wcsValue=copyWString(ONSLAUGHT_BUILD_VERSION_WSTR);
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
	tolower(name);
	toforwardslash(name);
	ErrorCode error;
	long loop=!wcscmp(line.line,L"dwave")?0:-1;
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
	tolower(name);
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
	NONS_Variable *dst=this->store->retrieve(line.parameters[0]);
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
			if (dst->wcsValue)
				delete[] dst->wcsValue;
			dst->wcsValue=val->getWStrValue();
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
	}else if (this->getIntValue(line.parameters[0],&color)==NONS_NO_ERROR){
		char r=(color&0xFF0000)>>16,
			g=(color&0xFF00)>>8,
			b=(color&0xFF);
		scr->Background->setShade(r,g,b);
		scr->Background->Clear();
	}else{
		wchar_t *filename=0;
		_GETWCSVALUE(filename,0,);
		tolower(filename);
		scr->hideText();
		scr->Background->load(filename,&(this->everything->screen->screen->virtualScreen->clip_rect),NO_ALPHA);
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
		ret=scr->BlendAll(number,duration,rule);
		delete[] rule;
	}else
		ret=scr->BlendAll(number);
	return ret;
}

ErrorCode NONS_ScriptInterpreter::command_br(NONS_ParsedLine &line){
	return this->Printer2(WSTRLITERAL(L""));
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
		ret=this->everything->screen->BlendAll(number,duration,rule);
		if (rule)
			delete[] rule;
	}else
		ret=this->everything->screen->BlendAll(number);
	return ret;
}

ErrorCode NONS_ScriptInterpreter::command_csp(NONS_ParsedLine &line){
	long n=-1;
	if (line.parameters.size()>0)
		_GETINTVALUE(n,0,)
	if (n>0 && n>=this->everything->screen->layerStack.size())
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	if (n<0){
		for (long a=0;a<this->everything->screen->layerStack.size();a++)
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
	NONS_Variable *var=this->store->retrieve(line.parameters[0]);
	if (!var)
		return NONS_UNDEFINED_VARIABLE;
	long from,to,step=1;
	_GETINTVALUE(from,1,)
	_GETINTVALUE(to,2,)
	if (line.parameters.size()>3)
		_GETINTVALUE(step,3,)
	if (!step)
		return NONS_INVALID_RUNTIME_PARAMETER_VALUE;
	var->intValue=from;
	NONS_StackElement *element=new NONS_StackElement(var,this->interpreter_position,from,to,step);
	this->callStack.push_back(element);
	if (step>0 && from>to || step<0 && from<to)
		return this->command_break(line);
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_break(NONS_ParsedLine &line){
	if (this->callStack.empty())
		return NONS_EMPTY_CALL_STACK;
	NONS_StackElement *element=*(this->callStack.end()-1);
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
	NONS_Variable *var=this->store->retrieve(line.parameters[0]);
	if (!var)
		return NONS_UNDEFINED_VARIABLE;
	var->intValue=this->timer;
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_btndef(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	if (this->imageButtons)
		delete this->imageButtons;
	this->imageButtons=0;
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
	SDL_Surface *img=ImageLoader->fetchSprite(filename,filename,NO_ALPHA);
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
	NONS_Variable *var=this->store->retrieve(line.parameters[0]);
	if (!var)
		return NONS_UNDEFINED_VARIABLE;
	int choice=this->imageButtons->getUserInput(this->imageButtonExpiration);
	var->set(choice+1);
	this->btnTimer=SDL_GetTicks();
	if (choice>=0){
		delete this->imageButtons;
		this->imageButtons=0;
	}
	return NONS_NO_ERROR;
}

ErrorCode NONS_ScriptInterpreter::command_getbtntimer(NONS_ParsedLine &line){
	if (!line.parameters.size())
		return NONS_INSUFFICIENT_PARAMETERS;
	NONS_Variable *var=this->store->retrieve(line.parameters[0]);
	if (!var)
		return NONS_UNDEFINED_VARIABLE;
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
	else if (z>=this->everything->screen->layerStack.size())
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
