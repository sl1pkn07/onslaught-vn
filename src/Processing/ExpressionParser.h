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

#ifndef NONS_EXPRESSIONPARSER_H
#define NONS_EXPRESSIONPARSER_H

#include "../Common.h"
#include "../ErrorCodes.h"
#include <vector>

struct operand{
	int constant;
	char *symbol;
	long position;
	char type;
	operand();
	operand(const operand &b);
	operand &operator=(const operand &b);
	~operand();
};

template <typename T>
struct simpleoperation{
	char *function;
	operand *operandA;
	operand *operandB;
	simpleoperation();
	simpleoperation(const simpleoperation &b);
	simpleoperation<T> &operator=(const simpleoperation &b);
	~simpleoperation();
	void clear();
};

ErrorCode parse_expression(char *exp,ulong *offset,std::vector<simpleoperation<char> *> *queue,operand *operandA=0);
//template <typename T> ErrorCode parse_expression(T *exp,ulong *offset,std::vector<simpleoperation<T> *> *queue,operand<T> *operandA)
#endif
