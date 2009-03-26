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

#ifndef NONS_EXPRESSIONPARSER_CPP
#define NONS_EXPRESSIONPARSER_CPP

#include "ExpressionParser.h"
#include "../Functions.h"
#include "../UTF.h"
#include "../Globals.h"

operand::operand(){
	this->constant=0;
	this->symbol=0;
	this->position=-2;
	this->type=0;
}

operand::~operand(){
	if (this->symbol)
		delete[] this->symbol;
}

operand::operand(const operand &b){
	this->constant=b.constant;
	this->position=b.position;
	this->symbol=copyWString(b.symbol);
	this->type=b.type;
}

operand &operand::operator=(const operand &b){
	this->constant=b.constant;
	this->position=b.position;
	if (this->symbol)
		delete[] this->symbol;
	this->symbol=copyWString(b.symbol);
	this->type=b.type;
	return *this;
}

simpleoperation::simpleoperation(){
	this->function=0;
	this->operandA=0;
	this->operandB=0;
}

simpleoperation::simpleoperation(const simpleoperation &b){
	this->function=copyWString(b.function);
	this->operandA=new operand(*b.operandA);
	if (b.operandB)
		this->operandB=new operand(*b.operandB);
	else
		this->operandB=0;
}

simpleoperation &simpleoperation::operator=(const simpleoperation &b){
	if (this->function)
		delete[] this->function;
	this->function=copyWString(b.function);
	if (this->operandA)
		delete this->operandA;
	this->operandA=new operand(*b.operandA);
	if (this->operandB)
		delete this->operandB;
	if (b.operandB)
		this->operandB=new operand(*b.operandB);
	else
		this->operandB=0;
	return *this;
}

simpleoperation::~simpleoperation(){
	if (this->function)
		delete[] this->function;
	if (this->operandA)
		delete this->operandA;
	if (this->operandB)
		delete this->operandB;
}

void simpleoperation::clear(){
	this->function=0;
	this->operandA=0;
	this->operandB=0;
}

long getNumber(const wchar_t *string,ulong *offset){
	string+=*offset;
	ulong l;
	for (l=0;string[l]>='0' && string[l]<='9' || string[l]=='.' || (string[l]=='-' && !l);l++);
	char *temp=copyString(string,l);
	long res=atoi(temp);
	delete[] temp;
	(*offset)+=l;
	return res;
}

wchar_t *getSymbol(const wchar_t *string,ulong *offset){
	string+=*offset;
	if (*string!='?'){
		long l=0;
		if (string[l]=='%')
			l++;
		for (l;string[l] && (isalnum(string[l]) || string[l]=='_');l++);
		(*offset)+=l;
		return copyWString(string,l);
	}else{
		if (!isalpha(*string) || *string!='_')
			return 0;
		long l=1;
		bool breakerfound=0;
		for (;string[l] && string[l]!='[' && !iswhitespace(string[l]) && !breakerfound;l++)
			if (!isalnum(string[l]) && string[l]!='_')
				breakerfound=1;
		if (breakerfound==1)
			return 0;
		for (;string[l] && iswhitespace(string[l]);l++);
		if (string[l]!='[')
			return 0;
		while (1){
			l++;
			ulong nesting=1;
			for (;string[l];l++){
				switch (string[l]){
					case '[':
						nesting++;
						break;
					case ']':
						nesting--;
				}
				if (!nesting)
					break;
			}
			if (nesting)
				return 0;
			l++;
			long l2=l;
			for (;string[l] && iswhitespace(string[l]);l++);
			if (string[l]!='['){
				l=l2;
				break;
			}
		}
		(*offset)+=l;
		return copyWString(string,l);
	}
}

