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

#ifndef NONS_GENERALARCHIVE_CPP
#define NONS_GENERALARCHIVE_CPP

#include "GeneralArchive.h"
#include "../../Functions.h"
#include "../../Globals.h"
#include "../FileIO.h"

NONS_GeneralArchive::NONS_GeneralArchive(){
	std::wstring path;
	if (CLOptions.archiveDirectory.size())
		path=CLOptions.archiveDirectory+L"/";
	else
		path=L"./";
	this->archive=new NONS_Archive(path+L"arc.sar",1);
	this->archive->readArchive();
	if (!this->archive->loaded){
		delete this->archive;
		this->archive=new NONS_Archive(path+L"ARC.SAR",1);
		this->archive->readArchive();
		if (!this->archive->loaded){
			delete this->archive;
			this->archive=0;
		}
	}
	const wchar_t *filenames[]={
		L"arc.nsa",
		L"ARC.NSA",
		L"arc1.nsa",
		L"ARC1.NSA",
		L"arc2.nsa",
		L"ARC2.NSA",
		L"arc3.nsa",
		L"ARC3.NSA",
		L"arc4.nsa",
		L"ARC4.NSA",
		L"arc5.nsa",
		L"ARC5.NSA",
		L"arc6.nsa",
		L"ARC6.NSA",
		L"arc7.nsa",
		L"ARC7.NSA",
		L"arc8.nsa",
		L"ARC8.NSA",
		L"arc9.nsa",
		L"ARC9.NSA",
		0
	};
	for (const wchar_t **p=filenames;*p;p++)
		this->init(path+*p,1,1);
}

NONS_GeneralArchive::~NONS_GeneralArchive(){
	if (this->archive)
		delete this->archive;
	for (ulong a=0;a<this->NSAarchives.size();a++)
		if (this->NSAarchives[a])
			delete this->NSAarchives[a];
}

ErrorCode NONS_GeneralArchive::init(const std::wstring &filename,bool which,bool failSilently){
	if (!which && this->archive)
		return NONS_ALREADY_INITIALIZED;
	NONS_Archive *temp=new NONS_Archive(filename,failSilently);
	if (!temp->readArchive()){
		delete temp;
		return NONS_INVALID_ARCHIVE;
	}
	if (!which)
		this->archive=temp;
	else{
		for (ulong a=0;a<this->NSAarchives.size();a++){
			if (this->NSAarchives[a]->path==filename){
				delete temp;
				return NONS_ALREADY_INITIALIZED;
			}
		}
		this->NSAarchives.push_back(temp);
	}
	return NONS_NO_ERROR;
}

uchar *NONS_GeneralArchive::getFileBuffer(const std::wstring &filepath,ulong &buffersize){
	uchar *res=0;
	for (long a=this->NSAarchives.size()-1;a>=0;a--){
		if (res=this->NSAarchives[a]->getFileBuffer(filepath,buffersize))
			return res;
	}
	if (this->archive && (res=this->archive->getFileBuffer(filepath,buffersize)))
		return res;
	res=readfile(filepath,buffersize);
	return res;
}

bool NONS_GeneralArchive::exists(const std::wstring &filepath){
	for (long a=this->NSAarchives.size()-1;a>=0;a--)
		if (this->NSAarchives[a]->exists(filepath))
			return 1;
	if (this->archive && this->archive->exists(filepath))
		return 1;
	return fileExists(filepath);
}
#endif
