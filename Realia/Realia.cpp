#include "stdafx.h"
#include "Realia.h"
#include <cstddef>
#include <stdlib.h>

#define AFX_STATIC_DATA extern __declspec(selectany)

AFX_STATIC_DATA HCURSOR _afxCursors[10] = { 0, };

CRealia::CRealia()
{
	Construct();
}

CRealia::~CRealia()
{
	DeleteObject(m_rgn);
}

CRealia::CRealia(POINT ptBegin, POINT ptEnd, UINT nStyle)
{
	Construct();
	m_ptBegin.x = ptBegin.x;
	m_ptBegin.y = ptBegin.y;
	m_ptEnd.x = ptEnd.x;
	m_ptEnd.y = ptEnd.y;
	m_nStyle = nStyle;
	if (m_nStyle == CRealia::Ruler) {
		m_iHeight = 50;
	}
	else if (m_nStyle == CRealia::Protractor) {
		m_iHeight = 50;
	}
}

void CRealia::Construct()
{
	// do one-time initialization if necessary
	//AfxLockGlobals(CRIT_RECTTRACKER);
	static BOOL bInitialized;
	if (!bInitialized)
	{
		// initialize the cursor array
		_afxCursors[0] = ::LoadCursorW(NULL, IDC_SIZENWSE);
		_afxCursors[1] = ::LoadCursorW(NULL, IDC_SIZENESW);
		_afxCursors[2] = _afxCursors[0];
		_afxCursors[3] = _afxCursors[1];
		_afxCursors[4] = ::LoadCursorW(NULL, IDC_SIZENS);
		_afxCursors[5] = ::LoadCursorW(NULL, IDC_SIZEWE);
		_afxCursors[6] = _afxCursors[4];
		_afxCursors[7] = _afxCursors[5];
		_afxCursors[8] = ::LoadCursorW(NULL, IDC_SIZEALL);
		_afxCursors[9] = ::LoadCursorW(NULL, IDC_CROSS);

		bInitialized = TRUE;
	}

	m_nStyle = 0;
	m_iHeight = 0;

	m_rgn = CreateRectRgn(0, 0, 0, 0);
	m_ptBegin = m_ptEnd = { 0, 0 };
	m_bErase = FALSE;
	m_bFinalErase = FALSE;
}

void CRealia::Draw(HDC pDC) const
{
	if (IsRegionNull())
		return;
	// set initial DC state
	//��������������ڴ�DC��SaveDC���ܻ������
	//assert(SaveDC(pDC) != 0);
	//SetMapMode(pDC, MM_TEXT);
	//pDC->SetViewportOrg(0, 0);
	//pDC->SetWindowOrg(0, 0);

	//���ڲ�����
	if (m_nStyle == CRealia::Ruler) {
		DrawRuler(pDC, m_ptBegin, m_ptEnd, m_iHeight);
	}
	else if (m_nStyle == CRealia::Protractor) {
		DrawProtractor(pDC, m_ptBegin, m_ptEnd);
	}

	//assert(RestoreDC(pDC, -1));
}

int CRealia::HitTest(POINT point)
{
	TrackerHit hitResult = hitNothing;

	assert(m_ptBegin.x <= m_ptEnd.x);

	AdjustRgn(0, m_ptBegin, m_ptEnd);

	HRGN rgn = CreateEllipticRgn(m_ptEnd.x - 5, m_ptEnd.y - 5, m_ptEnd.x + 5, m_ptEnd.y + 5);
	if (PtInRegion(rgn, point.x, point.y)) {
		hitResult = hitDrag;
	}
	else if (PtInRegion(m_rgn, point.x, point.y)) {
		hitResult = hitMiddle;
	}

	::DeleteObject(rgn);
	return hitResult;
}

BOOL CRealia::SetCursor(HWND pWnd, UINT nHitTest)
{
	// trackers should only be in client area
	if (nHitTest != HTCLIENT)
		return FALSE;

	// convert cursor position to client co-ordinates
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(pWnd, &point);

	// do hittest and normalize hit
	int nHandle = HitTest(point);
	if (nHandle < 0)
		return FALSE;

	assert(nHandle < _countof(_afxCursors));
	::SetCursor(_afxCursors[nHandle]);
	return TRUE;
}

