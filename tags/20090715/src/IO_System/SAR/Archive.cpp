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

#ifndef NONS_ARCHIVE_CPP
#define NONS_ARCHIVE_CPP

#include "Archive.h"
#include "../../Functions.h"
#include "../../Globals.h"
#include "../FileIO.h"
#include <bzlib.h>

NONS_Archive::NONS_Archive(const std::wstring &filename,bool failSilently){
	this->archive_type=UNRECOGNIZED;
	this->file=0;
	this->loaded=0;
	this->root=0;
	if (!fileExists(filename)){
		if (!failSilently)
			o_stderr <<"Error. Could not open \""<<filename<<"\".\n";
		this->file=0;
		this->loaded=0;
		this->root=0;
		return;
	}
	std::wstring temp0=filename;
	toforwardslash(temp0);
	size_t pos=temp0.rfind('/');
	if (pos!=temp0.npos)
		temp0=temp0.substr(pos+1);
	for (size_t a=0;a<temp0.size();a++)
		if (temp0[a]=='.')
			temp0[a]='_';
	this->root=new NONS_TreeNode(temp0);
	this->file=new std::ifstream(wstrToIOstr(filename).c_str(),std::ios::binary);
	this->path=filename;
	pos=this->path.rfind('.');
	std::wstring ext;
	if (pos!=this->path.npos)
		ext=this->path.substr(pos+1);
	tolower(ext);
	if (ext==L"sar")
		this->archive_type=SAR_ARCHIVE;
	else if (ext==L"nsa")
		this->archive_type=NSA_ARCHIVE;
	else if (ext==L"ns2")
		this->archive_type=NS2_ARCHIVE;
	else if (ext==L"ns3")
		this->archive_type=NS3_ARCHIVE;
	else
		this->archive_type=UNRECOGNIZED;
	this->loaded=0;
}

NONS_Archive::~NONS_Archive(){
	if (this->root)
		delete this->root;
	if (this->file)
		delete this->file;
}

bool NONS_Archive::readArchive(){
	if (this->loaded)
		return 1;
	switch (this->archive_type){
		case SAR_ARCHIVE:
			return this->readSAR();
		case NSA_ARCHIVE:
		case NS2_ARCHIVE:
		case NS3_ARCHIVE:
			return this->readNSA();
		case UNRECOGNIZED:
		default:
			return 0;
	}
}

bool NONS_Archive::readSAR(){
	if (!this->file)
		return 0;
	ulong size=4;
	uchar *buffer=readfile(*this->file,size,2);
	long start_of_data_stream=buffer[0]<<24|buffer[1]<<16|buffer[2]<<8|buffer[3];
	this->root->data.offset=start_of_data_stream;
	delete[] buffer;
	size=start_of_data_stream-6;
	buffer=readfile(*this->file,size,6);
	for (ulong pos=0;pos<size;){
		uchar *str=buffer+pos;
		std::wstring name=UniFromSJIS((char *)str);
		tolower(name);
		toforwardslash(name);
		NONS_TreeNode *node=this->root->getBranch(name,1);
		pos+=strlen((char *)str)+1;
		str=buffer+pos;
		node->data.offset=start_of_data_stream+(str[0]<<24|str[1]<<16|str[2]<<8|str[3]);
		pos+=4;
		str=buffer+pos;
		node->data.length=str[0]<<24|str[1]<<16|str[2]<<8|str[3];
		pos+=4;
		node->data.original_length=node->data.length;
		node->data.compression_type=NONS_TreeNode::NONS_ArchivedFile::NO_COMPRESSION;
	}
	delete[] buffer;
	this->loaded=1;
	return 1;
}

bool NONS_Archive::readNSA(){
	if (!this->file)
		return 0;
	ulong size=4;
	uchar *buffer=readfile(*this->file,size,2);
	ulong start_of_data_stream=buffer[0]<<24|buffer[1]<<16|buffer[2]<<8|buffer[3];
	this->root->data.offset=start_of_data_stream;
	delete[] buffer;
	size=start_of_data_stream-6;
	buffer=readfile(*this->file,size,6);
	for (ulong pos=0;pos<size;){
		uchar *str=buffer+pos;
		std::wstring name=UniFromSJIS((char *)str);
		tolower(name);
		toforwardslash(name);
		NONS_TreeNode *node=this->root->getBranch(name,1);
		pos+=strlen((char *)str)+1;
		str=buffer+pos;
		node->data.compression_type=*str;
		pos+=1;
		str=buffer+pos;
		node->data.offset=start_of_data_stream+(str[0]<<24|str[1]<<16|str[2]<<8|str[3]);
		pos+=4;
		str=buffer+pos;
		node->data.length=str[0]<<24|str[1]<<16|str[2]<<8|str[3];
		pos+=4;
		str=buffer+pos;
		node->data.original_length=str[0]<<24|str[1]<<16|str[2]<<8|str[3];
		pos+=4;
	}
	delete[] buffer;
	this->loaded=1;
	return 1;
}

/*#define EI 8
#define EJ 4
#define P 1
#define N (1 << EI)
#define F ((1 << EJ) + P)*/

