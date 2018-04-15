
#include "Util.h"

DWORD DistanceBetweenPointAndRect(RECT rc, POINT pt)
{
	if (PtInRect(&rc, pt))
		return 0;
	else if (pt.y < rc.top || pt.y > rc.bottom)
		return -1;
	else {
		DWORD mx = (rc.left + rc.right) / 2;
		if (pt.x < mx)
			return mx - pt.x;
		else
			return pt.x - mx;
	}
}


int CDuiStringToInt(CDuiString str)
{
	//CDuiString -> LPCTSTR -> char*
	LPCTSTR lpstr = str.GetData();
	int num = WideCharToMultiByte(CP_OEMCP, NULL, lpstr, -1, NULL, 0, NULL, FALSE);
	char *pchar = new char[num];
	WideCharToMultiByte(CP_OEMCP, NULL, lpstr, -1, pchar, num, NULL, FALSE);
	return atoi(pchar);
}

SIZE GetTextExtent(LPCTSTR lpszString, LPCTSTR lpszFontFamily, int iFontSize)
{
	HWND hWndDesktop = ::GetDesktopWindow();
	HDC hDcDesktop = GetDC(hWndDesktop);
	ASSERT(hDcDesktop != NULL);

	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	lstrcpyn(lf.lfFaceName, _T("Î¢ÈíÑÅºÚ"), LF_FACESIZE);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -iFontSize;
	HFONT hFont = ::CreateFontIndirect(&lf);
	ASSERT(hFont != NULL);

	HFONT hOldFont = (HFONT)SelectObject(hDcDesktop, hFont);

	SIZE size = { 0,0 };
	ASSERT(::GetTextExtentPoint32(hDcDesktop, lpszString, lstrlen(lpszString), &size));

	SelectObject(hDcDesktop, hOldFont);
	::ReleaseDC(hWndDesktop, hDcDesktop);
	::DeleteObject(hOldFont);
	::DeleteObject(hFont);

	return size;
}