wchar_t *getOperator(const wchar_t *string,ulong *offset){
	string+=*offset;
	wchar_t *copy1=copyWString(string,1);
	wchar_t *copy2=copyWString(string,2);
	static const wchar_t *operators[]={L"||",L"&&",L"==",L"!=",L"<>",L">=",L"<=",L"=",L">",L"<",L"+",L"-",L"*",L"/",L"|",L"&",0};
	for (ushort op=0;operators[op];op++){
		if (!wcscmp(copy1,operators[op]) || !wcscmp(copy2,operators[op])){
			delete[] copy1;
			delete[] copy2;
			(*offset)+=wcslen(operators[op]);
			return copyWString(operators[op]);
		}
	}
	delete[] copy1;
	delete[] copy2;
	return 0;
}

long getPrecedence(const wchar_t *string){
	if (!string || !*string)
		return -1;
	wchar_t *copy1=copyWString(string,1);
	wchar_t *copy2=copyWString(string,2);
	static const wchar_t *operators[][9]={
		{L"||",  L"|",   0,    0,    0,    0,   0,   0,   0},
		{L"&&",  L"&",   0,    0,    0,    0,   0,   0,   0},
		{L"==",  L"!=",  L"<>",L">=",L"<=",L"=",L">",L"<",0},
		{L"+",   L"-",   0,    0,    0,    0,   0,   0,   0},
		{L"*",   L"/",   0,    0,    0,    0,   0,   0,   0},
		{L"fchk",L"lchk",0,    0,    0,    0,   0,   0,   0},
		0
	};
	for (ushort precedence=0;operators[precedence][0];precedence++){
		for (ushort op=0;operators[precedence][op];op++){
			if (!wcscmp(copy1,operators[precedence][op]) || !wcscmp(copy2,operators[precedence][op])){
				delete[] copy1;
				delete[] copy2;
				return precedence;
			}
		}
	}
	delete[] copy1;
	delete[] copy2;
	return -1;
}

ulong isunaryoperator(const wchar_t *exp,ulong offset){
	exp+=offset;
	static const wchar_t *operators[]={
		L"fchk",
		L"lchk",
		0
	};
	ulong l=0;
	for (;exp[l] && !iswhitespace(exp[l]);l++);
	wchar_t *copy=copyWString(exp,l);
	for (ulong a=0;operators[a];a++)
		if (!wcscmp(copy,operators[a]))
			return a+1;
	return 0;
}

wchar_t *getNormalString(const wchar_t *exp,ulong *offset){
	ulong l=0;
	if (exp[*offset]=='"' || exp[*offset]=='`'){
		wchar_t quote=exp[*offset];
		(*offset)++;
		ulong off2=*offset;
		for (;exp[off2+l] && exp[off2+l]!=quote;l++);
		if (exp[*offset+l]!=quote)
			return 0;
		wchar_t *res=copyWString(exp+off2,l);
		(*offset)+=l+1;
		return res;
	}
	ulong off2=*offset;
	for (;exp[off2+l] && !iswhitespace(exp[off2+l]);l++);
	wchar_t *res=copyWString(exp+off2,l);
	(*offset)+=l;
	return res;
}

