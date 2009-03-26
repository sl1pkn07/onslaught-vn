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

#ifndef NONS_PARSEDLINE_CPP
#define NONS_PARSEDLINE_CPP

#include "ParsedLine.h"
#include "../Functions.h"
#include "../UTF.h"
#include "../Globals.h"
#include <climits>

wchar_t *getLine(wchar_t *buffer,ulong *increment){
	bool singleline=0;
	if (*buffer==13 || *buffer==10){
		*increment=0;
		return copyWString("");
	}
	if (multicomparison(*buffer,L";*`") || *buffer>0x7F)
		singleline=1;
	long l=0,finallength=0;
	std::wstring res;
	res.reserve(2048);
	while (1){
		while (buffer[l]){
			if (multicomparison(buffer[l],L"\"`")){
				l++;
				for (wchar_t delimiter=buffer[l-1];buffer[l] && buffer[l]!=delimiter && buffer[l]!=13 && buffer[l]!=10;l++);
			}
			if (!buffer[l] || buffer[l]==13 || buffer[l]==10 || buffer[l]==';')
				break;
			if (buffer[l]==':'){
				l++;
				break;
			}
			l++;
		}
		if (!l){
			*increment=0;
			return copyWString("");
		}
		for (;iswhitespace(buffer[l-1]);l--);
		finallength+=l;
		if (singleline){
			*increment=finallength;
			return copyWString(buffer,l);
		}
		res.append(buffer,l);
		if (!buffer[l] || !multicomparison(buffer[l-1],",/")){
			*increment=finallength;
			return copyWString(res.c_str());
		}
		if (buffer[l-1]=='/')
			res.erase(res.end());
		for (;buffer[l] && buffer[l]!=13 && buffer[l]!=10;l++,finallength++);
		for (;iswhitespace(buffer[l]);l++,finallength++);
		if (!buffer[l]){
			*increment=finallength;
			return copyWString(res.c_str());
		}
		buffer+=l;
		l=0;
	}
	/*T *res=copyWString(buffer,l);
	for (long a=0;a<l;a++){
		if (res[a]=='/' && (res[a+1]==13 || res[a+1]==10))
			res[a]=' ';
		else if (res[a]==13 || res[a]==10)
			res[a]=' ';
	}
	return res;*/
}

