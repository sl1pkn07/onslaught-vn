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
	this->archive=new NONS_Archive("arc.sar",1);
	this->archive->readArchive();
	if (!this->archive->loaded){
		delete this->archive;
		this->archive=new NONS_Archive("ARC.SAR",1);
		this->archive->readArchive();
		if (!this->archive->loaded){
			delete this->archive;
			this->archive=0;
		}
	}
	{
		const char *filenames[]={
			"arc.nsa",
			"ARC.NSA",
			"arc1.nsa",
			"ARC1.NSA",
			"arc2.nsa",
			"ARC2.NSA",
			"arc3.nsa",
			"ARC3.NSA",
			"arc4.nsa",
			"ARC4.NSA",
			"arc5.nsa",
			"ARC5.NSA",
			"arc6.nsa",
			"ARC6.NSA",
			"arc7.nsa",
			"ARC7.NSA",
			"arc8.nsa",
			"ARC8.NSA",
			"arc9.nsa",
			"ARC9.NSA",
			0
		};
		for (short a=0;filenames[a];a++)
			this->init(filenames[a],1,1);
	}
}

NONS_GeneralArchive::~NONS_GeneralArchive(){
	if (this->archive)
		delete this->archive;
	for (ulong a=0;a<this->NSAarchives.size();a++)
		if (this->NSAarchives[a])
			delete this->NSAarchives[a];
}

ErrorCode NONS_GeneralArchive::init(const char *filename,bool which,bool failSilently){
	if (!filename)
		return NONS_INTERNAL_INVALID_PARAMETER;
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
			if (!strcmp(this->NSAarchives[a]->path,filename)){
				delete temp;
				return NONS_ALREADY_INITIALIZED;
			}
		}
		this->NSAarchives.push_back(temp);
	}
	return NONS_NO_ERROR;
}

uchar *NONS_GeneralArchive::getFileBuffer(const wchar_t *filepath,ulong *buffersize){
	uchar *res=0;
	for (long a=this->NSAarchives.size()-1;a>=0;a--){
		if (res=this->NSAarchives[a]->getFileBuffer(filepath,buffersize))
			return res;
	}
	if (this->archive && (res=this->archive->getFileBuffer(filepath,buffersize)))
		return res;
	char *copy=copyString(filepath);
	res=readfile(copy,(long *)buffersize);
	delete[] copy;
	return res;
}

uchar *NONS_GeneralArchive::getFileBuffer(const char *filepath,ulong *buffersize){
	wchar_t *temp=copyWString(filepath);
	uchar *ret=this->getFileBuffer(temp,buffersize);
	delete[] temp;
	return ret;
}

bool NONS_GeneralArchive::exists(const wchar_t *filepath){
	for (long a=this->NSAarchives.size()-1;a>=0;a--)
		if (this->NSAarchives[a]->exists(filepath))
			return 1;
	if (this->archive && this->archive->exists(filepath))
		return 1;
	char *copy=copyString(filepath);
	bool res=fileExists(copy);
	delete[] copy;
	return res;
}
#endif
