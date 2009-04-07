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

#define VARIABLE_HAS_NO_DATA(x) (!(x) || !(x)->intValue->getInt() && (!(x)->wcsValue->getWcs() || !*(x)->wcsValue->getWcs()))

extern std::set<wchar_t *,wstrCmp> labelsUsed;
typedef std::map<wchar_t *,NONS_VariableMember *,wstrCmpCI> constants_map_T;
typedef std::map<Sint32,NONS_Variable *> variables_map_T;
typedef std::map<wchar_t *,NONS_VariableMember *,wstrCmpCI> arrays_map_T;

struct NONS_VariableStore{
	constants_map_T constants;
	variables_map_T variables;
	arrays_map_T arrays;
	bool commitGlobals;
	NONS_VariableStore();
	~NONS_VariableStore();
	void saveData();
	ErrorCode evaluate(const wchar_t *exp,long *result,bool invert_terms=0);
	ErrorCode resolveIndexing(const wchar_t *expression,NONS_VariableMember **res);
	void push(Sint32 pos,NONS_Variable *var);
	bool link(wchar_t *name,Sint32 pos);
	void push(wchar_t *name,NONS_Variable *var);
	NONS_VariableMember *retrieve(const wchar_t *name,ErrorCode *error);
	NONS_Variable *retrieve(Sint32 position,ErrorCode *error);
	NONS_VariableMember *retrieve(const wchar_t *name,const std::vector<ulong> *index,ErrorCode *error);
	NONS_VariableMember *retrieve(const wchar_t *name,ulong *index,ulong size,ErrorCode *error);
	ErrorCode getIntValue(wchar_t *str,long *value);
	ErrorCode getStrValue(wchar_t *str,char **value);
	//ErrorCode getWcsValue(wchar_t *str,wchar_t **value);
	ErrorCode getStrValue(char *str,char **value);
};
#endif
