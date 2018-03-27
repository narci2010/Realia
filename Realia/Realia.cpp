#include "stdafx.h"
#include "Realia.h"

CRealiaWnd::CRealiaWnd()
{

}

CRealiaWnd::~CRealiaWnd()
{

}

void CRealiaWnd::InitWindow(HWND hWnd)
{
	m_hWnd = hWnd;

	GetWindowRect(m_hWnd, &m_rcWindow);
	GetClientRect(m_hWnd, &m_rcWindow);
	m_lWndWidth = m_rcWindow.right - m_rcWindow.left;
	m_lWndHeight = m_rcWindow.bottom - m_rcWindow.top;
}

void CRealiaWnd::OnPaint(HDC pDc)
{
	//创建与windowDC兼容的内存设备环境  
	HDC hDcMem = CreateCompatibleDC(pDc);
	//位图的初始化和载入位图     
	HBITMAP hBmpMem = CreateCompatibleBitmap(pDc, m_lWndWidth, m_lWndHeight);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hDcMem, hBmpMem);

	//绘制背景
	DrawBackground(hDcMem, m_rcWindow);

	//画直尺
	POINT pt1 = { m_rcWindow.left + 100, m_rcWindow.top + 100 };
	POINT pt2 = { m_rcWindow.left + 300, m_rcWindow.top + 400 };
	DrawRuler(hDcMem, pt1, pt2);

	//双缓冲技术
	BitBlt(pDc, 0, 0, m_lWndWidth, m_lWndHeight, hDcMem, 0, 0, SRCCOPY);
	SelectObject(hDcMem, hBmpOld);
	//释放资源
	DeleteDC(hDcMem);
	DeleteObject(hBmpMem);
	DeleteObject(hBmpOld);
}

void CRealiaWnd::DrawBackground(HDC dc, RECT rc)
{
	HBRUSH hbrush = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH oldhbrush = (HBRUSH)SelectObject(dc, hbrush);

	FillRect(dc, &rc, hbrush);

	SelectObject(dc, oldhbrush);
	::DeleteObject(hbrush);
	::DeleteObject(oldhbrush);
}

//画直尺：5像素为1毫米,固定直尺宽度60像素
void CRealiaWnd::DrawRuler(HDC dc, POINT pt1, POINT pt2)
{
	HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	HPEN oldpen = (HPEN)SelectObject(dc, pen);

	HBRUSH hbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldbrush = (HBRUSH)SelectObject(dc, hbrush);

	//Rectangle(dc, pt1.x, pt1.y - 1, pt2.x, pt2.y + 60);
	POINT pt3, pt4;
	double px = pt2.x - pt1.x;
	double py = pt2.y - pt1.y;
	double pl = sqrt(px * px + py * py);
	double theta = atan(py / px);
	pt3.x = pt2.x - 60 * sin(theta);
	pt3.y = pt2.y + 60 * cos(theta);
	pt4.x = pt1.x - 60 * sin(theta);
	pt4.y = pt1.y + 60 * cos(theta);
	MoveToEx(dc, pt1.x, pt1.y - 1, NULL);
	LineTo(dc, pt2.x, pt2.y - 1);
	LineTo(dc, pt3.x, pt3.y);
	LineTo(dc, pt4.x, pt4.y);
	LineTo(dc, pt1.x, pt1.y - 1);

	pen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	SelectObject(dc, pen);

	//12号微软雅黑字体
	HFONT hFont = CreateFont(
		-12,                       // nHeight  
		0,                         // nWidth  
		0,                         // nEscapement  
		0,                         // nOrientation  
		FW_NORMAL,                 // nWeight  
		FALSE,                     // bItalic  
		FALSE,                     // bUnderline  
		0,                         // cStrikeOut  
		ANSI_CHARSET,              // nCharSet  
		OUT_DEFAULT_PRECIS,        // nOutPrecision  
		CLIP_DEFAULT_PRECIS,       // nClipPrecision  
		DEFAULT_QUALITY,           // nQuality  
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily  
		_T("微软雅黑"));           // lpszFacename 
	SetTextColor(dc, RGB(0, 0, 255));
	HFONT oldfont = (HFONT)SelectObject(dc, hFont);

	int count = 0;
	for (int i = pt1.x + 5; i < pt2.x; i += 5) {
		MoveToEx(dc, i, pt1.y, NULL);
		if (count % 10 == 0) {
			LineTo(dc, i, pt1.y + 25);
			TCHAR mark[8];
			wsprintf(mark, _T("%d"), count / 10);
			TextOut(dc, i - 3, pt1.y + 27, mark, lstrlen(mark));
		}
		else if (count % 5 == 0) {
			LineTo(dc, i, pt1.y + 20);
		}
		else {
			LineTo(dc, i, pt1.y + 10);
		}
		count++;
	}

	SelectObject(dc, oldfont);
	::DeleteObject(oldfont);
	::DeleteObject(hFont);
	SelectObject(dc, oldbrush);
	::DeleteObject(hbrush);
	::DeleteObject(oldbrush);
	SelectObject(dc, oldpen);
	::DeleteObject(pen);
	::DeleteObject(oldpen);
}
