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

#include "MacroParser.h"
#include "../../UTF.h"
#include "../../Globals.h"
#include "../FileIO.h"
#include <string>
#include <vector>
#include <sstream>
#include <deque>

//0: no changes; !0: something changed
bool preprocess(std::wstring &dst,const std::wstring &script,NONS_Macro::macroFile &macros,bool output);

bool preprocess(std::wstring &dst,const std::wstring &script){
	ulong l;
	char *temp;
	if (temp=(char *)readfile(L"macros.txt",l)){
		std::wstringstream stream(UniFromUTF8(std::string(temp,l)));
		delete[] temp;
		NONS_Macro::macroFile *macroFile;
		if (!macroParser_yyparse(stream,macroFile) && macroFile->checkSymbols() && preprocess(dst,script,*macroFile,1)){
			if (CLOptions.outputPreprocessedFile){
				std::string str2=UniToUTF8(dst);
				writefile(CLOptions.preprocessedFile,&str2[0],str2.size());
			}
		}else{
			if (CLOptions.outputPreprocessedFile){
				std::string str2=UniToUTF8(script);
				writefile(CLOptions.preprocessedFile,&str2[0],str2.size());
			}
			return 0;
		}
	}else{
		if (CLOptions.outputPreprocessedFile){
			std::string str2=UniToUTF8(script);
			writefile(CLOptions.preprocessedFile,&str2[0],str2.size());
		}
		return 0;
	}
	return 1;
}

bool readIdentifier(std::wstring &dst,const std::wstring &src,ulong offset){
	if (!NONS_isid1char(src[offset]))
		return 1;
	ulong len=1;
	while (NONS_isidnchar(src[offset+len]))
		len++;
	if (offset+len>=src.size())
		return 0;
	dst=src.substr(offset,len);
	return 1;
}

ulong countLinesUntilOffset(const std::wstring &str,ulong offset){
	ulong res=1;
	for (ulong a=0;a<offset && a<str.size();a++){
		if (str[a]==10)
			res++;
		else if (str[a]==13){
			res++;
			if (a+1<str.size() && str[a+1]==10)
				a++;
		}
	}
	return res;
}

template <typename T>
bool partialCompare(const std::basic_string<T> &A,size_t offset,const std::basic_string<T> &B){
	if (A.size()-offset<B.size())
		return 0;
	for (size_t a=offset,b=0;b<B.size();a++,b++)
		if (A[a]!=B[b])
			return 0;
	return 1;
}

bool preprocess(std::wstring &dst,const std::wstring &script,NONS_Macro::macroFile &macros,bool output){
	ulong first=script.find(L";#call");
	if (first==script.npos)
		return 0;
	std::deque<ulong> calls;
	while (first!=script.npos && first+7<script.size()){
		wchar_t c=script[first+6];
		if (iswhitespace(c))
			calls.push_back(first+6);
		first=script.find(L";#call",first+6);
	}
	if (!calls.size())
		return 0;
	ulong callsCompleted=0;
	dst.append(script.substr(0,calls.front()-6));
	static const std::wstring open_block=L"block{",
		close_block=L"}block";
	while (calls.size()){
		first=script.find_first_not_of(WCS_WHITESPACE,calls.front());
		ulong lineNo=countLinesUntilOffset(script,calls.front());
		std::wstring identifier;
		if (!readIdentifier(identifier,script,first)){
			o_stderr <<"Warning: Line "<<lineNo<<": Possible macro call truncated by the end of file: \'"<<identifier<<"\'.\n";
			return !!callsCompleted;
		}
		first+=identifier.size();
		first=script.find_first_not_of(WCS_WHITESPACE,first);
		if (!macros.symbol_table.get(identifier)){
			o_stderr <<"Warning: Line "<<lineNo
				<<": Possible macro call doesn't call a defined macro: \'"<<identifier<<"\'.\n";
			while (calls.size() && calls.front()<=first)
				calls.pop_front();
			goto preprocess_000;
		}
		if (script[first]!='('){
			o_stderr <<"Warning: Line "<<(ulong)lineNo<<": Possible macro call invalid. Expected \'(\' but found \'"<<script[first]<<"\'.\n";
			goto preprocess_000;
		}
		{
			std::vector<std::wstring> arguments;
			bool found_lparen=0;
			first++;
			while (!found_lparen){
				std::wstring argument;
				if (partialCompare(script,first,open_block)){
					do{
						first+=6;
						ulong len=script.find(close_block,first);
						if (len==script.npos){
							o_stderr <<"Warning: Line "<<lineNo<<": Possible macro call invalid. Expected \"}block\" but found end of file.\n";
							goto preprocess_000;
						}
						len-=first;
						argument.append(script.substr(first,len));
						first+=len+6;
						first=script.find_first_not_of(WCS_WHITESPACE,first);
					}while (first!=script.npos && partialCompare(script,first,open_block));
					if (first==script.npos){
						o_stderr <<"Warning: Line "<<lineNo<<": Possible macro call invalid. Expected \',\' or \')\' but found end of file.\n";
						goto preprocess_000;
					}
					if (script[first]!=',' && script[first]!=')'){
						o_stderr <<"Warning: Line "<<lineNo<<": Possible macro call invalid. Expected \',\' or \')\' but found \'"<<script[first]<<"\'.\n";
						goto preprocess_000;
					}
					if (script[first++]==')')
						found_lparen=1;
				}else{
					ulong second=first;
					bool found_comma=0;
					for (;second<script.size() && !found_comma;second++){
						switch (script[second]){
							case '\\':
								if (second+1>=script.size()){
									o_stderr <<"Warning: Line "<<lineNo<<": Possible macro call invalid. Incomplete escape sequence.\n";
									goto preprocess_000;
								}
								argument.push_back(script[++second]);
								break;
							case ')':
								found_lparen=1;
							case ',':
								found_comma=1;
								break;
							default:
								argument.push_back(script[second]);
						}
					}
					first=second;
				}
				arguments.push_back(argument);
			}
			std::wstring macroResult=macros.call(identifier,arguments);
			while (1){
				std::wstring temp;
				if (!preprocess(temp,macroResult,macros,0))
					break;
				macroResult=temp;
			}
			callsCompleted++;
			if (output && CLOptions.outputPreprocessedFile)
				o_stderr <<"Call to macro \""<<identifier<<"\" on line "<<lineNo<<" was written to the result on line "<<countLinesUntilOffset(dst,dst.size())<<".\n";
			dst.append(macroResult);
		}
preprocess_000:
		while (calls.size() && calls.front()<first)
			calls.pop_front();
		if (calls.size())
			dst.append(script.substr(first,calls.front()-6-first));
		else
			dst.append(script.substr(first));
	}
	return !!callsCompleted;
}
