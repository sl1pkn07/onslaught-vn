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

#ifndef NONS_MACROPARSER_CPP
#define NONS_MACROPARSER_CPP
#include "../../Globals.h"
#include "../../Functions.h"
#include "MacroParser.h"

#define THROW_MACRO_ID_ERROR(line,errorMsg,id) o_stderr <<"Macro processor: Error on line "<<(line)<<": "errorMsg" '"<<id<<"'.\n"

namespace NONS_Macro{
bool identifier::checkSymbols(const symbolTable &st,bool expectedVariable){
	symbol *s=st.get(this->id);
	bool r=1;
	if (!s){
		THROW_MACRO_ID_ERROR(this->referenced_on_line,"undefined symbol",this->id);
		r=0;
	}else{
		if (expectedVariable && s->type!=symbol::INTEGER && s->type!=symbol::STRING){
			THROW_MACRO_ID_ERROR(this->referenced_on_line,"symbol is not a variable",this->id);
			r=0;
		}else if (!expectedVariable && s->type!=symbol::MACRO){
			THROW_MACRO_ID_ERROR(this->referenced_on_line,"symbol is not a macro",this->id);
			r=0;
		}
	}
	return r;
}

symbol::symbol(const std::wstring &identifier,ulong line){
	this->type=UNDEFINED;
	this->identifier=identifier;
	this->declared_on_line=line;
	this->int_initialization=0;
	this->str_initialization=0;
	this->has_been_checked=0;
}

symbol::symbol(const std::wstring &identifier,NONS_Macro::macro *m,ulong line){
	this->type=MACRO;
	this->identifier=identifier;
	this->macro=m;
	this->declared_on_line=line;
	this->int_initialization=0;
	this->str_initialization=0;
	this->has_been_checked=0;
}

symbol::symbol(const std::wstring &identifier,const std::wstring &string,ulong line){
	this->type=STRING;
	this->identifier=identifier;
	this->str_val=string;
	this->declared_on_line=line;
	this->int_initialization=0;
	this->str_initialization=0;
	this->has_been_checked=0;
}

symbol::symbol(const std::wstring &identifier,long val,ulong line){
	this->type=INTEGER;
	this->identifier=identifier;
	this->int_val=val;
	this->declared_on_line=line;
	this->int_initialization=0;
	this->str_initialization=0;
	this->has_been_checked=0;
}

symbol::symbol(const symbol &b){
	this->has_been_checked=b.has_been_checked;
	this->type=b.type;
	this->identifier=b.identifier;
	this->declared_on_line=b.declared_on_line;
	switch (this->type){
		case MACRO:
			break;
		case INTEGER:
			this->int_val=b.int_val;
			this->int_initialization=b.int_initialization?b.int_initialization->clone():0;
			break;
		case STRING:
			this->str_val=b.str_val;
			this->str_initialization=b.str_initialization?b.str_initialization->clone():0;
			break;
	}
}

symbol::~symbol(){
	switch (this->type){
		case UNDEFINED:
			break;
		case MACRO:
			if (this->macro)
				delete this->macro;
			break;
		case STRING:
			if (this->str_initialization)
				delete this->str_initialization;
			break;
		case INTEGER:
			if (this->int_initialization)
				delete this->int_initialization;
			break;
	}
}

void symbol::reset(){
	switch (this->type){
		case UNDEFINED:
		case MACRO:
			break;
		case STRING:
			this->str_val.clear();
			break;
		case INTEGER:
			this->int_val=0;
	}
}

void symbol::set(long n){
	switch (this->type){
		case UNDEFINED:
		case MACRO:
			break;
		case STRING:
			this->str_val=itoa<wchar_t>(n);
			break;
		case INTEGER:
			this->int_val=n;
	}
}

void symbol::set(const std::wstring &s){
	switch (this->type){
		case UNDEFINED:
		case MACRO:
			break;
		case STRING:
			this->str_val=s;
			break;
		case INTEGER:
			this->int_val=atoi(s);
	}
}

long symbol::getInt(){
	switch (this->type){
		case MACRO:
			return 0;
		case STRING:
			return atoi(this->str_val);
		case INTEGER:
			return this->int_val;
	}
}

std::wstring symbol::getStr(){
	switch (this->type){
		case MACRO:
			return 0;
		case STRING:
			return this->str_val;
		case INTEGER:
			return itoa<wchar_t>(this->int_val);
	}
}

void symbol::initializeTo(expression *expr){
	this->int_initialization=expr;
}

void symbol::initializeTo(string *string){
	this->str_initialization=string;
}

ulong symbol::initialize(const symbolTable &st){
	ulong error;
	switch (this->type){
		case STRING:
			if (this->str_initialization){
				std::wstring val=this->str_initialization->evaluateToStr(&st,&error);
				if (error!=MACRO_NO_ERROR)
					return error;
				this->set(val);
			}
			break;
		case INTEGER:
			if (this->int_initialization){
				long val=this->int_initialization->evaluateToInt(&st,&error);
				if (error!=MACRO_NO_ERROR)
					return error;
				this->set(val);
			}
		default:
			break;
	}
	return MACRO_NO_ERROR;
}

bool symbol::checkSymbols(const symbolTable &st){
	switch (this->type){
		case UNDEFINED:
			THROW_MACRO_ID_ERROR(this->declared_on_line,"symbol of unknown type",this->identifier);
			break;
		case MACRO:
			if (!this->has_been_checked){
				this->has_been_checked=1;
				bool ret=this->macro->checkSymbols(st);
				return ret;
			}
		default:
			break;
	}
	return 1;
}

bool symbolTable::declare(const std::wstring &identifier,macro *m,ulong line,bool check){
	if (check && !!this->get(identifier))
		return 0;
	this->symbols.push_back(new symbol(identifier,m,line));
	return 1;
}

bool symbolTable::declare(const std::wstring &identifier,const std::wstring &string,ulong line,bool check){
	if (check && !!this->get(identifier))
		return 0;
	this->symbols.push_back(new symbol(identifier,string,line));
	return 1;
}

bool symbolTable::declare(const std::wstring &identifier,long value,ulong line,bool check){
	if (check && !!this->get(identifier))
		return 0;
	this->symbols.push_back(new symbol(identifier,value,line));
	return 1;
}

bool symbolTable::declare(symbol *s,bool check){
	if (check && !!this->get(s->identifier))
		return 0;
	this->symbols.push_back(s);
	return 1;
}

void symbolTable::addFrame(const symbolTable &st){
	for (ulong b=0;b<st.symbols.size();b++){
		if (st.symbols[b]->type==symbol::MACRO)
			continue;
		this->symbols.push_back(new symbol(*st.symbols[b]));
		this->symbols.back()->initialize(st);
	}
}

symbol *symbolTable::get(const std::wstring &name) const{
	for (ulong a=0;a<this->symbols.size();a++)
		if (this->symbols[a]->identifier==name)
			return this->symbols[a];
	return 0;
}

void symbolTable::resetAll(){
	for (ulong a=0;a<this->symbols.size();a++)
		this->symbols[a]->reset();
}

bool symbolTable::checkSymbols(){
	bool res=1;
	std::set<std::wstring> s;
	for (ulong a=0;a<this->symbols.size();a++){
		if (s.find(this->symbols[a]->identifier)!=s.end()){
			THROW_MACRO_ID_ERROR(this->symbols[a]->declared_on_line,"duplicate symbol",this->symbols[a]->identifier);
			res=0;
		}else
			s.insert(this->symbols[a]->identifier);
		res&=this->symbols[a]->checkSymbols(*this);
	}
	return res;
}

bool symbolTable::checkIntersection(const symbolTable &st) const{
	bool r=1;
	std::set<std::wstring> set;
	for (ulong a=0;a<st.symbols.size();a++){
		if (this->get(st.symbols[a]->identifier) || set.find(st.symbols[a]->identifier)!=set.end()){
			THROW_MACRO_ID_ERROR(st.symbols[a]->declared_on_line,"duplicate symbol",st.symbols[a]->identifier);
			r=0;
		}else
			set.insert(st.symbols[a]->identifier);
	}
	return r;
}

std::wstring constantExpression::evaluateToStr(const symbolTable *st,ulong *error){
	return itoa<wchar_t>(this->evaluateToInt(st,error));
}

long constantExpression::evaluateToInt(const symbolTable *st,ulong *error){
	if (error)
		*error=MACRO_NO_ERROR;
	return this->val;
}

std::wstring variableExpression::evaluateToStr(const symbolTable *st,ulong *error){
	if (error)
		*error=MACRO_NO_ERROR;
	symbol *s=st->get(this->id.id);
	return s->getStr();
}

long variableExpression::evaluateToInt(const symbolTable *st,ulong *error){
	if (error)
		*error=MACRO_NO_ERROR;
	symbol *s=st->get(this->id.id);
	return s->getInt();
}

bool variableExpression::checkSymbols(const symbolTable &st){
	return this->id.checkSymbols(st,1);
}

fullExpression::fullExpression(yytokentype op,expression *A,expression *B,expression *C){
	this->operation=op;
	this->operands[0]=A;
	this->operands[1]=B;
	this->operands[2]=C;
}

fullExpression::fullExpression(const fullExpression &b)
		:operation(b.operation){
	memset(this->operands,0,sizeof(expression *)*3);
	for (ulong a=0;a<3 && b.operands[a];a++)
		this->operands[a]=b.operands[a]->clone();
}

fullExpression::~fullExpression(){
	for (ulong a=0;a<3 && this->operands[a];a++)
		delete this->operands[a];
}

bool fullExpression::simplify(){
	bool r=1;
	for (ulong a=0;a<3 && this->operands[a];a++){
		bool temp=this->operands[a]->simplify();
		r&=temp;
		if (temp){
			long val=this->operands[a]->evaluateToInt();
			delete this->operands[a];
			this->operands[a]=new constantExpression(val);
		}
	}
	return r;
}

expression *simplifyExpression(expression *e){
	if (!e->simplify())
		return e;
	constantExpression *ret=new constantExpression(e->evaluateToInt());
	delete e;
	return ret;
}

std::wstring fullExpression::evaluateToStr(const symbolTable *st,ulong *error){
	return itoa<wchar_t>(this->evaluateToInt(st,error));
}

#define EVALUATETOINT_EVAL(i) {\
	ulong error2;\
	ops[(i)]=this->operands[(i)]->evaluateToInt(st,&error2);\
	if (error2!=MACRO_NO_ERROR){\
		if (!!error)\
			*error=error2;\
		return 0;\
	}\
}


