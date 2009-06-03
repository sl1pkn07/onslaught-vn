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

std::wstring getLine(const std::wstring &buffer,ulong offset,ulong *increment){
	bool singleline=0;
	if (buffer[offset+0]==13 || buffer[offset+0]==10){
		*increment=0;
		return L"";
	}
	if (multicomparison(buffer[offset+0],L";*`") || buffer[offset+0]>0x7F)
		singleline=1;
	ulong off=0,l=0,finallength=0;
	std::wstring res;
	res.reserve(2048);
	while (1){
		while (buffer[offset+off+l]){
			if (multicomparison(buffer[offset+off+l],L"\"`")){
				l++;
				for (wchar_t delimiter=buffer[offset+off+l-1];buffer[offset+off+l] && buffer[offset+off+l]!=delimiter && buffer[offset+off+l]!=13 && buffer[offset+off+l]!=10;l++);
			}
			if (!buffer[offset+off+l] || buffer[offset+off+l]==13 || buffer[offset+off+l]==10 || buffer[offset+off+l]==';')
				break;
			if (buffer[offset+off+l]==':'){
				l++;
				break;
			}
			l++;
		}
		if (!l){
			*increment=0;
			return L"";
		}
		for (;iswhitespace(buffer[offset+off+l-1]);l--);
		finallength+=l;
		if (singleline){
			*increment=finallength;
			return std::wstring(buffer,offset+0,l);
		}
		res.append(std::wstring(buffer,offset+0,l));
		if (off+l>=buffer.size() || !multicomparison(buffer[offset+off+l-1],",/")){
			*increment=finallength;
			return res;
		}
		if (buffer[offset+off+l-1]=='/')
			res.erase(res.end());
		for (;off+l<buffer.size() && buffer[offset+off+l]!=13 && buffer[offset+off+l]!=10;l++,finallength++);
		for (;off+l<buffer.size() && iswhitespace(buffer[offset+off+l]);l++,finallength++);
		if (off+l>=buffer.size()){
			*increment=finallength;
			return res;
		}
		off+=l;
		l=0;
	}
}

