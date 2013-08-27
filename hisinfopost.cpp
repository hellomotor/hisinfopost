// hisinfopost.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "hisinfopost.h"
#include <tchar.h>
#include <wininet.h>
#include <vector>
#include "resource.h"
#include "url_parse.h"
#include "browser_event.h"

#include <atlbase.h>
CComModule _Module;
#include <atlwin.h>
#include <windows.h>
#pragma comment(lib,"atl")
#pragma comment(lib,"User32.lib")

HINSTANCE hInstance;

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "ws2_32.lib")

CMyWebBrwoser2Event browserEventListener;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

BOOL CALLBACK DlgProc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam);

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			hInstance = (HINSTANCE)hModule;
			GUID guid;
			_Module.Init(ObjectMap, hInstance, &guid);
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#define ERROR_INTERNET			(1)
#define ERROR_URL_INVALID		(2)
#define ERROR_CONNECT			(3)
#define ERROR_OPEN_REQUEST		(4)
#define ERROR_SEND_REQUEST		(5)
#define ERROR_POST_FAILED		(6)

int httpPost(url_schema *urls, const char *headers, const char *data)
{
	int rc = 0;

	static const TCHAR agent[] = _T("Google Chrome");
	static LPCTSTR proxy = NULL;
	static LPCTSTR proxy_bypass = NULL;

	HINTERNET hSession = InternetOpen(agent, PRE_CONFIG_INTERNET_ACCESS, proxy, proxy_bypass, 0);
	if (hSession) {
		HINTERNET hConnect = InternetConnect(hSession, urls->host, urls->port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
		if (hConnect) {
			PCTSTR accept[] = {"*/*", NULL};
			HINTERNET hRequest = HttpOpenRequest(hConnect, _T("POST"), urls->page, NULL, NULL, accept, 0, 1);
			if (hRequest) {
				if (!HttpSendRequest(hRequest, headers, lstrlen(headers), (LPVOID)data, lstrlen(data)))
					rc = ERROR_SEND_REQUEST;
				else
					rc = ERROR_SUCCESS;
				InternetCloseHandle(hRequest);
			}
			else
				rc = ERROR_OPEN_REQUEST;
			InternetCloseHandle(hConnect);
		}
		else
			rc = ERROR_CONNECT;
		InternetCloseHandle(hSession);
	}
	else 
		rc = ERROR_INTERNET;
	return rc;
}

HISINFOPOST_API int postPres(const char *url, const char *xml, int post_type, const char *encoding)
{
	int rc = 0;
	url_schema *urls = url_parse(url);
	if (!urls) 
		return ERROR_URL_INVALID;
	HWND hParent = ::GetForegroundWindow();
	rc = DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hParent, DlgProc);
	if (rc == ACTION_COMMIT) {
		int length = strlen(xml) + 100;
		std::vector<char> data;
		data.reserve(length);
		char *p = &data[0];
		length = sprintf(p, "post_type=%d&encoding=%s&data=%s", post_type, encoding, xml);
		p[length] = '\0';
		static TCHAR hdrs[] = _T("Content-Type: application/xml");
		if (ERROR_SUCCESS != httpPost(urls, hdrs, p))
			rc = ERROR_POST_FAILED;
	}

	url_free(urls);
	return rc;
}

HISINFOPOST_API BOOL confirmPres(const char *url, const char *pres_no, int post_type)
{
	url_schema *urls = url_parse(url);
	if (!urls) 
		return FALSE;
	char data[100] = { 0 };
	sprintf(data, "pres_no=%s&post_type=%d", pres_no, post_type);
	static TCHAR hdrs[] = _T("Content-Type: text/html");
	if (ERROR_SUCCESS != httpPost(urls, hdrs, data))
		return FALSE;

	return TRUE;
}

BOOL CALLBACK DlgProc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	IWebBrowser2* iWebBrowser;
	VARIANT varMyURL;
	CAxWindow WinContainer;
	LPOLESTR pszName = OLESTR("shell.Explorer.2");
	RECT rc;
	BOOL result = FALSE;


	switch(Msg)
	{
	case WM_INITDIALOG:
		browserEventListener.setUnloaded();
		GetClientRect(hwnd, &rc);
		WinContainer.Create(hwnd, rc, 0,WS_CHILD |WS_VISIBLE);
		WinContainer.CreateControl(pszName);
		WinContainer.QueryControl(__uuidof(IWebBrowser2),(void**)&iWebBrowser); 
		VariantInit(&varMyURL);
		varMyURL.vt = VT_BSTR; 
		varMyURL.bstrVal = SysAllocString(OLESTR("about:blank"));
		browserEventListener.Advise(hwnd, iWebBrowser);
		iWebBrowser-> Navigate2(&varMyURL,0,0,0,0);

		VariantClear(&varMyURL);
		iWebBrowser-> Release(); 
		result = TRUE;
		break;
	case WM_HTML_ACTION:
		switch (wParam)
		{
		case ACTION_COMMIT:
			EndDialog(hwnd, ACTION_COMMIT);
			result = TRUE;
		case ACTION_RETURN:
			EndDialog(hwnd, ACTION_RETURN);
			result = TRUE;
		}
		break;
	case WM_COMMAND:
		switch(wParam)
		{
		case IDCANCEL:
			EndDialog(hwnd, ACTION_RETURN);
			result = TRUE;
		}
		break;
	}

	return result;
}