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

#ifndef NONS_VARIABLESTORE_H
#define NONS_VARIABLESTORE_H

#include "../Common.h"
#include "../ErrorCodes.h"
#include "../strCmpT.h"
#include "Variable.h"
#include "ExpressionParser.h"
#include <vector>
#include <map>
#include <set>

extern std::set<wchar_t *,wstrCmp> labelsUsed;

struct NONS_VariableStore{
	std::map<wchar_t *,NONS_Variable *,wstrCmpCI> variables;
	std::map<ulong,NONS_Variable *> stack;
	std::map<wchar_t *,NONS_Variable *,wstrCmpCI> arrayVariables;
	bool commitGlobals;
	NONS_VariableStore();
	~NONS_VariableStore();
	void saveData();
	ErrorCode evaluate(wchar_t *exp,long *result,bool invert_terms=0);
	ErrorCode resolveIndexing(wchar_t *expression,NONS_Variable **res);
	void push(ulong pos,NONS_Variable *var);
	bool link(wchar_t *name,ulong pos);
	void push(wchar_t *name,NONS_Variable *var);
	NONS_Variable *retrieve(wchar_t *name);
	NONS_Variable *retrieve(ulong position);
	NONS_Variable *retrieve(wchar_t *name,std::vector<ulong> *index);
	NONS_Variable *retrieve(wchar_t *name,ulong *index,ulong size);
	ErrorCode getIntValue(wchar_t *str,long *value);
	ErrorCode getStrValue(wchar_t *str,char **value);
	ErrorCode getWcsValue(wchar_t *str,wchar_t **value);
	ErrorCode getStrValue(char *str,char **value);
};
#endif
