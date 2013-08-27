// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

typedef int (*FUN_POSTPRES)(const char *, const char *, int , const char *);
typedef BOOL(*FUN_COMFIRMPRES)(const char *, const char *, int);

int _tmain(int argc, _TCHAR* argv[])
{
	const char *url = "http://192.168.2.4:8081/index";
	const char *frmdata = "<xml version=\"1.0\">"
		"<appinfo>"
		"</appinfo>";
	HMODULE hMod = LoadLibrary(_T("..\\..\\Debug\\hisinfopost.dll"));
	if (!hMod) {
		hMod = LoadLibrary(_T("..\\Debug\\hisinfopost.dll"));
	}

	FUN_POSTPRES postPres = (FUN_POSTPRES)GetProcAddress(hMod, "postPres");
	FUN_COMFIRMPRES confirmPres = (FUN_COMFIRMPRES)GetProcAddress(hMod, "confirmPres");
	if (postPres) {
		int rc = postPres(url, frmdata, 0, "GBK");
		if (rc == 8 || rc == 9) 
			confirmPres(url, "presNo", 1);
	}
	FreeLibrary(hMod);

	return 0;
}