void preparseIf(const std::wstring &string,std::vector<std::wstring> &vec){
	static const wchar_t *limiters[]={
		L"abssetcursor",L"add",L"allsphide",L"allspresume",L"amsp",L"arc",
		L"atoi",L"autoclick",L"automode_time",L"automode",L"avi",L"bar",
		L"barclear",L"bg",L"bgcopy",L"bgcpy",L"bgm",L"bgmonce",L"bgmstop",
		L"bgmvol",L"blt",L"br",L"break",L"btn",L"btndef",L"btndown",L"btntime",
		L"btntime2",L"btnwait",L"btnwait2",L"caption",L"cell",L"cellcheckexbtn",
		L"cellcheckspbtn",L"checkpage",L"chvol",L"cl",L"click",L"clickstr",
		L"clickvoice",L"cmp",L"cos",L"csel",L"cselbtn",L"cselgoto",L"csp",
		L"date",L"dec",L"defaultfont",L"defaultspeed",L"definereset",
		L"defmp3vol",L"defsevol",L"defsub",L"defvoicevol",L"delay",L"dim",
		L"div",L"draw",L"drawbg",L"drawbg2",L"drawclear",L"drawfill",L"drawsp",
		L"drawsp2",L"drawsp3",L"drawtext",L"dwave",L"dwaveload",L"dwaveloop",
		L"dwaveplay",L"dwaveplayloop",L"dwavestop",L"effect",L"effectblank",
		L"effectcut",L"end",L"erasetextwindow",L"exbtn_d",L"exbtn",L"exec_dll",
		L"existspbtn",L"fileexist",L"filelog",L"for",L"game",L"getbgmvol",
		L"getbtntimer",L"getcselnum",L"getcselstr",L"getcursor",L"getcursorpos",
		L"getenter",L"getfunction",L"getinsert",L"getlog",L"getmousepos",
		L"getmp3vol",L"getpage",L"getpageup",L"getparam",L"getreg",L"getret",
		L"getscreenshot",L"getsevol",L"getspmode",L"getspsize",L"gettab",
		L"gettag",L"gettext",L"gettimer",L"getversion",L"getvoicevol",L"getzxc",
		L"globalon",L"gosub",L"goto",L"humanorder",L"humanz",L"if",L"inc",
		L"indent",L"input",L"insertmenu",L"intlimit",L"isdown",L"isfull",
		L"ispage",L"isskip",L"itoa",L"itoa2",L"jumpb",L"jumpf",L"kidokumode",
		L"kidokuskip",L"labellog",L"layermessage",L"ld",L"len",L"linepage",
		L"linepage2",L"loadgame",L"loadgosub",L"locate",L"logsp",L"logsp2",
		L"lookbackbutton",L"lookbackcolor",L"lookbackflush",L"lookbacksp",
		L"loopbgm",L"loopbgmstop",L"lr_trap",L"lsp",L"lsph",L"maxkaisoupage",
		L"menu_automode",L"menu_full",L"menu_window",L"menuselectcolor",
		L"menuselectvoice",L"menusetwindow",L"mid",L"mod",L"mode_ext",
		L"mode_saya",L"monocro",L"mov",L"mov10",L"mov3",L"mov4",L"mov5",L"mov6",
		L"mov7",L"mov8",L"mov9",L"movemousecursor",L"movl",L"mp3",L"mp3fadeout",
		L"mp3loop",L"mp3save",L"mp3stop",L"mp3vol",L"mpegplay",L"msp",L"mul",
		L"nega",L"next",L"notif",L"ns2",L"ns3",L"nsa",L"nsadir",L"numalias",
		L"ofscopy",L"ofscpy",L"play",L"playonce",L"playstop",L"pretextgosub",
		L"print",L"prnum",L"prnumclear",L"puttext",L"quake",L"quakex",L"quakey",
		L"repaint",L"reset",L"resetmenu",L"resettimer",L"return",L"rmenu",
		L"rmode",L"rnd",L"rnd2",L"roff",L"rubyoff",L"rubyon",L"sar",
		L"savefileexist",L"savegame",L"savename",L"savenumber",L"saveoff",
		L"saveon",L"savescreenshot",L"savescreenshot2",L"savetime",L"select",
		L"selectbtnwait",L"selectcolor",L"selectvoice",L"selgosub",L"selnum",
		L"setcursor",L"setlayer",L"setwindow",L"setwindow2",L"setwindow3",
		L"sevol",L"shadedistance",L"sin",L"skip",L"skipoff",L"soundpressplgin",
		L"sp_rgb_gradation",L"spbtn",L"spclclk",L"spi",L"split",L"splitstring",
		L"spreload",L"spstr",L"stop",L"stralias",L"strsp",L"sub",L"systemcall",
		L"tablegoto",L"tal",L"tan",L"tateyoko",L"texec",L"textbtnwait",
		L"textclear",L"textgosub",L"texthide",L"textoff",L"texton",L"textshow",
		L"textspeed",L"time",L"transmode",L"trap",L"underline",L"useescspc",
		L"usewheel",L"versionstr",L"voicevol",L"vsp",L"wait",L"waittimer",
		L"wave",L"waveloop",L"wavestop",L"windowback",L"windoweffect",
		L"zenkakko",L"date2",L"getini",L"new_set_window",
		L"set_default_font_size",L"unalias",L"literal_print",L"use_new_if",
		L"centerh",L"centerv",L"killmenu",0
	};
	ulong end=string.npos,
		minend=ULONG_MAX;
	for (long a=0;limiters[a];a++){
		long offset=0;
		while (1){
			end=string.find(limiters[a],offset);
			if (end==string.npos)
				break;
			if (!multicomparison(string[end+offset-1]," \t:") && string[end+offset-1]!=0){
				offset+=end+1;
				continue;
			}
			ulong len=end+wcslen(limiters[a])+offset;
			if (!multicomparison(string[len]," \t:\0") && string[len]!=0){
				offset+=end+1;
				continue;
			}
			break;
		}
		if (end==string.npos)
			continue;
		end+=offset;
		if (end<minend)
			minend=end;
	}
	if (minend==ULONG_MAX){
		o_stderr <<"ERROR: Could not make sense of if: "<<string<<'\n';
		return;
	}
	end=minend;
	ulong start=end;
	for (;string[end-1] && (string[end-1]==' ' || string[end-1]=='\t');end--);
	vec.push_back(std::wstring(string,0,end));
	for (;string[start+string.size()-1-start] &&
		(string[start+string.size()-1-start]==' ' ||
		string[start+string.size()-1-start]=='\t');end--);
	vec.push_back(std::wstring(string,start,end));
}