uchar *NONS_Archive::getFileBuffer(NONS_TreeNode *node,ulong &buffersize){
	uchar *res=0;
	ulong len=0;
	switch (node->data.compression_type){
		case NONS_TreeNode::NONS_ArchivedFile::NO_COMPRESSION:
			len=node->data.length;
			res=readfile(*this->file,len,node->data.offset);
			break;
		case NONS_TreeNode::NONS_ArchivedFile::NBZ_COMPRESSION:
			{
				ulong compressedlen=node->data.length-4;
				uchar *compressedbuffer=readfile(*this->file,compressedlen,node->data.offset+4);
				res=(uchar *)decompressBuffer_BZ2((char *)compressedbuffer,compressedlen,&len);
				delete[] compressedbuffer;
			}
			break;
		case NONS_TreeNode::NONS_ArchivedFile::LZSS_COMPRESSION:
			{
				ulong compressedlen=node->data.length;
				uchar *compressedbuffer=readfile(*this->file,compressedlen,node->data.offset);
				uchar decompression_buffer[256*2];
				memset(decompression_buffer,0,239);
				ulong decompresssion_buffer_offset=239;
				ulong original_length=node->data.original_length;
				res=new uchar[original_length];
				ulong byteoffset=0;
				uchar bitoffset=0;
				while (len<original_length){
					if (getbit(compressedbuffer,&byteoffset,&bitoffset)){
						uchar a=getbits(compressedbuffer,8,&byteoffset,&bitoffset);
						res[len++]=a;
						decompression_buffer[decompresssion_buffer_offset++]=a;
						decompresssion_buffer_offset&=255;
					}else{
						uchar a=getbits(compressedbuffer,8,&byteoffset,&bitoffset);
						uchar b=getbits(compressedbuffer,4,&byteoffset,&bitoffset);
						for (long c=0;c<=b+1 && len<original_length;c++){
							uchar d=decompression_buffer[(a+c)&0xFF];
							res[len++]=d;
							decompression_buffer[decompresssion_buffer_offset++]=d;
							decompresssion_buffer_offset&=255;
						}
					}
				}
				delete[] compressedbuffer;
			}
			break;
		case NONS_TreeNode::NONS_ArchivedFile::SPB_COMPRESSION:
			{
				ulong compressedlen=node->data.length;
				uchar *compressedbuffer=readfile(*this->file,compressedlen,node->data.offset);
				ulong pos=0;
				ushort width=(compressedbuffer[pos]<<8)|compressedbuffer[pos+1];
				pos+=2;
				ushort height=(compressedbuffer[pos]<<8)|compressedbuffer[pos+1];
				pos+=2;
				size_t width_pad=(4-width*3%4)%4;
				size_t original_length=(width*3+width_pad)*height+54;
				res=new uchar[original_length];
				len=original_length;
				memset(res,0,54);
				res[0]='B';
				res[1]='M';
				res[2]=original_length&0xFF;
				res[3]=(original_length>>8)&0xFF;
				res[4]=(original_length>>16)&0xFF;
				res[5]=(original_length>>24)&0xFF;
				res[10]=54;
				res[14]=40;
				res[18]=width&0xFF;
				res[19]=(width>>8)&0xFF;
				res[22]=height&0xFF;
				res[23]=(height>>8)&0xFF;
				res[26]=1;
				res[28]=24;
				res[34]=original_length-54;
				uchar *buf=res+54;
				ulong decompressionbufferlen=width*height+4;
				uchar *decompressionbuffer=new uchar[decompressionbufferlen];
				uchar bitoffset=0;
				ulong surface=width*height;
				for (uchar a=0;a<3;a++){
					ulong count=0;
					uchar x=getbits(compressedbuffer,8,&pos,&bitoffset);
					decompressionbuffer[count++]=x;
					while (count<surface){
						uchar n=getbits(compressedbuffer,3,&pos,&bitoffset);
						if (!n){
							decompressionbuffer[count++]=x;
							decompressionbuffer[count++]=x;
							decompressionbuffer[count++]=x;
							decompressionbuffer[count++]=x;
							continue;
						}
						uchar m;
						if (n==7)
							m=getbit(compressedbuffer,&pos,&bitoffset)+1;
						else
							m=n+2;
						for (uchar b=0;b<4;b++){
							if (m==8)
								x=getbits(compressedbuffer,8,&pos,&bitoffset);
							else{
								ushort k=getbits(compressedbuffer,m,&pos,&bitoffset);
								if (k&1)
									x+=(k>>1)+1;
								else
									x-=k>>1;
							}
							decompressionbuffer[count++]=x;
						}
					}
					uchar *pbuf=buf+(width*3+width_pad)*(height-1)+a;
					uchar *psbuf=decompressionbuffer;
					for (ulong b=0;b<height;b++){
						if (b&1){
							for (ulong c=0;c<width;c++,pbuf-=3)
								*pbuf=*psbuf++;
							pbuf-=width*3+width_pad-3;
						}else{
							for (ulong c=0;c<width;c++,pbuf+=3)
								*pbuf=*psbuf++;
							pbuf-=width*3+width_pad+3;
						}
					}
					long b=0;
					for (long y0=height-1;y0>=0;y0--){
						if (y0%2){
							for (long x0=0;x0<width;x0++)
								buf[a+x0*3+y0*(width*3+width_pad)]=decompressionbuffer[b++];
						}else{
							for (long x0=width-1;x0>=0;x0--)
								buf[a+x0*3+y0*(width*3+width_pad)]=decompressionbuffer[b++];
						}
					}
				}
				delete[] decompressionbuffer;
			}
			break;
	}
	if (!res){
		buffersize=0;
		return 0;
	}
	buffersize=len;
	return res;
}

uchar *NONS_Archive::getFileBuffer(const std::wstring &filepath,ulong &buffersize){
	if (!this->loaded){
		buffersize=0;
		return 0;
	}
	NONS_TreeNode *node=this->root->getBranch(filepath,0);
	if (!node)
		return 0;
	return this->getFileBuffer(node,buffersize);
}

bool NONS_Archive::exists(const std::wstring &filepath){
	return this->loaded && this->root->getBranch(filepath,0)!=0;
}
#endif