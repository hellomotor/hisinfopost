#ifndef BROWSER_WINDOW_H
#define BROWSER_WNNDOW_H

#include <MSHTML.h>
#include <Exdisp.h>
#include <exdispid.h>
#include <comutil.h>
#include <strsafe.h>

#pragma comment(lib, "comsupp.lib")

static TCHAR szHTMLText[] = "<html>"
"<head>"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gbk\">"
"<style type=\"text/css\">"
"body { text-align:center; background-color: #ccc; }"
"</style>"
"</head>"
"<body>"
"<div>"
"<input type=button value=\"修改处方\" onclick='location.href(\"return\")'></input> . <input type=button value=\" 确 认 \" onclick='location.href(\"commit\")'></input>"
"</div>"
"</body>"
"</html>";

#define WM_HTML_ACTION (WM_USER + 100)
#define ACTION_RETURN (8)
#define ACTION_COMMIT (9)

class CMyWebBrwoser2Event : public DWebBrowserEvents2
{
	DWORD dwCookie;
	IWebBrowser2* webBrowser;
	BOOL hasLoaded;
	HWND hWnd;
public: // IDispatch methods

	CMyWebBrwoser2Event() : dwCookie(0), webBrowser(NULL), hasLoaded(FALSE)
	{
	}

	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
	{
		HRESULT hr = E_NOINTERFACE;
		if (riid == __uuidof(IDispatch))
		{
			*ppvObject = (IDispatch*)this;
			AddRef();
			hr = S_OK;
		} 
		else if (riid == __uuidof(DWebBrowserEvents2))
		{
			*ppvObject = (DWebBrowserEvents2*)this;
			AddRef();
			hr = S_OK;			
		}

		return hr;
	}

	STDMETHODIMP_(ULONG) AddRef(void)
	{
		return 1;
	}

	STDMETHODIMP_(ULONG) Release(void)
	{
		return 1;
	}

	STDMETHOD(GetTypeInfoCount)(UINT*) 
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetTypeInfo)(UINT, LCID, ITypeInfo**) 
	{
		return E_NOTIMPL; 
	}

	STDMETHOD(GetIDsOfNames)(REFIID, LPOLESTR *rgszNames, UINT, LCID, DISPID *rgDispId) 
	{ 
		return E_NOTIMPL;
	}

	STDMETHODIMP Invoke(DISPID dispidMember,
		REFIID riid, LCID lcid, 
		WORD wFlags,
		DISPPARAMS* dispParams,
		VARIANT* pvarResult,
		EXCEPINFO* pExcepInfo,
		UINT* puArgErr)  
	{
		if (!dispParams) {
			return E_INVALIDARG;
		}
		switch (dispidMember)
		{
		case DISPID_BEFORENAVIGATE2:
			{
				_bstr_t burl = dispParams->rgvarg[5].pvarVal->bstrVal;

				if (lstrcmp(burl, _T("about:return")) == 0) {
					*((*dispParams).rgvarg)[0].pboolVal = VARIANT_TRUE;
					SendMessage(hWnd, WM_HTML_ACTION, ACTION_RETURN, 0);
				}
				else if (lstrcmp(burl, _T("about:commit")) == 0) {
					*((*dispParams).rgvarg)[0].pboolVal = VARIANT_TRUE;
					SendMessage(hWnd, WM_HTML_ACTION, ACTION_COMMIT, 0);
				}
				else
					OutputDebugString(burl);
			}
			break;
		case DISPID_DOCUMENTCOMPLETE:
			{
				if (!hasLoaded) {
					//LPCWSTR pszURL = dispParams->rgvarg[0].pvarVal->bstrVal;
					LPDISPATCH pDisp = (LPDISPATCH)dispParams->rgvarg[dispParams->cArgs - 1].byref;
					LoadHTMLString(pDisp, szHTMLText, lstrlen(szHTMLText));
					hasLoaded = TRUE;
				}
			}
			break;
		default:
			break;
		}

		return S_OK;
	}

	STDMETHODIMP Advise(IUnknown* pUnkCP, const IID& iid)
	{
		if(pUnkCP == NULL) {
			return E_INVALIDARG;
		}

		IConnectionPointContainer* pCPC =NULL;
		HRESULT hr = pUnkCP->QueryInterface(IID_IConnectionPointContainer, reinterpret_cast<void**>(&pCPC));
		if (SUCCEEDED(hr)) {
			IConnectionPoint* pCP = NULL;
			hr = pCPC->FindConnectionPoint(iid, &pCP);
			if (SUCCEEDED(hr)) {
				hr = pCP->Advise(this, &dwCookie);
				pCP->Release();
			}
			pCPC->Release();    
		}
		return hr;
	}

