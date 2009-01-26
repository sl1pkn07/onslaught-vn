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

#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#include <iostream>
#include <string>

long getUTF8size(const wchar_t *string){
	if (!string)
		return 0;
	long res=0;
	for (;*string;string++){
		if (*string<0x80)
			res++;
		else if (*string<0x800)
			res+=2;
		else
			res+=3;
	}
	return res;
}

char *WChar_to_UTF8(const wchar_t *string){
	long fSize=getUTF8size(string);
	char *res=new char[fSize+1];
	res[fSize]=0;
	if (!string)
		return res;
	long b=0;
	for (;*string;string++,b++){
		if (*string<0x80)
			res[b]=(char)*string;
		else if (*string<0x800){
			res[b++]=(*string>>6)|192;
			res[b]=*string&63|128;
		}else{
			res[b++]=(*string>>12)|224;
			res[b++]=((*string&4095)>>6)|128;
			res[b]=*string&63|128;
		}
	}
	return res;
}

void printWChar(std::ostream &stream,const wchar_t *str){
	char *temp=WChar_to_UTF8(str);
	stream <<temp;
	delete[] temp;
}

wchar_t *copyWString(const wchar_t *str,unsigned long len=0);
wchar_t *copyWString(const char *str,unsigned long len=0);
char *copyString(const wchar_t *str,unsigned long len=0);
char *copyString(const char *str,unsigned long len=0);

#define _32KB 32768

int main(int argc,char **argv){
	if (argc<2)
		return 0;
	for (argv++;*argv;argv++){
		std::cout <<"Trying to add \""<<*argv<<"\""<<std::endl;
		wchar_t *buffer=new wchar_t[_32KB];
		wchar_t *temp=copyWString(*argv);
		ExpandEnvironmentStrings(temp,buffer,_32KB);
		delete[] temp;
		std::wstring path(buffer);
		delete[] buffer;
		std::cout <<"Expanded string: \"";
		printWChar(std::cout,path.c_str());
		std::cout <<"\""<<std::endl;
		
		bool found=1;
		HKEY k;
		if (RegOpenKeyEx(HKEY_CURRENT_USER,TEXT("Environment"),0,KEY_READ,&k)!=ERROR_SUCCESS){
			std::cout <<"Unable to open HKEY_CURRENT_USER\\Environment for reading."<<std::endl;
			return 0;
		}
		DWORD type,size;
		if (RegQueryValueEx(k,TEXT("PATH"),0,&type,0,&size)!=ERROR_SUCCESS){
			if (type!=REG_SZ){
				std::cout <<"HKEY_CURRENT_USER\\Environment\\PATH found but is a different type."<<std::endl;
				return 0;
			}
			found=0;
		}
		if (found){
			buffer=new wchar_t[size/2];
			RegQueryValueEx(k,TEXT("PATH"),0,&type,(LPBYTE)buffer,&size);
			RegCloseKey(k);
			std::wstring PATH(buffer);
			delete[] buffer;
			if (PATH.find(path)==PATH.npos){
				PATH.push_back(';');
				PATH.append(path);
				if (RegOpenKeyEx(HKEY_CURRENT_USER,TEXT("Environment"),0,KEY_SET_VALUE,&k)!=ERROR_SUCCESS){
					std::cout <<"Unable to open HKEY_CURRENT_USER\\Environment for writing."<<std::endl;
					return 0;
				}
				found=(RegSetValueEx(k,TEXT("PATH"),0,REG_SZ,(LPBYTE)PATH.c_str(),PATH.size()*sizeof(wchar_t))==ERROR_SUCCESS);
				if (!found)
					std::cout <<"Failed to set PATH."<<std::endl;
			}else{
				std::cout <<"The path was already added once to PATH."<<std::endl;
				found=0;
				DWORD res;
				SendMessageTimeout(HWND_BROADCAST,WM_SETTINGCHANGE,0,(LPARAM)TEXT("Environment"),0,0,&res);
			}
			RegCloseKey(k);
		}else{
			RegCloseKey(k);
			if (RegOpenKeyEx(HKEY_CURRENT_USER,TEXT("Environment"),0,KEY_SET_VALUE,&k)!=ERROR_SUCCESS){
				std::cout <<"Unable to open HKEY_CURRENT_USER\\Environment for writing."<<std::endl;
				return 0;
			}
			found=(RegSetValueEx(k,TEXT("PATH"),0,REG_SZ,(LPBYTE)path.c_str(),path.size()*sizeof(wchar_t))==ERROR_SUCCESS);
			if (!found)
				std::cout <<"Failed to create PATH."<<std::endl;
			RegCloseKey(k);
		}
		if (found){
			SendMessageTimeout(HWND_BROADCAST,WM_SETTINGCHANGE,0,(LPARAM)TEXT("Environment"),0,1000,0);
			std::cout <<"The PATH environment variable was correctly set, but the change will only\n"
				"apply to new cmd.exe sessions."<<std::endl;
		}
	}
	return 0;
}

template<typename dst,typename src>
dst *copyString_template(const src *str,unsigned long len){
	if (!str)
		return 0;
	if (!len)
		for (;str[len];len++);
	dst *res=new dst[len+1];
	res[len]=0;
	for (unsigned long a=0;a<len;a++)
		res[a]=str[a];
	return res;
}

wchar_t *copyWString(const wchar_t *str,unsigned long len){
	return copyString_template<wchar_t,wchar_t>(str,len);
}

wchar_t *copyWString(const char *str,unsigned long len){
	return copyString_template<wchar_t,char>(str,len);
}

char *copyString(const wchar_t *str,unsigned long len){
	return copyString_template<char,wchar_t>(str,len);
}

char *copyString(const char *str,unsigned long len){
	return copyString_template<char,char>(str,len);
}