std::vector<std::wstring> parseFor(const std::wstring &string){
	std::vector<std::wstring> res;
	std::wstring::const_iterator i=string.begin(),
		end=string.end();
	for (;i!=end && iswhitespace((char)*i);i++);
	std::wstring::const_iterator second=i;
	for (;second!=end && *second!=';' && *second!='=' && !iswhitespace(*second);second++);
	res.push_back(std::wstring(i,second));
	if (second==end || *second==';')
		return res;
	for (i=second;i!=end && iswhitespace(*i);i++);
	if (i==end || *i!='=')
		return res;
	for (i++;i!=end && iswhitespace(*i);i++);
	if (i==end || *i==';')
		return res;
	for (second=i;second!=end && *second!=';' && !iswhitespace(*second);second++);
	res.push_back(std::wstring(i,second));
	if (second==end || *second==';')
		return res;
	for (i=second;i!=end && iswhitespace(*i);i++);
	if (i==end || string.find(L"to",std::distance(string.begin(),i)))
		return res;
	for (i+=2;i!=end && iswhitespace(*i);i++);
	if (i==end || *i==';')
		return res;
	for (second=i;second!=end && *second!=';' && !iswhitespace(*second);second++);
	res.push_back(std::wstring(i,second));
	if (second==end || *second==';')
		return res;
	for (i=second;i!=end && iswhitespace(*i);i++);
	if (i==end || string.find(L"step",std::distance(string.begin(),i)))
		return res;
	for (i+=4;i!=end && iswhitespace(*i);i++);
	for (second=i;second!=end && *second!=';' && !iswhitespace(*second);second++);
	res.push_back(std::wstring(i,second));
	return res;
}

std::vector<std::wstring> parseLiteral_print(const std::wstring &string){
	std::vector<std::wstring> res;
	wchar_t delim=',',
		tempDelim=delim;
	for (std::wstring::const_iterator i=string.begin(),end=string.end();i!=end;i++){
		std::wstring::const_iterator second;
		for (;iswhitespace(*i);i++);
		if (*i==';')
			return res;
		if (*i=='\"' || *i=='`')
			delim=*i;
		if (delim==tempDelim)
			for (second=i+1;second!=end && *second!=delim && *second!=';';second++);
		else{
			for (second=i+1;second!=end;second++){
				if (*second=='\\'){
					second++;
					if (multicomparison(*second,"\\`\"nrx"))
						continue;
					second--;
				}
				if (*second==delim)
					break;
			}
			second++;
		}
		for (;iswhitespace(*second);second--);
		bool finishnow=0;
		if (*second==';'){
			second--;
			finishnow=1;
			for (;second!=i && *second==' ' || *second=='\t' || *second==13 || *second==10;second--);
			second++;
		}
		res.push_back(std::wstring(i,second));
		if (finishnow)
			return res;
		delim=tempDelim;
		for (i=second;i!=end && *i==delim;i++);
	}
	return res;
}

std::vector<std::wstring> parseCommandParameters(const std::wstring &string,wchar_t delim){
	std::vector<std::wstring> res;
	wchar_t tempDelim=delim;
	for (std::wstring::const_iterator i=string.begin(),end=string.end();i!=end;){
		std::wstring::const_iterator end2;
		for (;i!=end && iswhitespace(*i);i++);
		if (i==end || *i==';')
			return res;
		if (*i=='\"' || *i=='`')
			delim=*i;
		if (delim==tempDelim){
			/*
			If delim==tempDelim, it means that delim isn't any of the quote
			symbols. Therefore, if a semicolon was to be found here, it would
			mean it's not part of a string constant, and parsing doesn't need to
			continue.
			*/
			for (end2=i+1;end2!=end && *end2!=delim;end2++);
		}else{
			for (end2=i+1;end2!=end && *end2!=delim;end2++);
			end2++;
		}
		std::wstring::const_iterator comma=end2;
		for (;end2!=end && iswhitespace(end2[-1]);end--);
		res.push_back(std::wstring(i,end2));
		delim=tempDelim;
		for (i=comma;i!=end && *i==delim;i++);
	}
	return res;
}

