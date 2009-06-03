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

#ifndef NONS_SCRIPT_CPP
#define NONS_SCRIPT_CPP

#include "Script.h"
#include "../FileIO.h"
#include "../IOFunctions.h"
#include "../../UTF.h"
#include "../../Globals.h"
#include "../SaveFile.h"
#include "sha1.h"
#include <cstring>

NONS_ParsedStatement::NONS_ParsedStatement(const std::wstring &string){
	if (!string.size()){
		this->type=PARSEDSTMT_EMPTY;
		return;
	}
	ulong size=string.size();
	if (multicomparison(string[0],";*`\\@!#~%$?") || string[0]>0x7F){
		switch (this->commandName[0]){
			case ';':
				this->type=PARSEDSTMT_COMMENT;
				return;
			case '*':
				this->type=PARSEDSTMT_BLOCK;
				this->commandName=string;
				break;
			case '~':
				this->type=PARSEDSTMT_JUMP;
				return;
			default:
				this->type=PARSEDSTMT_PRINTER;
				this->commandName=string;
		}
	}else{
		this->type=PARSEDSTMT_COMMAND;
		ulong space=0;
		for (;space<size && !iswhitespace(string[space]);space++);
		this->commandName=string.substr(0,space);
		if (!isValidIdentifier(this->commandName)){
			this->commandName.clear();
			this->error=NONS_INVALID_COMMAND_NAME;
			return;
		}
		for (;space<size && iswhitespace(string[space]);space++);
		this->stringParameters=string.substr(space);
		if (!stdStrCmpCI(this->commandName,L"if") || !stdStrCmpCI(this->commandName,L"notif")){

		}else if (!stdStrCmpCI(this->commandName,L"for")){
		}else{
			ulong start=space,
				end;
			while (start<size){
				end=start;
				while (end<size && string[end]!=','){
					if (string[end]=='\"' || string[end]=='`'){
						wchar_t quote=string[end];
						for (end++;end<size && string[end]!=quote;end++);
					}else if (end+1<size && NONS_tolower(string[end])=='e' && string[end+1]){
						for (end+=2;end<size && string[end]!='\"';end++){
							if (string[end]=='\\' && string[end+1]=='x'){
								end+=2;
								for (ulong a=0;end<size && string[end]!='\"' && a<4;a++,end++);
							}
						}
					}
					for (end++;end<size && iswhitespace(string[end]);end++);
				}
				if (end>size)
					end=size;
				ulong start0=end;
				for (end--;end>start && iswhitespace(string[end]);end--);
				this->parameters.push_back(std::wstring(string,start,end-start));
				start=start0;
				for (;start<size && !iswhitespace(string[start]);start++);
			}
		}
	}
}

void NONS_ParsedStatement::preparseIf(){
}

void NONS_ParsedStatement::preparseFor(){
}

NONS_Statement::NONS_Statement(const std::wstring &string,NONS_ScriptLine *line,ulong number,ulong offset,bool terminal){
	this->stmt=string;
	this->lineOfOrigin=line;
	this->fileOffset=offset;
	this->statementNo=number;
	this->parsed_stmt=0;
	if (terminal)
		this->parse();
}

NONS_ParsedStatement *NONS_Statement::parse(){
	if (!this->parsed_stmt)
		this->parsed_stmt=new NONS_ParsedStatement(this->stmt);
	return this->parsed_stmt;
}