template <typename T>
void preparseIf(T *string,std::vector<T *> *vec){
	static const char *limiters[]={
		"abssetcursor","add","allsphide","allspresume","amsp","arc","atoi",
		"autoclick","automode_time","automode","avi","bar","barclear","bg",
		"bgcopy","bgcpy","bgm","bgmonce","bgmstop","bgmvol","blt","br","break",
		"btn","btndef","btndown","btntime","btntime2","btnwait","btnwait2",
		"caption","cell","cellcheckexbtn","cellcheckspbtn","checkpage","chvol",
		"cl","click","clickstr","clickvoice","cmp","cos","csel","cselbtn",
		"cselgoto","csp","date","dec","defaultfont","defaultspeed",
		"definereset","defmp3vol","defsevol","defsub","defvoicevol","delay",
		"dim","div","draw","drawbg","drawbg2","drawclear","drawfill","drawsp",
		"drawsp2","drawsp3","drawtext","dwave","dwaveload","dwaveloop",
		"dwaveplay","dwaveplayloop","dwavestop","effect","effectblank",
		"effectcut","end","erasetextwindow","exbtn_d","exbtn","exec_dll",
		"existspbtn","fileexist","filelog","for","game","getbgmvol",
		"getbtntimer","getcselnum","getcselstr","getcursor","getcursorpos",
		"getenter","getfunction","getinsert","getlog","getmousepos","getmp3vol",
		"getpage","getpageup","getparam","getreg","getret","getscreenshot",
		"getsevol","getspmode","getspsize","gettab","gettag","gettext",
		"gettimer","getversion","getvoicevol","getzxc","globalon","gosub",
		"goto","humanorder","humanz","if","inc","indent","input","insertmenu",
		"intlimit","isdown","isfull","ispage","isskip","itoa","itoa2","jumpb",
		"jumpf","kidokumode","kidokuskip","labellog","layermessage","ld","len",
		"linepage","linepage2","loadgame","loadgosub","locate","logsp","logsp2",
		"lookbackbutton","lookbackcolor","lookbackflush","lookbacksp","loopbgm",
		"loopbgmstop","lr_trap","lsp","lsph","maxkaisoupage","menu_automode",
		"menu_full","menu_window","menuselectcolor","menuselectvoice",
		"menusetwindow","mid","mod","mode_ext","mode_saya","monocro","mov",
		"mov10","mov3","mov4","mov5","mov6","mov7","mov8","mov9",
		"movemousecursor","movl","mp3","mp3fadeout","mp3loop","mp3save",
		"mp3stop","mp3vol","mpegplay","msp","mul","nega","next","notif","ns2",
		"ns3","nsa","nsadir","numalias","ofscopy","ofscpy","play","playonce",
		"playstop","pretextgosub","print","prnum","prnumclear","puttext",
		"quake","quakex","quakey","repaint","reset","resetmenu","resettimer",
		"return","rmenu","rmode","rnd","rnd2","roff","rubyoff","rubyon","sar",
		"savefileexist","savegame","savename","savenumber","saveoff","saveon",
		"savescreenshot","savescreenshot2","savetime","select","selectbtnwait",
		"selectcolor","selectvoice","selgosub","selnum","setcursor","setlayer",
		"setwindow","setwindow2","setwindow3","sevol","shadedistance","sin",
		"skip","skipoff","soundpressplgin","sp_rgb_gradation","spbtn","spclclk",
		"spi","split","splitstring","spreload","spstr","stop","stralias",
		"strsp","sub","systemcall","tablegoto","tal","tan","tateyoko","texec",
		"textbtnwait","textclear","textgosub","texthide","textoff","texton",
		"textshow","textspeed","time","transmode","trap","underline",
		"useescspc","usewheel","versionstr","voicevol","vsp","wait","waittimer",
		"wave","waveloop","wavestop","windowback","windoweffect","zenkakko",
		"date2","getini","new_set_window","set_default_font_size","unalias",
		"literal_print","use_new_if","centerh","centerv","killmenu",0
	};
	long end=-1,minend=LONG_MAX;
	for (long a=0;limiters[a];a++){
		long offset=0;
		while (1){
			end=instr(string+offset,limiters[a]);
			if (end<0)
				break;
			if (!multicomparison(string[end+offset-1]," \t:") && string[end+offset-1]!=0){
				offset+=end+1;
				continue;
			}
			long len=end+strlen(limiters[a])+offset;
			if (!multicomparison(string[len]," \t:\0") && string[len]!=0){
				offset+=end+1;
				continue;
			}
			break;
		}
		if (end<0)
			continue;
		end+=offset;
		if (end<minend)
			minend=end;
	}
	if (minend==LONG_MAX){
		v_stderr <<"ERROR: Could not make sense of if: ";
		char *temp=copyString(string);
		v_stderr <<temp<<std::endl;
		delete[] temp;
		return;//copyString("");
	}
	end=minend;
	long start=end;
	for (;string[end-1] && (string[end-1]==' ' || string[end-1]=='\t');end--);
	vec->push_back(copyWString(string,end));
	string+=start;
	end=STRLEN(string);
	for (;string[end-1] && (string[end-1]==' ' || string[end-1]=='\t');end--);
	vec->push_back(copyWString(string,end));
}

void parseFor(wchar_t *string,std::vector<wchar_t *> *vec){
	for (;*string && iswhitespace((char)*string);string++);
	long l=0;
	for (;string[l] && string[l]!=';' && string[l]!='=' && !iswhitespace((char)string[l]);l++);
	if (!string[l] || string[l]==';')
		return;
	vec->push_back(copyWString(string,l));
	for (string+=l;string[l] && iswhitespace((char)*string);string++);
	if (*string!='=')
		return;
	for (string++;*string && iswhitespace((char)*string);string++);
	if (*string==';')
		return;
	for (l=0;string[l] && string[l]!=';' && !iswhitespace((char)string[l]);l++);
	if (!string[l] || string[l]==';')
		return;
	vec->push_back(copyWString(string,l));
	for (string+=l;string[l] && iswhitespace((char)*string);string++);
	if (instr(string,"to"))
		return;
	for (string+=2;*string && iswhitespace((char)*string);string++);
	if (*string==';')
		return;
	for (l=0;string[l] && string[l]!=';' && !iswhitespace((char)string[l]);l++);
	if (string[l]==';')
		return;
	vec->push_back(copyWString(string,l));
	for (string+=l;*string && iswhitespace((char)*string);string++);
	if (instr(string,"step"))
		return;
	for (string+=4;*string && iswhitespace((char)*string);string++);
	for (l=0;string[l] && string[l]!=';' && !iswhitespace((char)string[l]);l++);
	if (string[l]==';')
		return;
	vec->push_back(copyWString(string,l));
}

