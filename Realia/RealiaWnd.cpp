#include "stdafx.h"
#include "RealiaWnd.h"

CRealiaWnd::CRealiaWnd()
{
	::ZeroMemory(&m_rcWindow, sizeof(m_rcWindow));
	m_lWndWidth = 0;
	m_lWndHeight = 0;
	m_vecRealias.reserve(30);
}

CRealiaWnd::~CRealiaWnd()
{
	for (std::vector<CRealia>::iterator it = m_vecRealias.begin(); it != m_vecRealias.end(); it++) {
		DeleteObject(it->m_rgn);
		DeleteObject(it->m_rgnTopRight);
	}
	m_vecRealias.clear();

	for (std::vector<CArc>::iterator it = m_vecArcs.begin(); it != m_vecArcs.end(); it++) {
		it->m_vecPoints.clear();
	}
	m_vecArcs.clear();
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
	//使用GDI方式：
	//创建与windowDC兼容的内存设备环境  
	HDC hDcMem = CreateCompatibleDC(pDc);
	//位图的初始化和载入位图     
	HBITMAP hBmpMem = CreateCompatibleBitmap(pDc, m_lWndWidth, m_lWndHeight);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hDcMem, hBmpMem);

	//绘制背景
	DrawBackground(hDcMem, m_rcWindow);


	//使用GDI+方式去锯齿
	Graphics graphics(hDcMem);

	//在内存中建立一块“虚拟画布”
	Bitmap memBmp(m_lWndWidth, m_lWndHeight);
	//获取这块内存画布的Graphics引用
	Graphics memGr(&memBmp);
	//设置线为高质量平滑模式
	memGr.SetSmoothingMode(SmoothingModeHighQuality);

	//模仿橡皮筋类画图
	for (int i = 0; i < m_vecRealias.size(); i++)
		m_vecRealias.at(i).Draw(&memGr);

	//将内存画布画到窗口中
	graphics.DrawImage(&memBmp, 0, 0, m_lWndWidth, m_lWndHeight);


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
		m_vecRealias.at(i).m_bSelect = FALSE;
	}

	for (int i = 0; i < m_vecRealias.size(); i++) {
		if (!m_vecRealias.at(i).IsRegionNull()) {
			int nHitTest = m_vecRealias.at(i).HitTest(pt);
			if (nHitTest >= 0 && nHitTest <= 9) {
				m_vecRealias.at(i).m_bSelect = TRUE;
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

UINT CRealiaWnd::DeleteRealia()
{
	UINT num = 0;
	//for (int i = 0; i < m_vecRealias.size(); i++) {
	//	if (m_vecRealias.at(i).m_bSelect == TRUE) {
	//		m_vecRealias.erase(m_vecRealias.begin() + i);
	//		InvalidateRect(m_hWnd, NULL, false);
	//		UpdateWindow(m_hWnd);
	//		++num;
	//		break;
	//	}
	//}
	for (std::vector<CRealia>::iterator it = m_vecRealias.begin(); it != m_vecRealias.end(); it++) {
		if (it->m_bSelect == TRUE) {
			//此处erase存在问题，它总是把最后一个对象的m_rgn给释放掉，所以改为这里手动释放，不在析构函数中释放
			DeleteObject(it->m_rgn);
			m_vecRealias.erase(it);
			InvalidateRect(m_hWnd, NULL, false);
			UpdateWindow(m_hWnd);
			++num;
			break;
		}
	}
	return num;
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