NONS_ScriptLine::NONS_ScriptLine(ulong line,const std::wstring &string,ulong off){
	this->lineNumber=line;
	ulong start=string.find_first_not_of(L"\x09\x20");
	off+=start;
	std::wstring temp=string.substr(start);
	wchar_t *C_temp=&temp[0];
	std::wstring temp2;
	for (ulong a=0,size=string.size();a<size;){
		bool terminal=0;
		if (multicomparison(temp[0],";~`?%$!\\@#") || temp[0]>0x7F){
			temp2=std::wstring(temp,a);
			terminal=1;
			a=size;
		}else{
			ulong b=a;
			while (b<size){
				if (C_temp[b]=='\"' || C_temp[b]=='`'){
					wchar_t quote=C_temp[b];
					for (b++;b<size && C_temp[b]!=quote;b++);
				}else if (C_temp[b]==':')
					break;
				else if (b+1<size && NONS_tolower(C_temp[b])=='e' && C_temp[b+1]){
					for (b+=2;b<size && C_temp[b]!='\"';b++){
						if (C_temp[b]=='\\' && C_temp[b+1]=='x'){
							b+=2;
							for (ulong c=0;b<size && C_temp[b]!='\"' && c<4;c++,b++);
						}
					}
				}
				b++;
			}
			if (b>=size)
				b=size;
			ulong c=b-1;
			for (;C_temp[c]==9 || C_temp[c]==32;c--);
			c++;
			temp2=std::wstring(temp,a,c-a);
			a=b;
			if (C_temp[a]==':')
				for (a++;a<size && (C_temp[a]==9 || C_temp[a]==32);a--);
		}
		this->statements.push_back(new NONS_Statement(temp2,this,this->statements.size(),off+a,terminal));
	}
}

NONS_ScriptLine::~NONS_ScriptLine(){
	for (ulong a=0;a<this->statements.size();a++)
		delete this->statements[a];
}

NONS_ScriptBlock::NONS_ScriptBlock(NONS_Statement *stmt,bool *valid){
	if (stmt->stmt[0]=='*')
		this->name=stmt->stmt.substr(1);
	else
		this->name=stmt->stmt;
	trim_string(this->name);
	if (!!valid)
		*valid=isValidIdentifier(this->name);
	this->used=0;
}

NONS_Script::NONS_Script(){
	memset(this->hash,0,sizeof(unsigned)*5);
}

