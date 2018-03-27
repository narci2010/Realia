#include "stdafx.h"
#include "Realia.h"

CRealiaWnd::CRealiaWnd()
{
	::ZeroMemory(&m_rcWindow, sizeof(m_rcWindow));
	m_lWndWidth = 0;
	m_lWndHeight = 0;

	m_bIsLButtonDown = false;
	m_ptBegin.x = m_ptBegin.y = 0;
	m_ptEnd.x = m_ptEnd.y = 0;
}

CRealiaWnd::~CRealiaWnd()
{

}

void CRealiaWnd::InitWindow(HWND hWnd)
{
	m_hWnd = hWnd;

	//GetWindowRect(m_hWnd, &m_rcWindow);
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
	if (m_bIsLButtonDown) {
		DrawRuler(hDcMem, m_ptBegin, m_ptEnd);
	}

	//双缓冲技术
	BitBlt(pDc, 0, 0, m_lWndWidth, m_lWndHeight, hDcMem, 0, 0, SRCCOPY);
	SelectObject(hDcMem, hBmpOld);
	//释放资源
	DeleteDC(hDcMem);
	DeleteObject(hBmpMem);
	DeleteObject(hBmpOld);
}

void CRealiaWnd::OnLButtonDown(POINT pt)
{
	m_ptBegin = pt;
	m_bIsLButtonDown = true;
}
void CRealiaWnd::OnLButtonUp(POINT pt)
{
	m_bIsLButtonDown = false;
}
void CRealiaWnd::OnMouseMove(POINT pt)
{
	if (m_bIsLButtonDown) {
		m_ptEnd = pt;
		InvalidateRect(m_hWnd, NULL, false);
		UpdateWindow(m_hWnd);
	}
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

//画直尺：5像素为1毫米，固定直尺总宽度为60像素
//普通刻度10像素长，5的倍数20像素长，10的倍数25像素长
void CRealiaWnd::DrawRuler(HDC dc, POINT pt1, POINT pt2)
{
	if (pt1.x > pt2.x) {
		LONG tmp = pt1.x;
		pt1.x = pt2.x;
		pt2.x = tmp;
		tmp = pt1.y;
		pt1.y = pt2.y;
		pt2.y = tmp;
	}

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

	int count = pl / 5;
	int j = 0;
	for (int i = 0; i < count; i++) {
		j = i + 1;//为了下面的计算少写一个括号
		MoveToEx(dc, pt1.x + 5 * j * cos(theta), pt1.y + 5 * j * sin(theta), NULL);
		if (i % 10 == 0) {
			LineTo(dc, pt1.x + 5 * j * cos(theta) - 25 * sin(theta), pt1.y + 5 * j * sin(theta) + 25 * cos(theta));
			TCHAR mark[8];
			wsprintf(mark, _T("%d"), i / 10);
			TextOut(dc, pt1.x + 5 * j * cos(theta) - 25 * sin(theta) - 3, pt1.y + 5 * j * sin(theta) + 25 * cos(theta) + 2, mark, lstrlen(mark));
		}
		else if (i % 5 == 0) {
			LineTo(dc, pt1.x + 5 * j * cos(theta) - 20 * sin(theta), pt1.y + 5 * j * sin(theta) + 20 * cos(theta));
		}
		else {
			LineTo(dc, pt1.x + 5 * j * cos(theta) - 10 * sin(theta), pt1.y + 5 * j * sin(theta) + 10 * cos(theta));
		}
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
