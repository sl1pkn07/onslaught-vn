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
#include "../IO_System/SAR/FileLog.h"
#include "ExpressionParser.tab.hpp"
#include <vector>
#include <map>
#include <set>

class NONS_VariableMember{
	long intValue;
	std::wstring wcsValue;
	bool constant;
	yytokentype type;
	long _long_upper_limit;
	long _long_lower_limit;
	const static std::wstring null;
public:
	NONS_VariableMember **dimension;
	ulong dimensionSize;
	bool temporary;
	bool negated;
	NONS_VariableMember(yytokentype type);
	NONS_VariableMember(long value);
	NONS_VariableMember(const std::wstring &a);
	//Assumes: All dimensions have a non-negative size.
	NONS_VariableMember(std::vector<long> &sizes,size_t startAt);
	NONS_VariableMember(const NONS_VariableMember &b);
	~NONS_VariableMember();
	void makeConstant();
	bool isConstant();
	yytokentype getType();
	long getInt();
	const std::wstring &getWcs();
	NONS_VariableMember *getIndex(ulong i);
	void set(long a);
	void atoi(const std::wstring &a);
	void set(const std::wstring &a);
	void inc();
	void dec();
	void add(long a);
	void sub(long a);
	void mul(long a);
	void div(long a);
	void mod(long a);
	void setlimits(long lower,long upper);
	void negate(bool a);
private:
	void fixint();
};

struct NONS_Variable{
	NONS_VariableMember *intValue;
	NONS_VariableMember *wcsValue;
	NONS_Variable();
	NONS_Variable(const NONS_Variable &b);
	NONS_Variable &operator=(const NONS_Variable &b);
	~NONS_Variable();
};

#define VARIABLE_HAS_NO_DATA(x) (!(x) || !(x)->intValue->getInt() && !(x)->wcsValue->getWcs().size())

extern NONS_LabelLog labellog;

typedef std::map<std::wstring,NONS_VariableMember *,stdStringCmpCI<wchar_t> > constants_map_T;
typedef std::map<Sint32,NONS_Variable *> variables_map_T;
typedef std::map<Sint32,NONS_VariableMember *> arrays_map_T;

struct NONS_VariableStore{
	static const Sint32 indexLowerLimit,
		indexUpperLimit;

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
		const std::wstring &exp,
		long *result,
		bool invert_terms,
		std::vector<long> *array_decl,
		NONS_VariableMember **retrievedVar,
		std::wstring *string);
	//ErrorCode resolveIndexing(const wchar_t *expression,NONS_VariableMember **res);
	void push(Sint32 pos,NONS_Variable *var);
	NONS_VariableMember *retrieve(const std::wstring &name,ErrorCode *error);
	NONS_Variable *retrieve(Sint32 position,ErrorCode *error);
	//NONS_VariableMember *retrieve(const wchar_t *name,const std::vector<ulong> *index,ErrorCode *error);
	//NONS_VariableMember *retrieve(const wchar_t *name,ulong *index,ulong size,ErrorCode *error);
	NONS_VariableMember *getConstant(const std::wstring &name);
	NONS_VariableMember *getArray(Sint32 arrayNo);
	Sint32 getVariableIndex(NONS_VariableMember *var);

	ErrorCode getWcsValue(const std::wstring &str,std::wstring &value);
	ErrorCode getIntValue(const std::wstring &str,long &value);
};
#endif
