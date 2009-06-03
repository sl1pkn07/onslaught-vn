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

#ifndef NONS_VARIABLEMEMBER_H
#define NONS_VARIABLEMEMBER_H

#include "../Common.h"
#include "../ErrorCodes.h"
#include "ExpressionParser.tab.hpp"
#include <climits>
#include <vector>

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
#endif