void parseLiteral_print(wchar_t *string,std::vector<wchar_t *> *vec){
	//ulong len=sizeof(T)==1?strlen((char *)string):wcslen((wchar_t *)string);
	ulong len=STRLEN(string);
	wchar_t delim=',';
	wchar_t tempDelim=delim;
	for (ulong start=0;start<len;){
		//for (;start<len && string[start];);
		ulong end;
		for (;iswhitespace((char)string[start]);start++);
		if (string[start]==';')
			return;
		if (string[start]=='\"' || string[start]=='`')
			delim=string[start];
		if (delim==tempDelim){
			for (end=start+1;string[end] && string[end]!=delim;end++)
				if (string[end]==';')
					break;
		}else{
			for (end=start+1;string[end];end++){
				if (string[end]=='\\'){
					end++;
					if (multicomparison(string[end],"\\`\"nrx"))
						continue;
					end--;
				}
				if (string[end]==delim)
					break;
			}
			end++;
		}
		for (;iswhitespace((char)string[end]);end--);
		bool finishnow=0;
		if (string[end]==';'){
			end--;
			finishnow=1;
			while (end && string[end]==' ' || string[end]=='\t' || string[end]==13 || string[end]==10)
				end--;
			end++;
		}
		wchar_t *el=copyWString(string+start,end-start);
		vec->push_back(el);
		if (finishnow)
			return;
		delim=tempDelim;
		for (start=end;string[start]==delim;start++);
	}
}

template <typename T>
std::vector<T *> *parseCommandParameters(T *string,T delim=' '){
	std::vector<T *> *res=new std::vector<T *>;
	//ulong len=sizeof(T)==1?strlen((char *)string):wcslen((wchar_t *)string);
	ulong len=STRLEN(string);
	T tempDelim=delim;
	for (ulong start=0;start<len;){
		//for (;start<len && string[start];);
		ulong end;
		for (;iswhitespace(string[start]);start++);
		if (string[start]==';')
			return res;
		if (string[start]=='\"' || string[start]=='`')
			delim=string[start];
		if (delim==tempDelim){
			/*
			If delim==tempDelim, it means that delim isn't any of the quote
			symbols. Therefore, if a semicolon was to be found here, it would
			mean it's not part of a string constant, and parsing doesn't need to
			continue.
			*/
			for (end=start+1;string[end] && string[end]!=delim;end++);
				/*if (string[end]==';')
					break;*/
		}else{
			for (end=start+1;string[end] && string[end]!=delim ;end++);
			end++;
		}
		ulong comma=end;
		for (;end>0 && iswhitespace(string[end-1]);end--);
		bool finishnow=0;
		/*if (string[end]==';'){
			end--;
			finishnow=1;
			while (end && string[end]==' ' || string[end]=='\t' || string[end]==13 || string[end]==10)
				end--;
			end++;
		}*/
		T *el=copyWString(string+start,end-start);
		res->push_back(el);
		if (finishnow)
			return res;
		delim=tempDelim;
		for (start=comma;string[start]==delim;start++);
	}
	return res;
}

template <typename T>
void parseCommandParameters(T *string,std::vector<T *> *vec,T delim=' '){
	std::vector<T *> *temp=parseCommandParameters(string,delim);
	ulong l=temp->size();
	vec->resize(l);
	for (ulong a=0;a<l;a++)
		(*vec)[a]=(*temp)[a];
	delete temp;
}

