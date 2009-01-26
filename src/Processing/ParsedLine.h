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
*     * Helios' name may not be used to endorse or promote products derived from
*       this software without specific prior written permission.
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

#ifndef NONS_PARSEDLINE_H
#define NONS_PARSEDLINE_H

#include "../Common.h"
#include "../ErrorCodes.h"
#include <vector>

#define STRLEN(x) sizeof(*(x))==1?strlen((char *)(x)):wcslen((wchar_t *)(x))

enum{
	PARSEDLINE_EMPTY,
	PARSEDLINE_COMMENT,
	PARSEDLINE_BLOCK,
	PARSEDLINE_JUMP,
	PARSEDLINE_PRINTER,
	PARSEDLINE_COMMAND,
	PARSEDLINE_INVALID,
};

struct NONS_ParsedLine{
	//It's not a very descriptive name (or rather, it's a misleading one) but
	//this actually holds the command C string.
	wchar_t *line;
	wchar_t *param;
	std::vector<wchar_t *> parameters;
	ulong type;
	ErrorCode error;
	long lineNo;
	NONS_ParsedLine(wchar_t *buffer,ulong *offset,ulong number=0);
	NONS_ParsedLine(wchar_t *string,ulong number=0);
	~NONS_ParsedLine();
	static ulong previousLine(wchar_t *buffer,ulong offset);
	static ulong nextLine(wchar_t *buffer,ulong offset);
	static ulong nextStatement(wchar_t *buffer,ulong offset);
	
};
#endif