ErrorCode NONS_Script::init(const char *scriptname,NONS_GeneralArchive *archive,ulong encoding,ulong encryption){
	ulong l;
	char *temp=(char *)archive->getFileBuffer(UniFromISO88591(scriptname),l);
	if (!temp)
		return NONS_FILE_NOT_FOUND;
	{
		int error_code=inPlaceDecryption(temp,l,encryption);
		if (error_code!=NONS_NO_ERROR){
			delete[] temp;
			return error_code;
		}
	}
	std::wstring wtemp;
	switch (encoding){
		case DETECT_ENCODING:
			if (isValidUTF8(temp,l)){
				o_stderr <<"The script seems to be a valid UTF-8 stream. Using it as such.\n";
				wtemp=UniFromUTF8(std::string(temp,l));
			}else if (isValidSJIS(temp,l)){
				o_stderr <<"The script seems to be a valid Shift JIS stream. Using it as such.\n";
				wtemp=UniFromSJIS(std::string(temp,l));
			}else if (!ISO88591_or_UCS2(temp,l)){
				o_stderr <<"The script seems to be a valid ISO-8859-1 stream. Using it as such.\n";
				wtemp=UniFromISO88591(std::string(temp,l));
			}else{
				o_stderr <<"The script seems to be a valid UCS-2 stream. Using it as such.\n";
				wtemp=UniFromUCS2(std::string(temp,l),UNDEFINED_ENDIANNESS);
			}
			break;
		case ISO_8859_1_ENCODING:
			wtemp=UniFromISO88591(std::string(temp,l));
			break;
		case UCS2_ENCODING:
			if (l%2){
				o_stderr <<"WARNING: input text has odd length. It may not be valid UCS-2 text.\n";
				l--;
			}
			wtemp=UniFromUCS2(std::string(temp,l),UNDEFINED_ENDIANNESS);
			break;
		case UTF8_ENCODING:
			wtemp=UniFromUTF8(std::string(temp,l));
			break;
		case SJIS_ENCODING:
			wtemp=UniFromSJIS(std::string(temp,l));
			break;
		default:
			break;
	}
	wchar_t *buffer=&wtemp[0];
	for (ulong a=0;a<wtemp.size();a++){
		ulong b=wtemp.find_first_of(L"\x0A\x0D",a);
		std::wstring temp(wtemp,a,b-a);
		temp=std::wstring(temp,temp.find_first_not_of(L"\x09\x20"));
		ulong off=a;
		while (temp[temp.size()-1]=='/' && b!=wtemp.npos){
			this->script.push_back(new NONS_ScriptLine(this->script.size()+1,L"",off));
			a=b;
			if (buffer[a]==10)
				a++;
			else if (a+1<wtemp.size() && buffer[a+1]==10)
				a+=2;
			else
				a++;
			b=wtemp.find_first_of(L"\x0A\x0D",a);
			temp=temp.substr(0,temp.size()-1)+std::wstring(wtemp,a,b-a);
		}
		this->script.push_back(new NONS_ScriptLine(this->script.size()+1,temp,off));
		if (b==wtemp.npos)
			break;
		a=b;
		if (buffer[a]==10)
			a++;
		else if (a+1<wtemp.size() && buffer[a+1]==10)
			a+=2;
		else
			a++;
	}
	wtemp.clear();
	std::set<std::wstring,stdStringCmpCI<wchar_t> > *checkDuplicates=new std::set<std::wstring,stdStringCmpCI<wchar_t> >;
	ErrorCode error=NONS_NO_ERROR;
	for (ulong a=0;a<this->script.size();a++){
		NONS_ScriptLine *line=this->script[a];
		for (ulong b=0;b<line->statements.size();b++){
			if (!line->statements[b]->stmt.size())
				continue;
			if (line->statements[b]->stmt[0]=='*'){
				bool valid;
				NONS_ScriptBlock *block=new NONS_ScriptBlock(line->statements[b],&valid);
				if (!valid){
					delete block;
					handleErrors(NONS_INVALID_ID_NAME,a+1,"NONS_Script::init",1,L"The label will be ignored");
					continue;
				}
				if (checkDuplicates->find(block->name)!=checkDuplicates->end()){
					delete block;
					handleErrors(NONS_DUPLICATE_LABEL,a+1,"NONS_Script::init",1);
					error=NONS_FATAL_ERROR;
				}
				this->blocksByLine.push_back(block);
				this->blocksByName.push_back(block);
			}else if (line->statements[b]->stmt[0]=='~')
				this->jumps.push_back(line->statements[b]);
		}
	}
	if (!CHECK_FLAG(error,NONS_NO_ERROR_FLAG)){
		delete checkDuplicates;
		return error;
	}
	std::sort(this->blocksByName.begin(),this->blocksByName.end(),sortBlocksByName);
	if (!this->statementFromLabel(L"*define"))
		return NONS_NO_DEFINE_LABEL;
	SHA1 hash;
	for (ulong a=0;a<this->blocksByLine.size();a++){
		std::wstring &b=this->blocksByLine[a]->name;
		wchar_t *temp=&b[0];
		hash.Input((char *)temp,b.size()*sizeof(wchar_t));
	}
	hash.Result(this->hash);
	save_directory=getSaveLocation(this->hash);
	return NONS_NO_ERROR;
}

NONS_Script::~NONS_Script(){
	for (ulong a=0;a<this->script.size();a++)
		delete this->script[a];
	for (ulong a=0;a<this->blocksByLine.size();a++)
		delete this->blocksByLine[a];
}

NONS_Statement *NONS_Script::statementFromLabel(std::wstring name){
	std::wstring copy;
	if (name[0]=='*')
		copy=name.substr(1);
	else
		copy=name;
	trim_string(copy);
	size_t off;
	if (!binary_search<NONS_ScriptBlock *,std::wstring>(&this->blocksByName[0],0,this->blocksByName.size(),name,off,&findBlocksByName))
		return 0;
	return this->blocksByName[off]->labelStatement;
}

std::wstring NONS_Script::statementFromOffset(ulong offset){
	size_t off;
	if (!binary_search<NONS_ScriptBlock *,ulong>(&this->blocksByLine[0],0,this->blocksByLine.size(),offset,off,&findBlocksByOffset))
		return L"";
	return this->blocksByName[off]->name;
}
#endif