NONS_ParsedLine::NONS_ParsedLine(wchar_t *buffer,ulong *offset,ulong number){
	this->lineNo=number;
	this->error=NONS_NO_ERROR;
	this->CstringParameters=0;
	ulong increment=0;
	this->commandName=getLine(buffer+*offset,&increment);
	ulong len=wcslen(this->commandName);
	wchar_t *string=this->commandName;
	if (iswhitespace((char)this->commandName[0])){
		long start=0;
		for (;iswhitespace((char)string[start]) && string[start];start++);
		string=copyWString(string+start);
		delete[] this->commandName;
		this->commandName=string;
	}
	if (multicomparison(*this->commandName,";*~`?%$!\\@#") || *this->commandName>0x7F){
		this->CstringParameters=0;
		switch (*this->commandName){
			case ';':
				this->type=PARSEDLINE_COMMENT;
				break;
			case '*':
				this->type=PARSEDLINE_BLOCK;
				break;
			case '~':
				this->type=PARSEDLINE_JUMP;
				break;
			default:
				this->type=PARSEDLINE_PRINTER;
		}
	}else
		this->type=PARSEDLINE_COMMAND;
	//if (string starts with "if" or "notif")...
	bool isif=!instr(string,"if") || !instr(string,"notif");
	if (this->type!=PARSEDLINE_COMMAND || isif){
		(*offset)+=increment;
		if (isif){
			while (this->commandName[len-1]==':'){
				wchar_t *temp=getLine(buffer+*offset,&increment);
				addStringsInplace(&this->commandName,temp);
				string=this->commandName;
				len+=wcslen(temp);
				(*offset)+=increment;
			}
		}
		for (;buffer[*offset] && buffer[*offset]!=13 && buffer[*offset]!=10;(*offset)++);
		for (;iswhitespace(buffer[*offset]);(*offset)++);
		if (this->type!=PARSEDLINE_COMMAND)
			return;
	}else{
		if (!instr(string,"literal_print") || !instr(string,"centerv")){
			for (ulong a=0;string[a];a++){
				if (string[a]=='\"' || string[a]=='`'){
					for (wchar_t quote=string[a++];string[a];a++){
						if (string[a]=='\\'){
							a++;
							if (multicomparison(string[a],"\\`\"nrx"))
								continue;
							a--;
						}
						if (string[a]==quote)
							break;
					}
					if (!string[a]){
						this->type=PARSEDLINE_INVALID;
						this->error=NONS_UNMATCHED_QUOTES;
						return;
					}
					continue;
				}
				if (multicomparison(string[a],":\\")){
					len=a;
					/*string=copyWString(string,a);
					delete[] this->commandName;
					this->commandName=string;*/
					string[a]=0;
					break;
				}
			}
			(*offset)+=increment;
			if (buffer[*offset]==':')
				(*offset)++;
			else if (buffer[*offset]=='\\');
			else{
				for (;buffer[*offset] && buffer[*offset]!=13 && buffer[*offset]!=10;(*offset)++);
				for (;iswhitespace(buffer[*offset]);(*offset)++);
			}
		}else{
			for (ulong a=0;string[a];a++){
				if (string[a]=='\"' || string[a]=='`'){
					for (wchar_t quote=string[a++];string[a] && string[a]!=quote;a++);
					if (!string[a]){
						this->type=PARSEDLINE_INVALID;
						this->error=NONS_UNMATCHED_QUOTES;
						return;
					}
					continue;
				}
				if (multicomparison(string[a],":\\")){
					len=a;
					/*string=copyWString(string,a);
					delete[] this->commandName;
					this->commandName=string;*/
					string[a]=0;
					break;
				}
			}
			(*offset)+=increment;
			if (buffer[*offset]==':')
				(*offset)++;
			else if (buffer[*offset]=='\\');
			else{
				for (;buffer[*offset] && buffer[*offset]!=13 && buffer[*offset]!=10;(*offset)++);
				for (;iswhitespace(buffer[*offset]);(*offset)++);
			}
		}
	}
	if (!len){
		this->type=PARSEDLINE_EMPTY;
		delete[] this->commandName;
		this->commandName=0;
		return;
	}
	for (this->CstringParameters=this->commandName;*this->CstringParameters && !iswhitespace((char)*this->CstringParameters);this->CstringParameters++);
	if (*this->CstringParameters){
		*this->CstringParameters=0;
		this->CstringParameters++;
		for (;*this->CstringParameters && iswhitespace((char)*this->CstringParameters);this->CstringParameters++);
	}
	char *tempCopy=copyString(this->commandName);
	if (!strcmp(tempCopy,"if") || !strcmp(tempCopy,"notif"))
		preparseIf(this->CstringParameters,&this->parameters);
	else if (!strcmp(tempCopy,"for"))
		parseFor(this->CstringParameters,&this->parameters);
	else if (!strcmp(tempCopy,"literal_print"))
		parseLiteral_print(this->CstringParameters,&this->parameters);
	else
		parseCommandParameters<wchar_t>(this->CstringParameters,&this->parameters,',');
	delete[] tempCopy;
}