BOOL CRealia::Track(HWND pWnd, POINT point, BOOL bAllowInvert,
	HWND pWndClipTo)
{
	// perform hit testing on the handles
	int nHandle = HitTest(point);
	if (nHandle < 0)
	{
		// didn't hit a handle, so just return FALSE
		return FALSE;
	}

	// otherwise, call helper function to do the tracking
	m_bAllowInvert = bAllowInvert;
	return TrackHandle(nHandle, pWnd, point, pWndClipTo);
}

BOOL CRealia::TrackRubberBand(HWND pWnd, POINT point, BOOL bAllowInvert)
{
	// simply call helper function to track from bottom right handle
	m_bAllowInvert = bAllowInvert;
	m_ptBegin.x = m_ptEnd.x = point.x;
	m_ptBegin.y = m_ptEnd.y = point.y;
	//m_rgn = CreateRectRgn(0, 0, 0, 0);
	SetRectRgn(m_rgn, point.x, point.y, point.x, point.y);
	return TrackHandle(hitDrag, pWnd, point, NULL);
}

BOOL CRealia::TrackHandle(int nHandle, HWND pWnd, POINT point,
	HWND pWndClipTo)
{
	assert(nHandle >= 0);
	assert(nHandle <= 9);

	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return FALSE;

	//AfxLockTempMaps();  // protect maps while looping

	assert(!m_bFinalErase);

	// set capture to the window which received this message
	SetCapture(pWnd);
	assert(pWnd == GetCapture());
	UpdateWindow(pWnd);
	if (pWndClipTo != NULL)
		UpdateWindow(pWndClipTo);
	POINT ptBeginSave, ptEndSave;
	ptBeginSave.x = m_ptBegin.x;
	ptBeginSave.y = m_ptBegin.y;
	ptEndSave.x = m_ptEnd.x;
	ptEndSave.y = m_ptEnd.y;

	POINT ptBeginOld, ptEndOld;
	BOOL bMoved = FALSE;
	POINT ptLast;

	// get messages until capture lost or cancelled/accepted
	for (;;)
	{
		MSG msg;
		assert(::GetMessage(&msg, NULL, 0, 0));

		if (GetCapture() != pWnd)
			break;

		switch (msg.message)
		{
			// handle movement/accept messages
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			EqualPoint(&ptBeginOld, &m_ptBegin);//ptBeginOld = m_ptBegin;
			EqualPoint(&ptEndOld, &m_ptEnd);//ptEndOld = m_ptEnd;

			ptLast = { GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam) };
			ModifyPointers(nHandle, ptBeginSave, ptEndSave, point, ptLast);
			AdjustRgn(nHandle, m_ptBegin, m_ptEnd);

			// only redraw and callback if the rect actually changed!
			m_bFinalErase = (msg.message == WM_LBUTTONUP);
			if (!IsPointEqual(ptBeginOld, m_ptBegin) || !IsPointEqual(ptEndOld, m_ptEnd) || m_bFinalErase)
			{
				if (bMoved)
				{
					m_bErase = TRUE;
					InvalidateRect(pWnd, NULL, false);
					UpdateWindow(pWnd);
				}
				if (msg.message != WM_LBUTTONUP)
					bMoved = TRUE;
			}
			if (m_bFinalErase)
				goto ExitLoop;

			if (!IsPointEqual(ptBeginOld, m_ptBegin) || !IsPointEqual(ptEndOld, m_ptEnd))
			{
				m_bErase = FALSE;
				//InvalidateRect(pWnd, NULL, false);
				//UpdateWindow(pWnd);
			}
			break;

			// handle cancel messages
		case WM_KEYDOWN:
			if (msg.wParam != VK_ESCAPE)
				break;
		case WM_RBUTTONDOWN:
			if (bMoved)
			{
				m_bErase = m_bFinalErase = TRUE;
				//InvalidateRect(pWnd, NULL, false);
				//UpdateWindow(pWnd);
			}
			EqualPoint(&m_ptBegin, &ptBeginSave);//m_ptBegin = ptBeginSave;
			EqualPoint(&m_ptEnd, &ptEndSave);//m_ptEnd = ptEndSave;
			goto ExitLoop;

			// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

ExitLoop:
	ReleaseCapture();

	//AfxUnlockTempMaps(FALSE);

	// restore rect in case bMoved is still FALSE
	if (!bMoved) {
		EqualPoint(&m_ptBegin, &ptBeginSave);//m_ptBegin = ptBeginSave;
		EqualPoint(&m_ptEnd, &ptEndSave);//m_ptEnd = ptEndSave;
	}
	m_bFinalErase = FALSE;
	m_bErase = FALSE;

	// return TRUE only if rect has changed
	return !IsPointEqual(ptBeginSave, m_ptBegin) || !IsPointEqual(ptEndSave, m_ptEnd);
}

BOOL CRealia::IsRegionNull() const
{
	return IsPointEqual(m_ptBegin, m_ptEnd);
}

void CRealia::ModifyPointers(int nHandle, POINT ptBeginSave, POINT ptEndSave, POINT ptDown, POINT ptLast)
{
	assert(nHandle >= 0);
	assert(nHandle <= 9);

	if (nHandle == hitMiddle) {//�ƶ�����
		m_ptBegin.x = ptBeginSave.x + ptLast.x - ptDown.x;
		m_ptBegin.y = ptBeginSave.y + ptLast.y - ptDown.y;
		m_ptEnd.x = ptEndSave.x + ptLast.x - ptDown.x;
		m_ptEnd.y = ptEndSave.y + ptLast.y - ptDown.y;
	}

	if (nHandle == hitDrag) {
		if (ptBeginSave.x == ptEndSave.x && ptBeginSave.y == ptEndSave.y) {//�״λ�
			m_ptBegin = ptBeginSave;
			m_ptEnd = ptLast;
		}
		else {//��������
			if (ptLast.x < ptBeginSave.x) {
				m_ptBegin = ptLast;
				m_ptEnd = ptBeginSave;
			}
			else {
				m_ptBegin = ptBeginSave;
				m_ptEnd.x = ptEndSave.x + ptLast.x - ptDown.x;
				m_ptEnd.y = ptEndSave.y + ptLast.y - ptDown.y;
			}
		}
	}

	if (m_ptBegin.x > m_ptEnd.x) {
		LONG tmp = m_ptBegin.x;
		m_ptBegin.x = m_ptEnd.x;
		m_ptEnd.x = tmp;
		tmp = m_ptBegin.y;
		m_ptBegin.y = m_ptEnd.y;
		m_ptEnd.y = tmp;
	}
}

void CRealia::AdjustRgn(int nHandle, POINT ptBegin, POINT ptEnd)
{
	if (m_nStyle == CRealia::Ruler) {
		POINT pt1, pt2, pt3, pt4;
		pt1.x = ptBegin.x;
		pt1.y = ptBegin.y;
		pt2.x = ptEnd.x;
		pt2.y = ptEnd.y;
		double px = pt2.x - pt1.x;
		double py = pt2.y - pt1.y;
		double pl = sqrt(px * px + py * py);
		double theta = atan(py / px);
		pt3.x = pt2.x - m_iHeight * sin(theta);
		pt3.y = pt2.y + m_iHeight * cos(theta);
		pt4.x = pt1.x - m_iHeight * sin(theta);
		pt4.y = pt1.y + m_iHeight * cos(theta);
		POINT pts[4];
		EqualPoint(&pts[0], &pt1);//pts[0] = pt1;
		EqualPoint(&pts[1], &pt2);//pts[1] = pt2;
		EqualPoint(&pts[2], &pt3);//pts[2] = pt3;
		EqualPoint(&pts[3], &pt4);//pts[3] = pt4;
		DeleteObject(m_rgn);
		m_rgn = CreatePolygonRgn(pts, 4, WINDING);
	}
	else if (m_nStyle == CRealia::Protractor) {
		POINT pt1, pt2, pt3, pt4;
		pt1.x = ptBegin.x;
		pt1.y = ptBegin.y;
		pt2.x = ptEnd.x;
		pt2.y = ptEnd.y;
		double px = pt2.x - pt1.x;
		double py = pt2.y - pt1.y;
		double pl = sqrt(px * px + py * py);
		double theta = atan(py / px);
		pt3.x = pt2.x - m_iHeight * sin(theta);
		pt3.y = pt2.y + m_iHeight * cos(theta);
		pt4.x = pt1.x - m_iHeight * sin(theta);
		pt4.y = pt1.y + m_iHeight * cos(theta);
		POINT pts[4];
		EqualPoint(&pts[0], &pt1);//pts[0] = pt1;
		EqualPoint(&pts[1], &pt2);//pts[1] = pt2;
		EqualPoint(&pts[2], &pt3);//pts[2] = pt3;
		EqualPoint(&pts[3], &pt4);//pts[3] = pt4;
		HRGN rgnRect2 = CreatePolygonRgn(pts, 4, WINDING);

		double r = sqrt(px * px + py * py) / 2;
		POINT ptCenter = { (pt1.x + pt2.x) / 2, (pt1.y + pt2.y) / 2 };//Բ������
		//����Բ����������ε����ϽǺ����½ǵ�����
		POINT ptLeftTop, ptRightBottom;
		ptLeftTop.x = ptCenter.x - r;
		ptLeftTop.y = ptCenter.y - r;
		ptRightBottom.x = ptCenter.x + r;
		ptRightBottom.y = ptCenter.y + r;
		HRGN rgnCircle = CreateEllipticRgn(ptLeftTop.x, ptLeftTop.y, ptRightBottom.x, ptRightBottom.y);

		POINT pt5, pt6;
		pt5.x = pt2.x + r * sin(theta);
		pt5.y = pt2.y - r * cos(theta);
		pt6.x = pt1.x + r * sin(theta);
		pt6.y = pt1.y - r * cos(theta);
		EqualPoint(&pts[2], &pt5);
		EqualPoint(&pts[3], &pt6);
		HRGN rgnRect1 = CreatePolygonRgn(pts, 4, WINDING);

		//Բ���Ͼ��ν���Ϊ�ϰ�Բ
		HRGN rgnHalfCircle = CreateRectRgn(0, 0, 0, 0);
		CombineRgn(rgnHalfCircle, rgnCircle, rgnRect1, RGN_AND);
		//�ϰ�Բ���¾��κϲ�ΪԲ�������
		DeleteObject(m_rgn);
		m_rgn = CreateRectRgn(0, 0, 0, 0);
		//��������൱���½����򣬾�������������ͷ�ԭ����Ŀ������Ϊ��
		//���廹Ҫ����������Դ���룬���ֻ�ǵ�ǰ״���Ĳ��Խ��
		int ret = CombineRgn(m_rgn, rgnHalfCircle, rgnRect2, RGN_OR);

		DeleteObject(rgnCircle);
		DeleteObject(rgnHalfCircle);
		DeleteObject(rgnRect1);
		DeleteObject(rgnRect2);
	}
}

//��ֱ�ߣ�5����Ϊ1���ף��̶�ֱ���ܿ��Ϊ60����
//��ͨ�̶�10���س���5�ı���20���س���10�ı���25���س�
void CRealia::DrawRuler(HDC dc, POINT pt1, POINT pt2, int iHeight) const
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
	pt3.x = pt2.x - iHeight * sin(theta);
	pt3.y = pt2.y + iHeight * cos(theta);
	pt4.x = pt1.x - iHeight * sin(theta);
	pt4.y = pt1.y + iHeight * cos(theta);
	MoveToEx(dc, pt1.x, pt1.y - 1, NULL);
	LineTo(dc, pt2.x, pt2.y - 1);
	LineTo(dc, pt3.x, pt3.y);
	LineTo(dc, pt4.x, pt4.y);
	LineTo(dc, pt1.x, pt1.y - 1);

	Ellipse(dc, pt2.x - 5, pt2.y - 5, pt2.x + 5, pt2.y + 5);

	HPEN pen2 = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	SelectObject(dc, pen2);

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
	::DeleteObject(pen2);
	::DeleteObject(oldpen);
}

//��������
//��ͨ�̶�10���س���5�ı���20���س���10�ı���25���س�
void CRealia::DrawProtractor(HDC dc, POINT pt1, POINT pt2) const
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

	HPEN pen2 = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	SelectObject(dc, pen2);

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
	::DeleteObject(pen2);
	::DeleteObject(oldpen);
}