long fullExpression::evaluateToInt(const symbolTable *st,ulong *error){
	long ops[3]={0},
		res;
	EVALUATETOINT_EVAL(0);
	switch (this->operation){
		case TRINARY:
			if (ops[0]){
				EVALUATETOINT_EVAL(1);
				res=ops[1];
			}else{
				EVALUATETOINT_EVAL(2);
				res=ops[2];
			}
			break;
		case BOR:
			if (ops[0])
				res=ops[0];
			else{
				EVALUATETOINT_EVAL(1);
				res=ops[1];
			}
			break;
		case BAND:
			if (!ops[0])
				res=ops[0];
			else{
				EVALUATETOINT_EVAL(1);
				res=ops[1];
			}
			break;
		case BNOT:
			res=!ops[0];
			break;
		case NOT_EQUALS:
			EVALUATETOINT_EVAL(1);
			res=ops[0]!=ops[1];
			break;
		case EQUALS:
			EVALUATETOINT_EVAL(1);
			res=ops[0]==ops[1];
			break;
		case LT_EQUALS:
			EVALUATETOINT_EVAL(1);
			res=ops[0]<=ops[1];
			break;
		case GT_EQUALS:
			EVALUATETOINT_EVAL(1);
			res=ops[0]>=ops[1];
			break;
		case LOWER_THAN:
			EVALUATETOINT_EVAL(1);
			res=ops[0]<ops[1];
			break;
		case GREATER_THAN:
			EVALUATETOINT_EVAL(1);
			res=ops[0]>ops[1];
			break;
		case PLUS:
			EVALUATETOINT_EVAL(1);
			res=ops[0]+ops[1];
			break;
		case MINUS:
			EVALUATETOINT_EVAL(1);
			res=ops[0]-ops[1];
			break;
		case MUL:
			EVALUATETOINT_EVAL(1);
			res=ops[0]*ops[1];
			break;
		case DIV:
			EVALUATETOINT_EVAL(1);
			res=ops[0]/ops[1];
			break;
		case MOD:
			EVALUATETOINT_EVAL(1);
			res=ops[0]%ops[1];
			break;
		case POS:
			res=ops[0];
			break;
		case NEG:
			res=-ops[0];
			break;
		default: //Impossible
			assert(0);
			res=0;
			break;
	}
	if (!!error)
		*error=MACRO_NO_ERROR;
	return res;
}