NONS_ParsedLine::NONS_ParsedLine(wchar_t *string,ulong number){
	this->lineNo=number;
	if (*string==';'){
		this->type=PARSEDLINE_COMMENT;
		return;
	}
	this->type=PARSEDLINE_COMMAND;
	this->commandName=copyWString(string);
	for (this->CstringParameters=this->commandName;*this->CstringParameters && *this->CstringParameters!=' ' && *this->CstringParameters!='\t';this->CstringParameters++);
	if (*this->CstringParameters){
		*this->CstringParameters=0;
		this->CstringParameters++;
		for (;*this->CstringParameters && (*this->CstringParameters==' ' || *this->CstringParameters=='\t');this->CstringParameters++);
	}
	parseCommandParameters<wchar_t>(this->CstringParameters,&this->parameters,',');
}

NONS_ParsedLine::~NONS_ParsedLine(){
	if (this->commandName)
		delete[] this->commandName;
	for (ulong a=0;a<this->parameters.size();a++)
		delete[] this->parameters[a];
}

ulong NONS_ParsedLine::nextLine(wchar_t *buffer,ulong offset){
	ulong increment=0;
	wchar_t *temp=getLine(buffer+offset,&increment);
	ulong res=offset+increment;
	delete[] temp;
	for (;buffer[res]!=13 && buffer[res]!=10;res++);
	for (;buffer[res]==13 || buffer[res]==10;res++);
	return res;
}

ulong NONS_ParsedLine::nextStatement(wchar_t *buffer,ulong offset){
	ulong previous=offset;
	while (1){
		NONS_ParsedLine line(buffer,&offset);
		if (line.type==PARSEDLINE_COMMAND)
			return previous;
		previous=offset;
	}
	/*wchar_t *line=getLine(buffer+offset),*string=line;
	ulong len=wcslen(line);
	ulong res=offset;
	if (iswhitespace((char)line[0])){
		string=line;
		for (;iswhitespace((char)*string) && *string;string++);
	}
	//if (string starts with "if" or "notif")...
	if (string[0]=='i' && string[1]=='f' || string[0]=='n' && string[1]=='o' && string[2]=='t' && string[3]=='i' && string[4]=='f'){
		res+=len;
		for (;buffer[res]!=13 && buffer[res]!=10;res++);
		for (;buffer[res]==13 || buffer[res]==10;res++);
	}else{
		ulong a;
		for (a=0;string[a];a++){
			if (string[a]=='\"' || string[a]=='`'){
				for (wchar_t quote=string[a];string[a] && string[a]!=quote;a++);
				if (!string[a]){
					res+=len;
					return res;
				}
				continue;
			}
			if (string[a]==':'){
				len=a;
				string[len]=0;
				break;
			}
		}
		res+=len;
		if (!string[a]){
			for (;buffer[res]!=13 && buffer[res]!=10;res++);
			for (;buffer[res]==13 || buffer[res]==10;res++);
		}else if (line[res]==':')
			res++;
		else{
			for (;buffer[res] && buffer[res]!=13 && buffer[res]!=10;res++);
			for (;buffer[res] && buffer[res]==13 || buffer[res]==10;res++);
		}
	}
	delete[] line;
	return res;*/
}

ulong NONS_ParsedLine::previousLine(wchar_t *buffer,ulong offset){
	ulong res=offset;
	for (;res && buffer[res]!=13 && buffer[res]!=10;res--);
	for (;res && buffer[res]==13 || buffer[res]==10;res--);
	for (;res && buffer[res]!=13 && buffer[res]!=10;res--);
	if (res)
		res++;
	return res;
}
#endif