ErrorCode parse_expression(const wchar_t *exp,ulong *offset,std::vector<simpleoperation *> *queue,operand *operandA){
	if (!exp || !queue)
		return NONS_INTERNAL_INVALID_PARAMETER;
	char step=!!operandA;
	simpleoperation op;
	if (operandA)
		op.operandA=operandA;
	ulong start=!offset?0:*offset;
	ulong a;
	for (a=start;exp[a];){
		//whitespace is ignored
		for (;iswhitespace(exp[a]) && exp[a];a++);
		switch (step){
			case 0:
				{
					op.operandA=new operand;
					bool unary=0;
					if (exp[a]=='('){
						a++;
						ErrorCode temp_res=parse_expression(exp,&a,queue);
						if (temp_res)
							return temp_res;
						op.operandA->position=queue->size()-1;
						op.operandA->type=2;
					}else{
						if (exp[a]!='%' && exp[a]!='?' && !isalpha(exp[a]) && exp[a]!='_'){
							op.operandA->constant=getNumber(exp,&a);
							op.operandA->type=0;
						}else{
							if (isunaryoperator(exp,a)){
								unary=1;
								op.function=getSymbol(exp,&a);
								for (;iswhitespace(exp[a]) && exp[a];a++);
								if (!(op.operandA->symbol=getNormalString(exp,&a)))
									return NONS_UNMATCHED_QUOTES;
								op.operandA->type=1;
								op.operandB=0;
								simpleoperation *copyforpush=new simpleoperation(op);
								//*copyforpush=op;
								queue->push_back(copyforpush);
							}else if (!(op.operandA->symbol=getSymbol(exp,&a)))
								return NONS_UNMATCHED_BRAKETS;
							op.operandA->type=1;
						}
					}
					for (;iswhitespace(exp[a]) && exp[a];a++);
					if (!exp[a] || exp[a]==')'){
						if (!unary){
							simpleoperation *copyforpush=new simpleoperation(op);
							//*copyforpush=op;
							queue->push_back(copyforpush);
						}
						if (offset)
							*offset=a;
						return NONS_NO_ERROR;
					}
					step++;
					break;
				}
			case 1:
				{
					wchar_t *temp=getOperator(exp,&a);
					if (!temp)
						return NONS_UNRECOGNIZED_OPERATOR;
					op.function=temp;
					step++;
					break;
				}
			case 2:
				{
					operand *temp=new operand;
					ulong unary_op;
					if (exp[a]=='(' || (unary_op=isunaryoperator(exp,a))){
						if (!unary_op)
							a++;
						ErrorCode temp_res=parse_expression(exp,&a,queue);
						if (temp_res!=NONS_NO_ERROR)
							return temp_res;
						temp->position=queue->size()-1;
						temp->type=2;
					}else{
						if (exp[a]!='%' && exp[a]!='?' && !isalpha(exp[a]) && exp[a]!='_'){
							temp->constant=getNumber(exp,&a);
							temp->type=0;
						}else{
							if (!(temp->symbol=getSymbol(exp,&a)))
								return NONS_UNMATCHED_BRAKETS;
							temp->type=1;
						}
					}
					for (;iswhitespace(exp[a]) && exp[a];a++);
					long prec0=getPrecedence(exp+a),
						prec1=getPrecedence(op.function);
					if (prec0<0){
						if (exp[a]==')'){
							op.operandB=temp;
							simpleoperation *copyforpush=new simpleoperation;
							*copyforpush=op;
							queue->push_back(copyforpush);
							op.clear();
							if (offset)
								*offset=a+1;
							return NONS_NO_ERROR;
						}
						if (!exp[a]){
							op.operandB=temp;
							simpleoperation *copyforpush=new simpleoperation;
							*copyforpush=op;
							queue->push_back(copyforpush);
							op.clear();
							if (offset)
								*offset=a;
							return NONS_NO_ERROR;
						}
						return NONS_UNDEFINED_SYNTAX_ERROR;
					}else{
						if (prec0>prec1){
							while (1){
								ErrorCode temp_res=parse_expression(exp,&a,queue,temp);
								if (temp_res!=NONS_NO_ERROR)
									return temp_res;
								prec0=getPrecedence(exp+a);
								if (prec0<=prec1)
									break;
								temp=new operand;
								temp->position=queue->size()-1;
								temp->type=2;
							}
							op.operandB=new operand;
							op.operandB->position=queue->size()-1;
							op.operandB->type=2;
							simpleoperation *copyforpush=new simpleoperation;
							*copyforpush=op;
							queue->push_back(copyforpush);
							op.clear();
							op.operandA=new operand;
							op.operandA->position=queue->size()-1;
							op.operandA->type=2;
						}else{
							op.operandB=temp;
							simpleoperation *copyforpush=new simpleoperation(op);
							//*copyforpush=op;
							queue->push_back(copyforpush);
							op.clear();
							if (prec0<prec1 && operandA){
								if (offset)
									*offset=a;
								return NONS_NO_ERROR;
							}else{
								op.operandA=new operand;
								op.operandA->position=queue->size()-1;
								op.operandA->type=2;
							}
						}
						step=1;
					}
				}
		}
	}
	if (offset)
		*offset=a;
	return NONS_NO_ERROR;
}
#endif