bool fullExpression::checkSymbols(const symbolTable &st){
	bool r=1;
	for (ulong a=0;a<3 && this->operands[a];a++)
		r=r & this->operands[a]->checkSymbols(st);
	return r;
}

std::wstring constantString::evaluateToStr(const symbolTable *st,ulong *error){
	if (error)
		*error=MACRO_NO_ERROR;
	return this->val;
}

long constantString::evaluateToInt(const symbolTable *st,ulong *error){
	if (error)
		*error=MACRO_NO_ERROR;
	return atoi(this->val);
}

std::wstring variableString::evaluateToStr(const symbolTable *st,ulong *error){
	if (error)
		*error=MACRO_NO_ERROR;
	symbol *s=st->get(this->id.id);
	return s->getStr();
}

long variableString::evaluateToInt(const symbolTable *st,ulong *error){
	if (error)
		*error=MACRO_NO_ERROR;
	symbol *s=st->get(this->id.id);
	return s->getInt();
}

bool variableString::checkSymbols(const symbolTable &st){
	return this->id.checkSymbols(st,1);
}

stringConcatenation::stringConcatenation(string *A,string *B){
	this->operands[0]=A;
	this->operands[1]=B;
}

stringConcatenation::stringConcatenation(const stringConcatenation &a){
	this->operands[0]=a.operands[0]->clone();
	this->operands[1]=a.operands[0]?a.operands[0]->clone():0;
}

