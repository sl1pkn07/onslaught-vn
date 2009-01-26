#ifndef STRCMPT_H
#define STRCMPT_H
struct strCmp{
	bool operator()(const char *a,const char *b) const;
};

struct wstrCmp{
	bool operator()(const wchar_t *s1,const wchar_t *s2) const;
};

//Case-insensitive versions.

struct strCmpCI{
	bool operator()(const char *a,const char *b) const;
};

struct wstrCmpCI{
	bool operator()(const wchar_t *s1,const wchar_t *s2) const;
};
#endif
