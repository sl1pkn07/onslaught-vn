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
#include "Functions.h"
#include <algorithm>

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

NONS_TreeNode *NONS_TreeNode::getBranch(const wchar_t *name){
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
		tolower(name0);
		tolower(name1);
	}else{
		name0=copyWString(name);
		tolower(name0);
	}
	for (ulong a=0;a<pBranches->size();a++){
		if (!wcscmp((*pBranches)[a]->data.name,name0)){
			delete[] name0;
			if (!name1){
				return (*pBranches)[a];
			}
			NONS_TreeNode *res=(*pBranches)[a]->getBranch(name1);
			delete[] name1;
			return res;
		}
	}
	delete[] name0;
	if (name1)
		delete[] name1;
	return this->newBranch(name);
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

void NONS_TreeNode::sort(){
	if (!this->branches)
		return;
	ulong first_directory=0;
	//First, sort nodes by type. First files, then directories.
	for (ulong a=0;a<this->branches->size();a++){
		if (!!this->branches->at(a)->branches){
			long b=-1;
			for (ulong c=a+1;c<this->branches->size() && b<0;c++)
				if (!this->branches->at(c)->branches)
					b=c;
			if (b<0)
				break;
			std::swap(this->branches->at(a),this->branches->at(b));
		}
		first_directory++;
	}
	//Now, sort all files.
	for (ulong a=0;a<first_directory;a++){
		ulong min=a;
		for (ulong b=a+1;b<first_directory;b++)
			if (wcscmp(this->branches->at(min)->data.name,this->branches->at(b)->data.name)>0)
				min=b;
		if (min==a)
			continue;
		std::swap(this->branches->at(a),this->branches->at(min));
	}
	//And finally, all directories, recursively.
	for (ulong a=first_directory;a<this->branches->size();a++){
		ulong min=a;
		for (ulong b=a+1;b<this->branches->size();b++)
			if (wcscmp(this->branches->at(min)->data.name,this->branches->at(b)->data.name)>0)
				min=b;
		if (min!=a)
			std::swap(this->branches->at(a),this->branches->at(min));
		this->branches->at(a)->sort();
	}
}

void NONS_TreeNode::vectorizeFiles(std::vector<NONS_TreeNode *> &vector){
	if (!this->branches){
		vector.push_back(this);
		return;
	}
	for (ulong a=0;a<this->branches->size();a++)
		this->branches->at(a)->vectorizeFiles(vector);
}
#endif