stringConcatenation::~stringConcatenation(){
	delete this->operands[0];
	delete this->operands[1];
}

bool stringConcatenation::simplify(){
	bool r=1;
	for (ulong a=0;a<2 && this->operands[a];a++){
		bool temp=this->operands[a]->simplify();
		r&=temp;
		if (temp){
			std::wstring val=this->operands[a]->evaluateToStr();
			delete this->operands[a];
			this->operands[a]=new constantString(val);
		}
	}
	return r;
}

string *simplifyExpression(string *e){
	if (!e->simplify())
		return e;
	constantString *ret=new constantString(e->evaluateToStr());
	delete e;
	return ret;
}

std::wstring stringConcatenation::evaluateToStr(const symbolTable *st,ulong *error){
	std::wstring res;
	ulong error2;
	for (ulong a=0;a<2 && this->operands[a];a++){
		std::wstring temp=this->operands[a]->evaluateToStr(st,&error2);
		if (error2!=MACRO_NO_ERROR){
			if (error)
				*error=error2;
			return L"";
		}
		res.append(temp);
	}
	if (error)
		*error=MACRO_NO_ERROR;
	return res;
}

long stringConcatenation::evaluateToInt(const symbolTable *st,ulong *error){
	return atoi(this->evaluateToStr(st,error));
}

bool stringConcatenation::checkSymbols(const symbolTable &st){
	bool r=1;
	for (ulong a=0;a<2 && this->operands[a];a++)
		r&=this->operands[a]->checkSymbols(st);
	return r;
}

