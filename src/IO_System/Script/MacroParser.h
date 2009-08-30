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

#ifndef NONS_MACROPARSER_H
#define NONS_MACROPARSER_H
#include "../../Common.h"
#include "MacroParser.tab.hpp"
#include <vector>
#include <string>
#include <set>

namespace NONS_Macro{

struct macro;
struct expression;
struct string;
struct symbolTable;
struct identifier;

struct symbol{
	enum{
		UNDEFINED,
		MACRO,
		INTEGER,
		STRING
	} type;
	std::wstring identifier;
	ulong declared_on_line;
	macro *macro;
	long int_val;
	std::wstring str_val;
	expression *int_initialization;
	string *str_initialization;
	bool has_been_checked;
	symbol(const std::wstring &,NONS_Macro::macro *,ulong);
	symbol(const std::wstring &,ulong);
	symbol(const std::wstring &,const std::wstring &,ulong);
	symbol(const std::wstring &,long,ulong);
	symbol(const symbol &);
	~symbol();
	void reset();
	void set(long);
	void set(const std::wstring &);
	long getInt();
	std::wstring getStr();
	void initializeTo(expression *);
	void initializeTo(string *);
	ulong initialize(const symbolTable &);
	bool checkSymbols(const symbolTable &);
};

struct identifier{
	std::wstring id;
	ulong referenced_on_line;
	identifier(const std::wstring &a,ulong b):id(a),referenced_on_line(b){}
	bool checkSymbols(const symbolTable &st,bool expectedVariable);
};

struct symbolTable{
	std::vector<symbol *> symbols;
	symbolTable(){};
	symbolTable(const symbolTable &a):symbols(a.symbols){}
	bool declare(const std::wstring &,macro *,ulong,bool check=1);
	bool declare(const std::wstring &,const std::wstring &,ulong,bool check=1);
	bool declare(const std::wstring &,long,ulong,bool check=1);
	bool declare(symbol *,bool check=1);
	void addFrame(const symbolTable &);
	symbol *get(const std::wstring &) const;
	void resetAll();
	bool checkSymbols();
	bool checkIntersection(const symbolTable &) const;
};

enum ErrorCode{
	MACRO_NO_ERROR=0,
	MACRO_NO_SUCH_MACRO,
	MACRO_NO_SYMBOL_FOUND,
	MACRO_NUMBER_EXPECTED
};

struct argument{
	virtual ~argument(){}
	virtual bool simplify()=0;
	virtual std::wstring evaluateToStr(const symbolTable * =0,ulong * =0)=0;
	virtual long evaluateToInt(const symbolTable * =0,ulong * =0)=0;
	virtual bool checkSymbols(const symbolTable &)=0;
};

struct expression:argument{
	virtual expression *clone()=0;
	virtual ~expression(){}
};

expression *simplifyExpression(expression *);

struct constantExpression:expression{
	long val;
	constantExpression(long a):val(a){}
	constantExpression(const constantExpression &b):val(b.val){}
	expression *clone(){
		return this?new constantExpression(*this):0;
	}
	bool simplify(){
		return 1;
	}
	std::wstring evaluateToStr(const symbolTable * =0,ulong *error=0);
	long evaluateToInt(const symbolTable * =0,ulong *error=0);
	bool checkSymbols(const symbolTable &){return 1;}
};

struct variableExpression:expression{
	identifier id;
	symbol *s;
	variableExpression(const identifier &a):id(a),s(0){}
	variableExpression(const variableExpression &b):id(b.id),s(b.s){}
	expression *clone(){
		return this?new variableExpression(*this):0;
	}
	bool simplify(){return 0;};
	std::wstring evaluateToStr(const symbolTable * =0,ulong *error=0);
	long evaluateToInt(const symbolTable * =0,ulong *error=0);
	bool checkSymbols(const symbolTable &st);
};

struct fullExpression:expression{
	yytokentype operation;
	expression *operands[3];
	fullExpression(yytokentype,expression * =0,expression * =0,expression * =0);
	fullExpression(const fullExpression &b);
	expression *clone(){
		return this?new fullExpression(*this):0;
	}
	~fullExpression();
	bool simplify();
	std::wstring evaluateToStr(const symbolTable * =0,ulong *error=0);
	long evaluateToInt(const symbolTable * =0,ulong *error=0);
	bool checkSymbols(const symbolTable &st);
};

struct string:argument{
	virtual string *clone()=0;
	virtual ~string(){}
};

string *simplifyString(string *);

struct constantString:string{
	std::wstring val;
	constantString(const std::wstring &a):val(a){}
	constantString(const constantString &a):val(a.val){}
	string *clone(){
		return this?new constantString(*this):0;
	}
	bool simplify(){return 1;}
	std::wstring evaluateToStr(const symbolTable * =0,ulong *error=0);
	long evaluateToInt(const symbolTable * =0,ulong *error=0);
	bool checkSymbols(const symbolTable &){return 1;}
};

struct variableString:string{
	identifier id;
	variableString(const identifier &a):id(a){}
	variableString(const variableString &a):id(a.id){}
	string *clone(){
		return this?new variableString(*this):0;
	}
	bool simplify(){return 0;}
	std::wstring evaluateToStr(const symbolTable * =0,ulong *error=0);
	long evaluateToInt(const symbolTable * =0,ulong *error=0);
	bool checkSymbols(const symbolTable &st);
};

struct stringConcatenation:string{
	string *operands[2];
	stringConcatenation(string *,string *);
	stringConcatenation(const stringConcatenation &a);
	string *clone(){
		return this?new stringConcatenation(*this):0;
	}
	~stringConcatenation();
	bool simplify();
	std::wstring evaluateToStr(const symbolTable * =0,ulong *error=0);
	long evaluateToInt(const symbolTable * =0,ulong *error=0);
	bool checkSymbols(const symbolTable &st);
};

struct block;
struct macroFile;

struct statement{
	virtual ~statement(){}
	virtual std::wstring perform(symbolTable,ulong * =0)=0;
	virtual bool checkSymbols(const symbolTable &)=0;
};

struct emptyStatement:statement{
	emptyStatement(){}
	std::wstring perform(symbolTable symbol_table,ulong *error=0);
	bool checkSymbols(const symbolTable &){return 1;}
};

struct dataBlock:statement{
	std::wstring data;
	dataBlock(const std::wstring &a):data(a){}
	std::wstring perform(symbolTable symbol_table,ulong *error=0);
	bool checkSymbols(const symbolTable &){return 1;}
private:
	static std::wstring replace(const std::wstring &src,const symbolTable &symbol_table);
};

struct assignmentStatement:statement{
	identifier dst;
	expression *src;
	assignmentStatement(const identifier &id,expression *e):dst(id),src(e){}
	~assignmentStatement(){delete this->src;}
	std::wstring perform(symbolTable symbol_table,ulong *error=0);
	bool checkSymbols(const symbolTable &);
};

struct stringAssignmentStatement:statement{
	identifier dst;
	symbol *symbol;
	string *src;
	stringAssignmentStatement(const identifier &,string *);
	~stringAssignmentStatement();
	std::wstring perform(symbolTable symbol_table,ulong *error=0);
	bool checkSymbols(const symbolTable &);
};

struct ifStructure:statement{
	expression *condition;
	block *true_block,
		*false_block;
	ifStructure(expression *,block *,block * =0);
	~ifStructure();
	std::wstring perform(symbolTable symbol_table,ulong *error=0);
	bool checkSymbols(const symbolTable &);
};

struct whileStructure:statement{
	expression *condition;
	block *block;
	whileStructure(expression *,NONS_Macro::block *);
	~whileStructure();
	std::wstring perform(symbolTable *symbol_table,ulong *error=0);
	std::wstring perform(symbolTable symbol_table,ulong *error=0);
	bool checkSymbols(const symbolTable &);
};

struct forStructure:statement{
	identifier forIndex;
	expression *start,
		*end,
		*step;
	block *block;
	forStructure(const identifier &,expression *,expression *,expression *,NONS_Macro::block *);
	~forStructure();
	std::wstring perform(symbolTable *symbol_table,ulong *error=0);
	std::wstring perform(symbolTable symbol_table,ulong *error=0);
	bool checkSymbols(const symbolTable &);
};

struct macroCall:statement{
	identifier id;
	std::vector<argument *> *arguments;
	macroCall(const identifier &a,std::vector<argument *> *b=0):id(a),arguments(b){}
	~macroCall();
	std::wstring perform(symbolTable symbol_table,ulong *error=0);
	bool checkSymbols(const symbolTable &);
};

struct block{
	std::vector<statement *> *statements;
	symbolTable *symbol_table;
	block(std::vector<statement *> * =0,symbolTable * =0);
	~block();
	std::wstring perform(symbolTable *symbol_table,ulong *error=0,bool doNotAddFrame=0);
	std::wstring perform(symbolTable symbol_table,ulong *error=0,bool doNotAddFrame=0);
	void addStatement(statement *);
	//void addStatements(std::vector<statement *> *);
	bool checkSymbols(const symbolTable &,bool doNotAddFrame=0);
};

struct macro{
	symbolTable *parameters;
	block *statements;
	macro(block *,symbolTable * =0);
	~macro();
	std::wstring perform(const std::vector<std::wstring> &parameters,symbolTable *st,ulong *error=0);
	std::wstring perform(const std::vector<std::wstring> &parameters,symbolTable st,ulong *error=0);
	bool checkSymbols(const symbolTable &);
};

struct macroFile{
	symbolTable symbol_table;
	std::wstring call(const std::wstring &name,const std::vector<std::wstring> &parameters,ulong *error=0);
	bool checkSymbols();
};
}
#endif