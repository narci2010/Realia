#include "RichEditUtil.h"
#include <RichOle.h>

static DWORD CALLBACK streamInCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	ReadFile((HANDLE)dwCookie, pbBuff, cb, (LPDWORD)pcb, NULL);//¶ÁÈ¡Êý¾Ý
	return 0;
}

BOOL ReadFileByStreamIn(CRichEditUI* hRichEdit, LPCTSTR pszFileName)
{
	HRESULT hr;

	HANDLE hFile = CreateFile(pszFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	EDITSTREAM es;
	es.dwCookie = (DWORD_PTR)hFile;
	es.pfnCallback = streamInCallback;

	hRichEdit->SetSel(0, -1);
	DWORD dwStart, dwEnd;
	hRichEdit->GetSel((long&)dwStart, (long&)dwEnd);
	hRichEdit->SetSel(dwEnd + 1, dwEnd + 1);
	//hRichEdit->ReplaceSel(L"\n", TRUE);

	CDuiString strFileName(pszFileName);
	CDuiString strExt = strFileName.Right(4);
	if (strExt == _T(".txt"))
		hr = hRichEdit->StreamIn(SF_TEXT | SFF_SELECTION, es);
	else if (strExt == _T(".rtf"))
		hr = hRichEdit->StreamIn(SF_RTF | SFF_SELECTION, es);

	CloseHandle(hFile);
	if (FAILED(hr))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL InsertObject(CRichEditUI* hRichEdit, LPCTSTR pszFileName)
{
	HRESULT hr;

	//1.Get the IRichEditOle interface.
	LPRICHEDITOLE pRichEditOle;
	LRESULT lResult;
	hRichEdit->TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)(LPVOID*)&pRichEditOle, &lResult);
	if (pRichEditOle == NULL)
	{
		return FALSE;
	}

	//2.Create structured storage.
	LPLOCKBYTES pLockBytes = NULL;
	hr = CreateILockBytesOnHGlobal(NULL, TRUE, &pLockBytes);
	if (FAILED(hr))
	{
		return FALSE;
	}

	LPSTORAGE pStorage;
	hr = StgCreateDocfileOnILockBytes(pLockBytes,
		STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE,
		0, &pStorage);
	if (FAILED(hr))
	{
		return FALSE;
	}

	//3.Set up the data format.
	FORMATETC formatEtc;
	formatEtc.cfFormat = 0;
	formatEtc.ptd = NULL;
	formatEtc.dwAspect = DVASPECT_CONTENT;
	formatEtc.lindex = -1;
	formatEtc.tymed = TYMED_NULL;

	//4.Get a pointer to the display site.
	LPOLECLIENTSITE	pClientSite;
	hr = pRichEditOle->GetClientSite(&pClientSite);
	if (FAILED(hr))
	{
		return FALSE;
	}

	//5.Create the object and retrieve its IUnknown interface.
	LPUNKNOWN pUnk;
	CLSID clsid = CLSID_NULL;
	hr = OleCreateFromFile(clsid,
		pszFileName,
		IID_IUnknown,
		OLERENDER_DRAW,
		&formatEtc,
		pClientSite,
		pStorage,
		(void**)&pUnk);

	pClientSite->Release();
	if (FAILED(hr))
	{
		return FALSE;
	}

	//6.Get the IOleObject interface to the object.
	LPOLEOBJECT pObject;
	hr = pUnk->QueryInterface(IID_IOleObject, (void**)&pObject);
	pUnk->Release();
	if (FAILED(hr))
	{
		return FALSE;
	}

	//7.To ensure that references are counted correctly, notify the object that it is contained.
	OleSetContainedObject(pObject, TRUE);

	//8.Set up object info.
	REOBJECT reobject = { sizeof(REOBJECT) };

	hr = pObject->GetUserClassID(&clsid);
	if (FAILED(hr))
	{
		return FALSE;
	}

	reobject.clsid = clsid;
	reobject.cp = REO_CP_SELECTION;
	reobject.dvaspect = DVASPECT_CONTENT;
	reobject.dwFlags = REO_NULL;
	reobject.dwUser = 0;
	reobject.poleobj = pObject;
	reobject.polesite = pClientSite;
	reobject.pstg = pStorage;

	SIZEL sizel = { 0 };
	reobject.sizel = sizel;

	//9.Move the caret to the end of the text and add a carriage return.
	hRichEdit->SetSel(0, -1);
	DWORD dwStart, dwEnd;
	hRichEdit->GetSel((long&)dwStart, (long&)dwEnd);
	hRichEdit->SetSel(dwEnd + 1, dwEnd + 1);
	hRichEdit->ReplaceSel(L"\n", TRUE);

	//10.Insert the object.
	hr = pRichEditOle->InsertObject(&reobject);

	//11.Clean up.
	pObject->Release();
	pRichEditOle->Release();
	if (FAILED(hr))
	{
		return FALSE;
	}

	return TRUE;
}