std::wstring emptyStatement::perform(symbolTable symbol_table,ulong *error){
	if (!!error)
		*error=MACRO_NO_ERROR;
	return L"";
}

template <typename T>
bool partialCompare(const std::basic_string<T> &A,size_t offset,const std::basic_string<T> &B){
	if (A.size()-offset<B.size())
		return 0;
	for (size_t a=offset,b=0;b<B.size();a++,b++)
		if (A[a]!=B[b])
			return 0;
	return 1;
}

std::wstring dataBlock::replace(const std::wstring &src,const symbolTable &symbol_table){
	if (!symbol_table.symbols.size())
		return src;
	const std::vector<symbol *> &symbols=symbol_table.symbols;
	std::wstring res;
	for (ulong a=0;a<src.size();a++){
		wchar_t c=src[a];
		bool _continue=0;
		for (ulong b=0;b<symbols.size() && !_continue;b++){
			if (partialCompare(src,a,symbols[b]->identifier)){
				if (symbols[b]->type==symbol::STRING){
					res.append(symbols[b]->str_val);
					a+=symbols[b]->identifier.size()-1;
					_continue=1;
				}else if (symbols[b]->type==symbol::INTEGER){
					std::wstring temp=itoa<wchar_t>(symbols[b]->int_val);
					res.append(temp);
					a+=symbols[b]->identifier.size()-1;
					_continue=1;
				}
			}
		}
		if (_continue)
			continue;
		res.push_back(c);
	}
	return res;
}

std::wstring dataBlock::perform(symbolTable symbol_table,ulong *error){
	if (!!error)
		*error=MACRO_NO_ERROR;
	return dataBlock::replace(this->data,symbol_table);
}

std::wstring assignmentStatement::perform(symbolTable st,ulong *error){
	ulong error2;
	long val=this->src->evaluateToInt(&st,&error2);
	if (error2!=MACRO_NO_ERROR){
		if (!!error)
			*error=error2;
		return L"";
	}
	st.get(this->dst.id)->set(val);
	if (!!error)
		*error=MACRO_NO_ERROR;
	return L"";
}

bool assignmentStatement::checkSymbols(const symbolTable &st){
	return this->dst.checkSymbols(st,1) && this->src->checkSymbols(st);
}

stringAssignmentStatement::stringAssignmentStatement(const identifier &id,string *e):dst(id){
	this->symbol=0;
	this->src=e;
}

stringAssignmentStatement::~stringAssignmentStatement(){
	delete this->src;
}

std::wstring stringAssignmentStatement::perform(symbolTable st,ulong *error){
	ulong error2;
	std::wstring val=this->src->evaluateToStr(&st,&error2);
	if (error2!=MACRO_NO_ERROR){
		if (!!error)
			*error=error2;
		return L"";
	}
	st.get(this->dst.id)->set(val);
	if (!!error)
		*error=MACRO_NO_ERROR;
	return L"";
}

bool stringAssignmentStatement::checkSymbols(const symbolTable &st){
	return this->dst.checkSymbols(st,1) & this->src->checkSymbols(st);
}

ifStructure::ifStructure(expression *a,block *b,block *c){
	this->condition=a;
	this->true_block=b;
	this->false_block=c;
}

ifStructure::~ifStructure(){
	delete this->condition;
	delete this->true_block;
	if (this->false_block)
		delete this->false_block;
}

std::wstring ifStructure::perform(symbolTable st,ulong *error){
	ulong error2;
	bool condition=!!this->condition->evaluateToInt(&st,&error2);
	if (error2!=MACRO_NO_ERROR){
		if (!!error)
			*error=error2;
		return L"";
	}
	if (condition)
		return this->true_block->perform(&st,error);
	else if (!!this->false_block)
		return this->false_block->perform(&st,error);
	return L"";
}

bool ifStructure::checkSymbols(const symbolTable &st){
	bool r=this->condition->checkSymbols(st);
	r&=this->true_block->checkSymbols(st);
	if (this->false_block)
		r&=this->false_block->checkSymbols(st);
	return r;
}

