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
#include <climits>

class NONS_VariableMember{
	long intValue;
	wchar_t *wcsValue;
	bool constant;
	//'%' for integer, '$' for string, '?' for array.
	char type;
	long _long_upper_limit;
	long _long_lower_limit;
public:
	NONS_VariableMember **dimension;
	ulong dimensionSize;
	NONS_VariableMember(char type);
	NONS_VariableMember(ulong *dimensions,ulong size);
	NONS_VariableMember(const NONS_VariableMember &b);
	~NONS_VariableMember();
	void makeConstant();
	bool isConstant();
	//'%' for integer, '$' for string.
	char getType();
	long getInt();
	const wchar_t *getWcs();
	wchar_t *getWcsCopy();
	char *getStrCopy();
	void set(long a);
	void set(const wchar_t *a,bool takeOwnership);
	void inc();
	void dec();
	void add(long a);
	void sub(long a);
	void mul(long a);
	void div(long a);
	void mod(long a);
	void setlimits(long lower,long upper);
private:
	void fixint();
};
#endif
