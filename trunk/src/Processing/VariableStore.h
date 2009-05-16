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
#include "../IO_System/SAR/Image_Loader/FileLog.h"
#include "Variable.h"
#include <vector>
#include <map>
#include <set>

#define VARIABLE_HAS_NO_DATA(x) (!(x) || !(x)->intValue->getInt() && (!(x)->wcsValue->getWcs() || !*(x)->wcsValue->getWcs()))

extern NONS_LabelLog labellog;

typedef std::map<wchar_t *,NONS_VariableMember *,wstrCmpCI> constants_map_T;
typedef std::map<Sint32,NONS_Variable *> variables_map_T;
typedef std::map<Sint32,NONS_VariableMember *> arrays_map_T;

struct NONS_VariableStore{
	constants_map_T constants;
	variables_map_T variables;
	arrays_map_T arrays;
	bool commitGlobals;
	NONS_VariableStore();
	~NONS_VariableStore();
	//Destroys all variables and arrays. Use with care!
	void reset();
	void saveData();
	/*
	exp: (IN) The expression to be evaluated.
	result: (OUT) Where the result of evaluation will go to. Pass 0 if the
		result is not needed.
	invert_terms: (IN) Pass 1 if the expression comes from command_notif and
		we're using the old style evaluator. This parameter is designed to be
		used with simple expressions (e.g. * [&& * [&& * [&& ...]]], * being
		comparisons). Using it with more complex expressions will have weirder
		and weirder results as the expression gets more complex.
	array_decl: (OUT) Use to have the evaluator figure out the number and sizes
		of the dimensions a declared array should have. In order for it to both
		work and be simple, the evaluator makes the assumption that one and only
		one array in the expression doesn't exist. The contents of the resulting
		vector are: Element 0: How many arrays that didn't exist were referenced
		in the expression. Any value other than 1 makes elements 1 and above
		invalid and the caller should throw an error. Element 1: The array
		identifier. Element 2+n: The size of dimension n.
		If the resulting vector is of size zero, either no valid array syntaxes
		were found, or all the valid ones that were found referred to array that
		already existed.
	retrievedVar: (OUT) Use to have the evaluator figure out what variable
		member is the expression referencing. The method is stronger than
		array_decl's, and it will figure out the variable even if it's not the
		only variable referenced in the expression. The only limitation is that
		I can't predict which of the two variables will be the result if
		there's a binary operator at the top level of the expression and both
		operands are variables (e.g. %0+%1). This restriction does not apply to
		the [] operator.
	string: (OUT) Use to get the string the expression evaluates to, if any. It
		works with both literals and string variables.
	*/
	ErrorCode evaluate(
		const wchar_t *exp,
		long *result,
		bool invert_terms,
		std::vector<long> *array_decl,
		NONS_VariableMember **retrievedVar,
		wchar_t **string);
	//ErrorCode resolveIndexing(const wchar_t *expression,NONS_VariableMember **res);
	void push(Sint32 pos,NONS_Variable *var);
	bool link(wchar_t *name,Sint32 pos);
	void push(wchar_t *name,NONS_Variable *var);
	NONS_VariableMember *retrieve(const wchar_t *name,ErrorCode *error);
	NONS_Variable *retrieve(Sint32 position,ErrorCode *error);
	//NONS_VariableMember *retrieve(const wchar_t *name,const std::vector<ulong> *index,ErrorCode *error);
	//NONS_VariableMember *retrieve(const wchar_t *name,ulong *index,ulong size,ErrorCode *error);
	NONS_VariableMember *getConstant(const wchar_t *name);
	NONS_VariableMember *getArray(Sint32 arrayNo);

	ErrorCode getWcsValue(const wchar_t *str,wchar_t **value);
	ErrorCode getStrValue(const wchar_t *str,char **value);
	ErrorCode getStrValue(const char *str,char **value);
	ErrorCode getIntValue(const wchar_t *str,long *value);
};
#endif