whileStructure::whileStructure(expression *a,NONS_Macro::block *b){
	this->condition=a;
	this->block=b;
}

whileStructure::~whileStructure(){
	delete this->condition;
	delete this->block;
}

std::wstring whileStructure::perform(symbolTable *st,ulong *error){
	ulong error2=0;
	std::wstring res;
	if (this->block->symbol_table)
		st->addFrame(*this->block->symbol_table);
	while (1){
		long _while=this->condition->evaluateToInt(st,&error2);
		if (error2!=MACRO_NO_ERROR){
			if (!!error)
				*error=error2;
			return L"";
		}
		if (!_while)
			break;
		res.append(this->block->perform(st,&error2,1));
		if (error2!=MACRO_NO_ERROR){
			if (!!error)
				*error=error2;
			return L"";
		}
	}
	if (!!error)
		*error=MACRO_NO_ERROR;
	return res;
}

std::wstring whileStructure::perform(symbolTable st,ulong *error){
	return this->perform(&st,error);
}

bool whileStructure::checkSymbols(const symbolTable &st){
	symbolTable st2=st;
	bool r;
	if (this->block->symbol_table){
		r=st.checkIntersection(*this->block->symbol_table);
		st2.addFrame(*this->block->symbol_table);
	}
	r&=this->condition->checkSymbols(st);
	r&=this->block->checkSymbols(st,1);
	return r;
}

forStructure::forStructure(const identifier &s,expression *e1,expression *e2,expression *e3,NONS_Macro::block *b):forIndex(s){
	this->start=e1;
	this->end=e2;
	this->step=e3;
	this->block=b;
}

forStructure::~forStructure(){
	delete this->start;
	delete this->end;
	delete this->step;
	delete this->block;
}

std::wstring forStructure::perform(symbolTable *st,ulong *error){
	ulong error2=0;
	std::wstring res;
	st->declare(this->forIndex.id,(long)0,this->forIndex.referenced_on_line,0);
	if (this->block->symbol_table)
		st->addFrame(*this->block->symbol_table);
	long start=this->start->evaluateToInt(st,&error2);
	if (error2!=MACRO_NO_ERROR){
		if (!!error)
			*error=error2;
		return L"";
	}
	symbol *s=st->get(this->forIndex.id);
	s->set(start);
	while (1){
		long end=this->end->evaluateToInt(st,&error2);
		if (error2!=MACRO_NO_ERROR){
			if (!!error)
				*error=error2;
			return L"";
		}
		if (s->getInt()>end)
			break;
		res.append(this->block->perform(st,&error2,1));
		if (error2!=MACRO_NO_ERROR){
			if (!!error)
				*error=error2;
			return L"";
		}
		long step=this->step->evaluateToInt(st,&error2);
		if (error2!=MACRO_NO_ERROR){
			if (!!error)
				*error=error2;
			return L"";
		}
		s->set(s->getInt()+step);
	}
	if (!!error)
		*error=MACRO_NO_ERROR;
	return res;
}

std::wstring forStructure::perform(symbolTable st,ulong *error){
	return this->perform(&st,error);
}

bool forStructure::checkSymbols(const symbolTable &st){
	symbolTable st2=st;
	bool r=st2.declare(this->forIndex.id,(long)0,this->forIndex.referenced_on_line,0);
	if (this->block->symbol_table){
		r&=st.checkIntersection(*this->block->symbol_table);
		st2.addFrame(*this->block->symbol_table);
	}
	r&=this->forIndex.checkSymbols(st2,1);
	r&=this->start->checkSymbols(st2);
	r&=this->end->checkSymbols(st2);
	r&=this->step->checkSymbols(st2);
	r&=this->block->checkSymbols(st2,1);
	return r;
}

macroCall::~macroCall(){
	if (this->arguments){
		for (ulong a=0;a<this->arguments->size();a++)
			delete (*this->arguments)[a];
		delete this->arguments;
	}
}

