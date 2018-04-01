#include "stdafx.h"
#include "RealiaWnd.h"

CRealiaWnd::CRealiaWnd()
{
	::ZeroMemory(&m_rcWindow, sizeof(m_rcWindow));
	m_lWndWidth = 0;
	m_lWndHeight = 0;
	m_vecRealias.reserve(10);
}

CRealiaWnd::~CRealiaWnd()
{
	m_vecRealias.clear();
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

	//模仿橡皮筋类画图
	for (int i = 0; i < m_vecRealias.size(); i++)
		m_vecRealias.at(i).Draw(hDcMem);

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
	for (int i = 0; i < m_vecRealias.size(); i++) {
		if (!m_vecRealias.at(i).IsRegionNull()) {
			int nHitTest = m_vecRealias.at(i).HitTest(pt);
			if (nHitTest >= 0 && nHitTest <= 9) {
				m_vecRealias.at(i).Track(m_hWnd, pt);
				return;
			}
		}
	}

	if (m_iRealiaType >= 1 && m_iRealiaType <= 6) {
		CRealia pRealia(pt, pt, m_iRealiaType);
		m_vecRealias.push_back(pRealia);
		m_vecRealias.at(m_vecRealias.size() - 1).TrackRubberBand(m_hWnd, pt, true);
	}
	return;
}
void CRealiaWnd::OnLButtonUp(POINT pt)
{

}

void CRealiaWnd::OnMouseMove(POINT pt)
{

}

BOOL CRealiaWnd::OnSetCursor(HWND pWnd, UINT nHitTest)
{
	for (int i = 0; i < m_vecRealias.size(); i++) {
		if (!m_vecRealias.at(i).IsRegionNull()) {
			if (m_vecRealias.at(i).SetCursor(m_hWnd, nHitTest))
			{
				return FALSE;
			}
		}
	}
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	return true;
}

void CRealiaWnd::SetRealiaType(int nType)
{
	m_iRealiaType = nType;
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