public:
	STDMETHODIMP Unadvise(IUnknown* pUnkCP, const IID& iid)
	{
		if(pUnkCP == NULL) {
			return E_INVALIDARG;
		}

		IConnectionPointContainer* pCPC =NULL;
		HRESULT hr = pUnkCP->QueryInterface(IID_IConnectionPointContainer, reinterpret_cast<void**>(&pCPC));
		if (SUCCEEDED(hr)) {
			IConnectionPoint* pCP = NULL;
			hr = pCPC->FindConnectionPoint(iid, &pCP);
			if (SUCCEEDED(hr)) {
				hr = pCP->Unadvise(dwCookie);
				pCP->Release();
			}
			pCPC->Release();
		}
		return hr;
	}

	/**
	* Register DWebBrowserEvents2 to IWebBrowser2.
	*/
	STDMETHODIMP Advise(HWND hwnd, IWebBrowser2* pIE)
	{
		HRESULT hr = Advise(pIE, __uuidof(DWebBrowserEvents2));
		this->webBrowser = pIE;
		this->hWnd = hwnd;
		this->webBrowser->AddRef();
		return hr;
	}

	/*
	* Unregister DWebBrowserEvents2 to IWebBrowser2.
	*/
	STDMETHODIMP Unadvise()
	{
		HRESULT hr = Unadvise(this->webBrowser, __uuidof(DWebBrowserEvents2));
		this->webBrowser->Release();
		this->webBrowser = NULL;
		this->hWnd = NULL;
		return hr;
	}

protected:

	/*HRESULT LoadWebBrowserFromStream(IWebBrowser2* pWebBrowser,TCHAR* szHTMLText)
	{
	IUnknown* pUnkBrowser = NULL;    	 
	HRESULT hr;

	hr = pWebBrowser->QueryInterface(IID_IUnknown,(void**)&pUnkBrowser);    
	if (SUCCEEDED(hr))    
	{                       
	size_t ulLength = _tcslen(szHTMLText) * sizeof(TCHAR);                
	HGLOBAL hHTMLText = GlobalAlloc(GPTR,ulLength + sizeof(TCHAR));                    
	if (hHTMLText != NULL)               
	{                    
	_tcsncpy((TCHAR*)hHTMLText,szHTMLText,ulLength);

	IStream* pStream = NULL;  
	hr = CreateStreamOnHGlobal(hHTMLText,TRUE,&pStream);           
	if (SUCCEEDED(hr))                
	{     
	IHTMLDocument* pHtmlDoc = NULL;
	IPersistStreamInit* pPersistStreamInit = NULL;
	hr = pWebBrowser->get_Document((IDispatch**)&pHtmlDoc);    
	if (SUCCEEDED(hr))    
	{
	hr = pHtmlDoc->QueryInterface(IID_IPersistStreamInit,(void**)&pPersistStreamInit);        
	if (SUCCEEDED(hr))        
	{
	hr = pPersistStreamInit->InitNew();            
	if (SUCCEEDED(hr))            
	{
	hr = pPersistStreamInit->Load(pStream); 
	}            
	pPersistStreamInit->Release();       
	}  
	pHtmlDoc->Release();
	}
	pStream->Release();  
	}      
	}          
	pUnkBrowser->Release();  
	}

	return hr;
	}*/

	HRESULT LoadWebBrowserFromStream(IWebBrowser* pWebBrowser, IStream* pStream)
	{
		HRESULT hr;
		IDispatch* pHtmlDoc = NULL;
		IPersistStreamInit* pPersistStreamInit = NULL;

		// Retrieve the document object.
		hr = pWebBrowser->get_Document( &pHtmlDoc );
		if ( SUCCEEDED(hr) )
		{
			// Query for IPersistStreamInit.
			hr = pHtmlDoc->QueryInterface( IID_IPersistStreamInit,  (void**)&pPersistStreamInit );
			if ( SUCCEEDED(hr) )
			{
				// Initialize the document.
				hr = pPersistStreamInit->InitNew();
				if ( SUCCEEDED(hr) )
				{
					// Load the contents of the stream.
					hr = pPersistStreamInit->Load( pStream );
				}
				pPersistStreamInit->Release();
			}
			pHtmlDoc->Release();
		}
		return hr;
	}

	void LoadHTMLString(LPDISPATCH  pDisp, LPCTSTR html, DWORD length)
	{
		HRESULT hr;
		IUnknown* pUnkBrowser = NULL;
		IUnknown* pUnkDisp = NULL;
		IStream* pStream = NULL;
		HGLOBAL hHTMLText;


		// Is this the DocumentComplete event for the top frame window?
		// Check COM identity: compare IUnknown interface pointers.
		hr = webBrowser->QueryInterface( IID_IUnknown,  (void**)&pUnkBrowser );
		if ( SUCCEEDED(hr) )
		{
			hr = pDisp->QueryInterface( IID_IUnknown,  (void**)&pUnkDisp );
			if ( SUCCEEDED(hr) )
			{
				if ( pUnkBrowser == pUnkDisp )
				{   // This is the DocumentComplete event for the top 
					//   frame - page is loaded!
					// Create a stream containing the HTML.
					// Alternatively, this stream may have been passed to us.

					//  TODO: Safely determine the length of szHTMLText in TCHAR.
					hHTMLText = GlobalAlloc( GPTR, length+1 );

					if ( hHTMLText )
					{
						_tcsncpy((TCHAR*)hHTMLText, szHTMLText, length);
						//  TODO: Add error handling code here.

						hr = CreateStreamOnHGlobal( hHTMLText, FALSE, &pStream );
						if ( SUCCEEDED(hr) )
						{
							// Call the helper function to load the browser from the stream.
							LoadWebBrowserFromStream( webBrowser, pStream  );
							pStream->Release();
						}
						GlobalFree( hHTMLText );
					}
				}
				pUnkDisp->Release();
			}
			pUnkBrowser->Release();
		}


	}

};

#endif