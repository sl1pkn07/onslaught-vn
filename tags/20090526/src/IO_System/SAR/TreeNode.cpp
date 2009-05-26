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

#ifndef NONS_TREENODE_CPP
#define NONS_TREENODE_CPP

#include "TreeNode.h"
#include "../../Functions.h"
#include "../../UTF.h"
#include "../../Globals.h"

NONS_TreeNode::NONS_TreeNode(const wchar_t *name){
	this->branches=0;
	this->data.name=copyWString(name);
}

NONS_TreeNode::~NONS_TreeNode(){
	if (this->branches){
		for (ulong a=0;a<this->branches->size();a++)
			delete this->branches->at(a);
		delete this->branches;
	}
}

NONS_TreeNode *NONS_TreeNode::getBranch(const wchar_t *name,bool createIfMissing){
	if (!this->branches || !name)
		return 0;
	std::vector<NONS_TreeNode *> *pBranches=this->branches;
	long pos=instr(name,L"/");
	if (pos<0)
		pos=instr(name,L"\\");
	wchar_t *name0=0;
	wchar_t *name1=0;
	if (pos>=0){
		name0=copyWString(name,pos);
		name1=copyWString(name+pos+1);
		NONS_tolower(name0);
		NONS_tolower(name1);
	}else{
		name0=copyWString(name);
		NONS_tolower(name0);
	}
	for (ulong a=0;a<pBranches->size();a++){
		if (!wcscmp((*pBranches)[a]->data.name,name0)){
			delete[] name0;
			if (!name1){
				return (*pBranches)[a];
			}
			NONS_TreeNode *res=(*pBranches)[a]->getBranch(name1,createIfMissing);
			delete[] name1;
			return res;
		}
	}
	delete[] name0;
	if (name1)
		delete[] name1;
	return createIfMissing?this->newBranch(name):0;
}

NONS_TreeNode *NONS_TreeNode::newBranch(const wchar_t *name){
	long pos=instr(name,L"/");
	if (pos<0)
		pos=instr(name,L"\\");
	NONS_TreeNode *res;
	if (pos>=0){
		wchar_t *name0=copyWString(name,pos);
		wchar_t *name1=copyWString(name+pos+1);
		res=new NONS_TreeNode(name0);
		res->makeDirectory();
		this->branches->push_back(res);
		res=res->newBranch(name1);
		delete[] name0;
		delete[] name1;
	}else{
		res=new NONS_TreeNode(name);
		this->branches->push_back(res);
	}
	return res;
}

void NONS_TreeNode::makeDirectory(){
	this->branches=new std::vector<NONS_TreeNode *>();
	this->branches->reserve(10);
}
#endif
