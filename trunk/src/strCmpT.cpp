#include "strCmpT.h"
#include <cstring>
#include <cwchar>

bool strCmp::operator()(const char *a,const char *b) const{
	return strcmp(a,b)<0;
}

bool wstrCmp::operator()(const wchar_t *s1,const wchar_t *s2) const{
	return wcscmp(s1,s2)<0;
}

template <typename T>
int strcmpCI(T *a,T *b){
    for (;*a || *b;a++,b++){
		char c=(*a>='A' && *a<='Z')?*a|32:*a,
			d=(*b>='A' && *b<='Z')?*b|32:*b;
		if (c<d)
            return -1;
        if (c>d)
            return 1;
    }
	return 0;
}

bool strCmpCI::operator()(const char *a,const char *b) const{
	return strcmpCI(a,b)<0;
}

bool wstrCmpCI::operator()(const wchar_t *s1,const wchar_t *s2) const{
	return strcmpCI(s1,s2)<0;
}