std::wstring macroCall::perform(symbolTable st,ulong *error){
	std::vector<std::wstring> parameters;
	if (!!this->arguments){
		parameters.resize(this->arguments->size());
		ulong error2;
		for (ulong a=0;a<parameters.size();a++){
			parameters[a]=(*this->arguments)[a]->evaluateToStr(&st,&error2);
			if (error2!=MACRO_NO_ERROR){
				if (!!error)
					*error=error2;
				return L"";
			}
		}
	}
	return st.get(this->id.id)->macro->perform(parameters,&st,error);
}

bool macroCall::checkSymbols(const symbolTable &st){
	bool r=this->id.checkSymbols(st,0);
	for (ulong a=0;a<this->arguments->size();a++)
		r&=(*this->arguments)[a]->checkSymbols(st);
	return r;
}

block::block(std::vector<statement *> *a,symbolTable *b){
	this->statements=a;
	this->symbol_table=b;
}

block::~block(){
	for (ulong a=0;a<this->statements->size();a++)
		delete (*this->statements)[a];
	delete this->statements;
}

std::wstring block::perform(symbolTable *symbol_table,ulong *error,bool doNotAddFrame){
	if (!doNotAddFrame && this->symbol_table)
		symbol_table->addFrame(*this->symbol_table);
	std::wstring res;
	ulong error2=0;
	for (ulong a=0;a<this->statements->size();a++){
		res.append((*this->statements)[a]->perform(*symbol_table,&error2));
		if (error2){
			if (!!error)
				*error=error2;
			return L"";
		}
	}
	if (!!error)
		*error=MACRO_NO_ERROR;
	return res;
}

std::wstring block::perform(symbolTable symbol_table,ulong *error,bool doNotAddFrame){
	return this->perform(&symbol_table,error,doNotAddFrame);
}

void block::addStatement(statement *s){
	if (!this->statements)
		this->statements=new std::vector<statement *>;
	this->statements->push_back(s);
}

bool block::checkSymbols(const symbolTable &st,bool doNotAddFrame){
	symbolTable st2=st;
	bool r=1;
	if (!doNotAddFrame && this->symbol_table && (r=st.checkIntersection(*this->symbol_table)))
		st2.addFrame(*this->symbol_table);
	for (ulong a=0;a<this->statements->size();a++)
		r&=(*this->statements)[a]->checkSymbols(st2);
	return r;
}

macro::macro(block *c,symbolTable *b){
	this->statements=c;
	this->parameters=b;
	if (!this->parameters)
		this->parameters=new symbolTable;
}

macro::~macro(){
	delete this->parameters;
	delete this->statements;
}

std::wstring macro::perform(const std::vector<std::wstring> &parameters,symbolTable *st,ulong *error){
	ulong first=st->symbols.size();
	if (this->parameters)
		st->addFrame(*this->parameters);
	for (ulong a=first;a<st->symbols.size() && a-first<parameters.size();a++)
		st->symbols[a]->set(parameters[a-first]);
	return this->statements->perform(st,error);
}

std::wstring macro::perform(const std::vector<std::wstring> &parameters,symbolTable st,ulong *error){
	return this->perform(parameters,&st,error);
}

bool macro::checkSymbols(const symbolTable &st){
	symbolTable st2=st;
	bool r=1;
	if (this->parameters && (r=st.checkIntersection(*this->parameters)))
		st2.addFrame(*this->parameters);
	return r & this->statements->checkSymbols(st2);
}

std::wstring macroFile::call(const std::wstring &name,const std::vector<std::wstring> &parameters,ulong *error){
	symbol *s=this->symbol_table.get(name);
	if (!s || s->type!=symbol::MACRO){
		if (!!error)
			*error=MACRO_NO_SUCH_MACRO;
		return L"";
	}
	return s->macro->perform(parameters,this->symbol_table,error);
}

bool macroFile::checkSymbols(){
	return this->symbol_table.checkSymbols();
}
}
#endif