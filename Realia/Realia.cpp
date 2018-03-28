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
	//������windowDC���ݵ��ڴ��豸����  
	HDC hDcMem = CreateCompatibleDC(pDc);
	//λͼ�ĳ�ʼ��������λͼ     
	HBITMAP hBmpMem = CreateCompatibleBitmap(pDc, m_lWndWidth, m_lWndHeight);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hDcMem, hBmpMem);

	//���Ʊ���
	DrawBackground(hDcMem, m_rcWindow);

	//��ֱ��
	if (m_bIsLButtonDown) {
		//DrawRuler(hDcMem, m_ptBegin, m_ptEnd);
		DrawProtractor(hDcMem, m_ptBegin, m_ptEnd);
	}
	//POINT pt1 = { 100, 400 };
	//POINT pt2 = { 500, 300 };
	//DrawProtractor(hDcMem, pt1, pt2);

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

//��ֱ�ߣ�5����Ϊ1���ף��̶�ֱ���ܿ��Ϊ60����
//��ͨ�̶�10���س���5�ı���20���س���10�ı���25���س�
void CRealiaWnd::DrawRuler(HDC dc, POINT pt1, POINT pt2, const int iHeight)
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
	pt3.x = pt2.x - iHeight * sin(theta);
	pt3.y = pt2.y + iHeight * cos(theta);
	pt4.x = pt1.x - iHeight * sin(theta);
	pt4.y = pt1.y + iHeight * cos(theta);
	MoveToEx(dc, pt1.x, pt1.y - 1, NULL);
	LineTo(dc, pt2.x, pt2.y - 1);
	LineTo(dc, pt3.x, pt3.y);
	LineTo(dc, pt4.x, pt4.y);
	LineTo(dc, pt1.x, pt1.y - 1);

	pen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	SelectObject(dc, pen);

	//12��΢���ź�����
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
		_T("΢���ź�"));           // lpszFacename 
	SetTextColor(dc, RGB(0, 0, 255));
	HFONT oldfont = (HFONT)SelectObject(dc, hFont);

	int count = pl / 5;
	int j = 0;
	for (int i = 0; i < count; i++) {
		j = i + 1;//Ϊ������ļ�����дһ������
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

//��������
//��ͨ�̶�10���س���5�ı���20���س���10�ı���25���س�
void CRealiaWnd::DrawProtractor(HDC dc, POINT pt1, POINT pt2)
{
	HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	HPEN oldpen = (HPEN)SelectObject(dc, pen);

	HBRUSH hbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldbrush = (HBRUSH)SelectObject(dc, hbrush);

	double px = pt2.x - pt1.x;
	double py = pt2.y - pt1.y;
	double r = sqrt(px * px + py * py) / 2;
	double theta = atan(py / px);//�����X���ƫת��
	POINT ptCenter = { (pt1.x + pt2.x) / 2, (pt1.y + pt2.y) / 2 };//Բ������
	//����Բ����������ε����ϽǺ����½ǵ�����
	POINT ptLeftTop, ptRightBottom;
	ptLeftTop.x = ptCenter.x - r;
	ptLeftTop.y = ptCenter.y - r;
	ptRightBottom.x = ptCenter.x + r;
	ptRightBottom.y = ptCenter.y + r;
	//Arc������������յ���ʱ�뻭��
	Arc(dc, ptLeftTop.x, ptLeftTop.y, ptRightBottom.x, ptRightBottom.y, pt2.x, pt2.y, pt1.x, pt1.y);//�ϰ�Բ

	//����������Բ�Ĵ���һ��׼��
	Ellipse(dc, ptCenter.x - 5, ptCenter.y - 5, ptCenter.x + 5, ptCenter.y + 5);

	//�������������ֱ��
	DrawRuler(dc, pt1, pt2);

	pen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	SelectObject(dc, pen);

	//12��΢���ź�����
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
		_T("΢���ź�"));           // lpszFacename 
	SetTextColor(dc, RGB(0, 0, 255));
	HFONT oldfont = (HFONT)SelectObject(dc, hFont);

	int count = 180;
	double rad = 3.1415926 / 180;
	for (int i = 0; i <= count; i++) {
		MoveToEx(dc, ptCenter.x - r * cos(rad * i + theta), ptCenter.y - r * sin(rad * i + theta), NULL);
		if (i % 10 == 0) {
			LineTo(dc, ptCenter.x - (r - 25) * cos(rad * i + theta), ptCenter.y - (r - 25) * sin(rad * i + theta));
			TCHAR mark[8];
			wsprintf(mark, _T("%d"), 180 - i);
			TextOut(dc, ptCenter.x - (r - 25) * cos(rad * i + theta), ptCenter.y - (r - 25) * sin(rad * i + theta), mark, lstrlen(mark));
		}
		else if (i % 5 == 0) {
			LineTo(dc, ptCenter.x - (r - 20) * cos(rad * i + theta), ptCenter.y - (r - 20) * sin(rad * i + theta));
		}
		else {
			LineTo(dc, ptCenter.x - (r - 10) * cos(rad * i + theta), ptCenter.y - (r - 10) * sin(rad * i + theta));
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