NONS_ParsedLine::NONS_ParsedLine(const std::wstring &buffer,ulong &offset,ulong number){
	this->lineNo=number;
	this->error=NONS_NO_ERROR;
	ulong increment=0;
	this->commandName=getLine(buffer,offset,&increment);
	if (iswhitespace((char)this->commandName[0])){
		long start=0;
		for (;iswhitespace((char)this->commandName[start]) && this->commandName[start];start++);
		this->commandName=this->commandName.substr(start);
	}
	if (multicomparison(this->commandName[0],";*~`?%$!\\@#") || this->commandName[0]>0x7F){
		switch (this->commandName[0]){
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
	bool isif=!this->commandName.find(L"if") || !this->commandName.find(L"notif");
	if (this->type!=PARSEDLINE_COMMAND || isif){
		offset+=increment;
		if (isif){
			while (this->commandName[this->commandName.size()-1]==':'){
				this->commandName.append(getLine(buffer,offset,&increment));
				offset+=increment;
			}
		}
		for (;buffer[offset] && buffer[offset]!=13 && buffer[offset]!=10;offset++);
		for (;iswhitespace(buffer[offset]);offset++);
		if (this->type!=PARSEDLINE_COMMAND)
			return;
	}else{
		if (!this->commandName.find(L"literal_print") || !this->commandName.find(L"centerv")){
			for (std::wstring::iterator i=this->commandName.begin(),end=this->commandName.end();i!=end;i++){
				if (*i=='\"' || *i=='`'){
					for (wchar_t quote=*i++;i!=end;i++){
						if (*i=='\\'){
							if (multicomparison(*++i,"\\`\"nrx"))
								continue;
							i--;
						}
						if (*i==quote)
							break;
					}
					if (i==end){
						this->type=PARSEDLINE_INVALID;
						this->error=NONS_UNMATCHED_QUOTES;
						return;
					}
					continue;
				}
				if (multicomparison(*i,":\\")){
					this->stringParameters=std::wstring(i,end);
					this->commandName=std::wstring(this->commandName.begin(),i);
					trim_string(this->commandName);
					break;
				}
			}
			offset+=increment;
			if (buffer[offset]==':')
				offset++;
			else if (buffer[offset]=='\\');
			else{
				for (;buffer[offset] && buffer[offset]!=13 && buffer[offset]!=10;offset++);
				for (;iswhitespace(buffer[offset]);offset++);
			}
		}else{
			for (std::wstring::iterator i=this->commandName.begin(),end=this->commandName.end();i!=end;i++){
				if (*i=='\"' || *i=='`'){
					for (wchar_t quote=*i++;i!=end && *i!=quote;i++);
					if (i==end){
						this->type=PARSEDLINE_INVALID;
						this->error=NONS_UNMATCHED_QUOTES;
						return;
					}
					continue;
				}
				if (multicomparison(*i,":\\")){
					this->stringParameters=std::wstring(i,end);
					this->commandName=std::wstring(this->commandName.begin(),i);
					trim_string(this->commandName);
					break;
				}
			}
			offset+=increment;
			if (buffer[offset]==':')
				offset++;
			else if (buffer[offset]=='\\');
			else{
				for (;buffer[offset] && buffer[offset]!=13 && buffer[offset]!=10;offset++);
				for (;iswhitespace(buffer[offset]);offset++);
			}
		}
	}
	if (!this->commandName.size()){
		this->type=PARSEDLINE_EMPTY;
		return;
	}
	ulong a,b;
	for (a=0;a<this->commandName.size() && !iswhitespace(this->commandName[a]);a++);
	for (b=a;b<this->commandName.size() && iswhitespace(this->commandName[b]);b++);
	this->stringParameters=this->commandName.substr(b);
	this->commandName=this->commandName.substr(0,a);
	if (this->commandName==L"if" || this->commandName==L"notif")
		preparseIf(this->stringParameters,this->parameters);
	else if (this->commandName==L"for")
		this->parameters=parseFor(this->stringParameters);
	else if (this->commandName==L"literal_print")
		this->parameters=parseLiteral_print(this->stringParameters);
	else
		this->parameters=parseCommandParameters(this->stringParameters,',');
}

NONS_ParsedLine::NONS_ParsedLine(const std::wstring &string,ulong number){
	this->lineNo=number;
	std::wstring::const_iterator first=string.begin(),
		second,
		end=string.end();
	for (;first!=end && iswhitespace(*first);first++);
	if (*first==';'){
		this->type=PARSEDLINE_COMMENT;
		return;
	}
	for (second=first;second!=end && !iswhitespace(*second);second++);
	this->type=PARSEDLINE_COMMAND;
	this->commandName=std::wstring(first,second);
	this->stringParameters=std::wstring(second,end);
	trim_string(this->stringParameters);
	this->parameters=parseCommandParameters(this->stringParameters,',');
}

ulong NONS_ParsedLine::nextLine(const std::wstring &buffer,ulong offset){
	ulong increment=0;
	getLine(buffer,offset,&increment);
	ulong res=offset+increment;
	for (;buffer[res]!=13 && buffer[res]!=10;res++);
	for (;buffer[res]==13 || buffer[res]==10;res++);
	return res;
}

ulong NONS_ParsedLine::nextStatement(const std::wstring &buffer,ulong offset){
	ulong previous=offset;
	while (1){
		NONS_ParsedLine line(buffer,offset,0);
		if (line.type==PARSEDLINE_COMMAND)
			return previous;
		previous=offset;
	}
}

ulong NONS_ParsedLine::previousLine(const std::wstring &buffer,ulong offset){
	ulong res=offset;
	for (;res && buffer[res]!=13 && buffer[res]!=10;res--);
	for (;res && buffer[res]==13 || buffer[res]==10;res--);
	for (;res && buffer[res]!=13 && buffer[res]!=10;res--);
	if (res)
		res++;
	return res;
}
#endif
