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
	//ʹ��GDI��ʽ��
	//������windowDC���ݵ��ڴ��豸����  
	HDC hDcMem = CreateCompatibleDC(pDc);
	//λͼ�ĳ�ʼ��������λͼ     
	HBITMAP hBmpMem = CreateCompatibleBitmap(pDc, m_lWndWidth, m_lWndHeight);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hDcMem, hBmpMem);

	//���Ʊ���
	DrawBackground(hDcMem, m_rcWindow);


	//ʹ��GDI+��ʽȥ���
	Graphics graphics(hDcMem);

	//���ڴ��н���һ�顰���⻭����
	Bitmap memBmp(m_lWndWidth, m_lWndHeight);
	//��ȡ����ڴ滭����Graphics����
	Graphics memGr(&memBmp);
	//������Ϊ������ƽ��ģʽ
	memGr.SetSmoothingMode(SmoothingModeHighQuality);

	//ģ����Ƥ���໭ͼ
	for (int i = 0; i < m_vecRealias.size(); i++)
		m_vecRealias.at(i).Draw(&memGr);

	//���ڴ滭������������
	graphics.DrawImage(&memBmp, 0, 0, m_lWndWidth, m_lWndHeight);


	//˫���弼��
	BitBlt(pDc, 0, 0, m_lWndWidth, m_lWndHeight, hDcMem, 0, 0, SRCCOPY);
	SelectObject(hDcMem, hBmpOld);
	//�ͷ���Դ
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
			//�˴�erase�������⣬�����ǰ����һ�������m_rgn���ͷŵ������Ը�Ϊ�����ֶ��ͷţ����������������ͷ�
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
