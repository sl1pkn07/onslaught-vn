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

#ifndef NONS_SCRIPT_H
#define NONS_SCRIPT_H

#include "../../Common.h"
#include "../../UTF.h"
#include "../../ErrorCodes.h"
#include "../../enums.h"
#include "../SAR/GeneralArchive.h"
#include <vector>
#include <map>

struct NONS_ParsedStatement{
	std::wstring commandName;
	std::wstring stringParameters;
	std::vector<std::wstring> parameters;
	enum StatementType{
		PARSEDSTMT_EMPTY,
		PARSEDSTMT_COMMENT,
		PARSEDSTMT_BLOCK,
		PARSEDSTMT_JUMP,
		PARSEDSTMT_PRINTER,
		PARSEDSTMT_COMMAND,
		PARSEDSTMT_INVALID,
	} type;
	ErrorCode error;
	NONS_ParsedStatement(const std::wstring &string);
private:
	void preparseIf();
	void preparseFor();
};

struct NONS_ScriptLine;

struct NONS_Statement{
	std::wstring stmt;
	NONS_ScriptLine *lineOfOrigin;
	ulong fileOffset;
	ulong statementNo;
	NONS_ParsedStatement *parsed_stmt;
	NONS_Statement(const std::wstring &string,NONS_ScriptLine *line,ulong number,ulong offset,bool terminal=0);
	NONS_ParsedStatement *parse();
};

struct NONS_ScriptLine{
	ulong lineNumber;
	std::vector<NONS_Statement *> statements;
	NONS_ScriptLine(ulong line,const std::wstring &string,ulong off);
	~NONS_ScriptLine();
};

struct NONS_ScriptBlock{
	std::wstring name;
	NONS_Statement *labelStatement;
	bool used;
	NONS_ScriptBlock(NONS_Statement *stmt,bool *valid=0);
};

inline bool sortBlocksByName(const NONS_ScriptBlock *a,const NONS_ScriptBlock *b){
	return stdStrCmpCI(a->name,b->name)<0;
}

inline int findBlocksByName(const std::wstring &a,NONS_ScriptBlock * const &b){
	return stdStrCmpCI(a,b->name);
}

inline int findBlocksByOffset(const ulong &a,NONS_ScriptBlock * const &b){
	ulong b0=b->labelStatement->fileOffset;
	if (a==b0)
		return 0;
	if (a<b0)
		return -1;
	return 1;
}

struct NONS_Script{
	std::vector<NONS_ScriptLine *> script;
	std::vector<NONS_ScriptBlock *> blocksByLine;
	std::vector<NONS_ScriptBlock *> blocksByName;
	std::vector<NONS_Statement *> jumps;
	unsigned hash[5];
	NONS_Script();
	~NONS_Script();
	ErrorCode init(const char *scriptname,NONS_GeneralArchive *archive,ulong encoding,ulong encryption);
	NONS_Statement *statementFromLabel(std::wstring name);
	std::wstring statementFromOffset(ulong offset);
};
#endif
