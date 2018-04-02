#include "stdafx.h"
#include "Realia.h"
#include <cstddef>
#include <stdlib.h>

#define AFX_STATIC_DATA extern __declspec(selectany)

AFX_STATIC_DATA HCURSOR _afxCursors[10] = { 0, };
AFX_STATIC_DATA double pi = 3.1415926;
AFX_STATIC_DATA UINT rDrag = 5;//用于伸缩区的半径
AFX_STATIC_DATA UINT rGoniometer = 30;//用于角度尺的半径

CRealia::CRealia()
{
	Construct();
}

CRealia::~CRealia()
{
	//DeleteObject(m_rgn);
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
	else if (m_nStyle == CRealia::AcuteTriangle) {
		m_iAngle = 30;
	}
	else if (m_nStyle == CRealia::IsoscelesTriangle) {
		m_iAngle = 45;
	}
	else if (m_nStyle == CRealia::Protractor) {
		m_iHeight = 50;
	}
	else if (m_nStyle == CRealia::Compass) {
		m_iAngle = 45;
		m_iHeight = 300;
	}
}

CRealia::CRealia(const CRealia &other)
{
	Construct();
	m_nStyle = other.m_nStyle;
	DeleteObject(m_rgn);
	m_rgn = other.m_rgn;
	m_ptBegin.x = other.m_ptBegin.x;
	m_ptBegin.y = other.m_ptBegin.y;
	m_ptEnd.x = other.m_ptEnd.x;
	m_ptEnd.y = other.m_ptEnd.y;
	m_ptTmp.x = other.m_ptTmp.x;
	m_ptTmp.y = other.m_ptTmp.y;
	m_bSelect = other.m_bSelect;
	m_bAllowInvert = other.m_bAllowInvert;
	m_iHeight = other.m_iHeight;
	m_iAngle = other.m_iAngle;
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
	m_iAngle = 0;

	m_rgn = CreateRectRgn(0, 0, 0, 0);
	m_ptBegin = m_ptEnd = m_ptTmp = { 0, 0 };
	m_bSelect = FALSE;
}

void CRealia::Draw(HDC pDC) const
{
	if (IsRegionNull())
		return;
	// set initial DC state
	//如果传进来的是内存DC，SaveDC可能会出问题
	//assert(SaveDC(pDC) != 0);
	//SetMapMode(pDC, MM_TEXT);
	//pDC->SetViewportOrg(0, 0);
	//pDC->SetWindowOrg(0, 0);

	//画内部区域
	if (m_nStyle == CRealia::Ruler) {
		DrawRuler(pDC, m_ptBegin, m_ptEnd, m_iHeight);
	}
	else if (m_nStyle == CRealia::AcuteTriangle) {
		DrawTriangle(pDC, m_ptBegin, m_ptEnd, m_iAngle);
	}
	else if (m_nStyle == CRealia::IsoscelesTriangle) {
		DrawTriangle(pDC, m_ptBegin, m_ptEnd, m_iAngle);
	}
	else if (m_nStyle == CRealia::Protractor) {
		DrawProtractor(pDC, m_ptBegin, m_ptEnd);
	}
	else if (m_nStyle == CRealia::Goniometer) {
		DrawGoniometer(pDC, m_ptBegin, m_ptEnd, m_ptTmp);
	}
	else if (m_nStyle == CRealia::Compass) {
		DrawCompass(pDC, m_ptBegin, m_ptEnd, m_iAngle);
	}

	//assert(RestoreDC(pDC, -1));
}

int CRealia::HitTest(POINT point)
{
	TrackerHit hitResult = hitNothing;

	//AdjustRgn(0, m_ptBegin, m_ptEnd);

	HRGN rgn = CreateEllipticRgn(m_ptEnd.x - rDrag, m_ptEnd.y - rDrag, m_ptEnd.x + rDrag, m_ptEnd.y + rDrag);
	if (m_nStyle == CRealia::Goniometer) {
		HRGN rgn2 = CreateEllipticRgn(m_ptBegin.x - rDrag, m_ptBegin.y - rDrag, m_ptBegin.x + rDrag, m_ptBegin.y + rDrag);
		if (PtInRegion(rgn, point.x, point.y))
			hitResult = hitTopRight;
		else if (PtInRegion(rgn2, point.x, point.y))
			hitResult = hitTopLeft;
		else if (PtInRegion(m_rgn, point.x, point.y))
			hitResult = hitMiddle;
		::DeleteObject(rgn2);
	}
	else if (PtInRegion(rgn, point.x, point.y)) {
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
	int nHandle = hitDrag;
	if (m_nStyle == CRealia::Goniometer)
		nHandle = hitTopLeft;
	return TrackHandle(nHandle, pWnd, point, NULL);
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

	// set capture to the window which received this message
	SetCapture(pWnd);
	assert(pWnd == GetCapture());
	UpdateWindow(pWnd);
	if (pWndClipTo != NULL)
		UpdateWindow(pWndClipTo);
	POINT ptBeginSave, ptEndSave, ptTmpSave;
	ptBeginSave.x = m_ptBegin.x;
	ptBeginSave.y = m_ptBegin.y;
	ptEndSave.x = m_ptEnd.x;
	ptEndSave.y = m_ptEnd.y;
	ptTmpSave.x = m_ptTmp.x;
	ptTmpSave.y = m_ptTmp.y;

	POINT ptBeginOld, ptEndOld;
	BOOL bMoved = FALSE;
	POINT ptLast;
	UINT lBtnCount = 0;

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
			if (m_nStyle != CRealia::Goniometer)
				goto ExitLoop;

			if (lBtnCount > 0 || m_ptTmp.x != 0 || m_ptTmp.y != 0)
				goto ExitLoop;

			if (m_nStyle == CRealia::Goniometer) {
				++lBtnCount;
				EqualPoint(&m_ptTmp, &m_ptEnd);
				nHandle = hitTopRight;
			}

			break;
		case WM_MOUSEMOVE:
			EqualPoint(&ptBeginOld, &m_ptBegin);//ptBeginOld = m_ptBegin;
			EqualPoint(&ptEndOld, &m_ptEnd);//ptEndOld = m_ptEnd;

			ptLast = { GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam) };
			ModifyPointers(nHandle, ptBeginSave, ptEndSave, ptTmpSave, point, ptLast);

			// only redraw and callback if the rect actually changed!
			if (!IsPointEqual(ptBeginOld, m_ptBegin) || !IsPointEqual(ptEndOld, m_ptEnd))
			{
				bMoved = TRUE;
				AdjustRgn(nHandle, m_ptBegin, m_ptEnd);
				InvalidateRect(pWnd, NULL, false);
				UpdateWindow(pWnd);
			}

			break;

			// handle cancel messages
		case WM_KEYDOWN:
			if (msg.wParam != VK_ESCAPE)
				break;
		case WM_RBUTTONDOWN:
			if (bMoved)
			{
				InvalidateRect(pWnd, NULL, false);
				UpdateWindow(pWnd);
			}
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

	// return TRUE only if rect has changed
	return !IsPointEqual(ptBeginSave, m_ptBegin) || !IsPointEqual(ptEndSave, m_ptEnd);
}

BOOL CRealia::IsRegionNull() const
{
	return IsPointEqual(m_ptBegin, m_ptEnd);
}

void CRealia::ModifyPointers(int nHandle, POINT ptBeginSave, POINT ptEndSave, POINT ptTmpSave, POINT ptDown, POINT ptLast)
{
	assert(nHandle >= 0);
	assert(nHandle <= 9);

	if (m_nStyle == CRealia::Goniometer) {
		if (nHandle == hitTopLeft) {
			if (ptTmpSave.x == 0 && ptTmpSave.y == 0) {//首次画
				m_ptBegin = ptBeginSave;
				m_ptEnd = ptLast;
			}
			else {
				m_ptBegin = ptLast;
			}
		}
		else if (nHandle == hitTopRight) {
			m_ptEnd = ptLast;
		}
		else if (nHandle == hitMiddle) {
			m_ptBegin.x = ptBeginSave.x + ptLast.x - ptDown.x;
			m_ptBegin.y = ptBeginSave.y + ptLast.y - ptDown.y;
			m_ptEnd.x = ptEndSave.x + ptLast.x - ptDown.x;
			m_ptEnd.y = ptEndSave.y + ptLast.y - ptDown.y;
			m_ptTmp.x = ptTmpSave.x + ptLast.x - ptDown.x;
			m_ptTmp.y = ptTmpSave.y + ptLast.y - ptDown.y;
		}
		return;
	}

	if (nHandle == hitMiddle) {//移动区域
		m_ptBegin.x = ptBeginSave.x + ptLast.x - ptDown.x;
		m_ptBegin.y = ptBeginSave.y + ptLast.y - ptDown.y;
		m_ptEnd.x = ptEndSave.x + ptLast.x - ptDown.x;
		m_ptEnd.y = ptEndSave.y + ptLast.y - ptDown.y;
	}

	if (nHandle == hitDrag) {
		if (ptBeginSave.x == ptEndSave.x && ptBeginSave.y == ptEndSave.y) {//首次画
			m_ptBegin = ptBeginSave;
			m_ptEnd = ptLast;
		}
		else {//拉伸区域
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
	else if (m_nStyle == CRealia::AcuteTriangle || m_nStyle == CRealia::IsoscelesTriangle) {
		POINT pt1, pt2, pt3;
		pt1.x = ptBegin.x;
		pt1.y = ptBegin.y;
		pt2.x = ptEnd.x;
		pt2.y = ptEnd.y;
		double px = pt2.x - pt1.x;
		double py = pt2.y - pt1.y;
		double pl = sqrt(px * px + py * py);
		double theta = atan(py / px);
		double ptheta = m_iAngle * pi / 180;
		pt3.x = pt1.x - pl * tan(ptheta) * sin(theta);
		pt3.y = pt1.y + pl * tan(ptheta) * cos(theta);
		POINT pts[3];
		EqualPoint(&pts[0], &pt1);//pts[0] = pt1;
		EqualPoint(&pts[1], &pt2);//pts[1] = pt2;
		EqualPoint(&pts[2], &pt3);//pts[2] = pt3;
		DeleteObject(m_rgn);
		m_rgn = CreatePolygonRgn(pts, 3, WINDING);
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
		POINT ptCenter = { (pt1.x + pt2.x) / 2, (pt1.y + pt2.y) / 2 };//圆心坐标
		//计算圆的外接正方形的左上角和右下角的坐标
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

		//圆和上矩形交集为上半圆
		HRGN rgnHalfCircle = CreateRectRgn(0, 0, 0, 0);
		CombineRgn(rgnHalfCircle, rgnCircle, rgnRect1, RGN_AND);
		//上半圆和下矩形合并为圆规的区域
		DeleteObject(m_rgn);
		m_rgn = CreateRectRgn(0, 0, 0, 0);
		//组合区域相当于新建区域，经测试如果不先释放原区域，目标区域将为空
		//具体还要看组合区域的源代码，这个只是当前状况的测试结果
		int ret = CombineRgn(m_rgn, rgnHalfCircle, rgnRect2, RGN_OR);

		DeleteObject(rgnCircle);
		DeleteObject(rgnHalfCircle);
		DeleteObject(rgnRect1);
		DeleteObject(rgnRect2);
	}
	else if (m_nStyle == CRealia::Goniometer) {
		if (m_ptTmp.x != 0 || m_ptTmp.y != 0) {
			POINT ptLeftTop, ptRightBottom;
			ptLeftTop.x = m_ptTmp.x - rGoniometer;
			ptLeftTop.y = m_ptTmp.y - rGoniometer;
			ptRightBottom.x = m_ptTmp.x + rGoniometer;
			ptRightBottom.y = m_ptTmp.y + rGoniometer;
			::DeleteObject(m_rgn);
			m_rgn = CreateEllipticRgn(ptLeftTop.x, ptLeftTop.y, ptRightBottom.x, ptRightBottom.y);
		}
	}
}

//画直尺：5像素为1毫米，固定直尺总宽度为60像素
//普通刻度10像素长，5的倍数20像素长，10的倍数25像素长
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

	Ellipse(dc, pt2.x - rDrag, pt2.y - rDrag, pt2.x + rDrag, pt2.y + rDrag);

	HPEN pen2 = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	SelectObject(dc, pen2);

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
	for (int i = 0; i < count - 2; i++) {
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
	::DeleteObject(pen2);
	::DeleteObject(oldpen);
}

//画三角形
//当angle = 30度，画出来的就是锐角三角形
//当angle = 45度，画出来的就是等腰三角形
void CRealia::DrawTriangle(HDC dc, POINT pt1, POINT pt2, int angle) const
{
	assert(angle >= 0);
	assert(angle <= 90);
	HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	HPEN oldpen = (HPEN)SelectObject(dc, pen);

	HBRUSH hbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldbrush = (HBRUSH)SelectObject(dc, hbrush);

	POINT pt3;
	double px = pt2.x - pt1.x;
	double py = pt2.y - pt1.y;
	double pl = sqrt(px * px + py * py);
	double theta = atan(py / px);
	double ptheta = angle * pi / 180;
	pt3.x = pt1.x - pl * tan(ptheta) * sin(theta);
	pt3.y = pt1.y + pl * tan(ptheta) * cos(theta);
	MoveToEx(dc, pt1.x, pt1.y - 1, NULL);
	LineTo(dc, pt2.x, pt2.y - 1);
	LineTo(dc, pt3.x, pt3.y);
	LineTo(dc, pt1.x, pt1.y - 1);

	Ellipse(dc, pt2.x - rDrag, pt2.y - rDrag, pt2.x + rDrag, pt2.y + rDrag);

	HPEN pen2 = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	SelectObject(dc, pen2);

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
	for (int i = 0; i < count - 5; i++) {
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

	count = pl * tan(ptheta) / 5;
	for (int i = 0; i < count - 5; i++) {
		j = i + 1;//为了下面的计算少写一个括号
		MoveToEx(dc, pt1.x - 5 * j * sin(theta), pt1.y + 5 * j * cos(theta), NULL);
		if (i % 10 == 0) {
			LineTo(dc, pt1.x - 5 * j * sin(theta) + 25 * cos(theta), pt1.y + 5 * j * cos(theta) + 25 * sin(theta));
			TCHAR mark[8];
			wsprintf(mark, _T("%d"), i / 10);
			TextOut(dc, pt1.x - 5 * j * sin(theta) + 25 * cos(theta) + 3, pt1.y + 5 * j * cos(theta) + 25 * sin(theta) - 4, mark, lstrlen(mark));
		}
		else if (i % 5 == 0) {
			LineTo(dc, pt1.x - 5 * j * sin(theta) + 20 * cos(theta), pt1.y + 5 * j * cos(theta) + 20 * sin(theta));
		}
		else {
			LineTo(dc, pt1.x - 5 * j * sin(theta) + 10 * cos(theta), pt1.y + 5 * j * cos(theta) + 10 * sin(theta));
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

//画量角器
//普通刻度10像素长，5的倍数20像素长，10的倍数25像素长
void CRealia::DrawProtractor(HDC dc, POINT pt1, POINT pt2) const
{
	HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	HPEN oldpen = (HPEN)SelectObject(dc, pen);

	HBRUSH hbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldbrush = (HBRUSH)SelectObject(dc, hbrush);

	double px = pt2.x - pt1.x;
	double py = pt2.y - pt1.y;
	double r = sqrt(px * px + py * py) / 2;
	double theta = atan(py / px);//相对于X轴的偏转角
	POINT ptCenter = { (pt1.x + pt2.x) / 2, (pt1.y + pt2.y) / 2 };//圆心坐标
	//计算圆的外接正方形的左上角和右下角的坐标
	POINT ptLeftTop, ptRightBottom;
	ptLeftTop.x = ptCenter.x - r;
	ptLeftTop.y = ptCenter.y - r;
	ptRightBottom.x = ptCenter.x + r;
	ptRightBottom.y = ptCenter.y + r;
	//Arc函数从起点至终点逆时针画弧
	Arc(dc, ptLeftTop.x, ptLeftTop.y, ptRightBottom.x, ptRightBottom.y, pt2.x, pt2.y, pt1.x, pt1.y);//上半圆

	//在量角器的圆心处画一个准心
	Ellipse(dc, ptCenter.x - 5, ptCenter.y - 5, ptCenter.x + 5, ptCenter.y + 5);

	//画量角器下面的直尺
	DrawRuler(dc, pt1, pt2, m_iHeight);

	HPEN pen2 = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	SelectObject(dc, pen2);

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

	int count = 180;
	double rad = pi / 180;
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

//画角度尺
//余弦定理a^2 = b^2 + c^2 - 2bc * cosA
//cosA = (b^2 + c^2 - a^2) / (2bc)
void CRealia::DrawGoniometer(HDC dc, POINT pt1, POINT pt2, POINT pt3) const
{
	HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	HPEN oldpen = (HPEN)SelectObject(dc, pen);

	HBRUSH hbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldbrush = (HBRUSH)SelectObject(dc, hbrush);

	if (pt3.x == 0 && pt3.y == 0) {
		MoveToEx(dc, pt1.x, pt1.y, NULL);
		LineTo(dc, pt2.x, pt2.y);
	}
	else {
		MoveToEx(dc, pt1.x, pt1.y, NULL);
		LineTo(dc, pt3.x, pt3.y);
		LineTo(dc, pt2.x, pt2.y);

		Ellipse(dc, pt1.x - rDrag, pt1.y - rDrag, pt1.x + rDrag, pt1.y + rDrag);
		Ellipse(dc, pt2.x - rDrag, pt2.y - rDrag, pt2.x + rDrag, pt2.y + rDrag);

		//用余弦定理求夹角
		LONG pxAB = abs(pt3.x - pt1.x);
		LONG pyAB = abs(pt3.y - pt1.y);
		double c = sqrt(pxAB * pxAB + pyAB * pyAB);
		LONG pxAC = abs(pt3.x - pt2.x);
		LONG pyAC = abs(pt3.y - pt2.y);
		double b = sqrt(pxAC * pxAC + pyAC * pyAC);
		LONG pxBC = abs(pt2.x - pt1.x);
		LONG pyBC = abs(pt2.y - pt1.y);
		double a = sqrt(pxBC * pxBC + pyBC * pyBC);
		if (2 * b * c == 0) return;
		double theta = acos((b * b + c * c - a * a) / (2 * b * c));
		int angle = theta * 180 / pi;

		//计算圆的外接正方形的左上角和右下角的坐标
		POINT ptLeftTop, ptRightBottom;
		ptLeftTop.x = pt3.x - rGoniometer;
		ptLeftTop.y = pt3.y - rGoniometer;
		ptRightBottom.x = pt3.x + rGoniometer;
		ptRightBottom.y = pt3.y + rGoniometer;
		//Arc函数从起点至终点逆时针画弧
		Arc(dc, ptLeftTop.x, ptLeftTop.y, ptRightBottom.x, ptRightBottom.y, pt2.x, pt2.y, pt1.x, pt1.y);

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
		SetTextColor(dc, RGB(0, 0, 0));
		HFONT oldfont = (HFONT)SelectObject(dc, hFont);

		TCHAR mark[8];
		wsprintf(mark, _T("%d°"), angle);
		TextOut(dc, pt3.x, pt3.y, mark, lstrlen(mark));

		SelectObject(dc, oldfont);
		::DeleteObject(hFont);
		::DeleteObject(oldfont);
	}

	SelectObject(dc, oldbrush);
	::DeleteObject(hbrush);
	::DeleteObject(oldbrush);
	SelectObject(dc, oldpen);
	::DeleteObject(pen);
	::DeleteObject(oldpen);
}

//画圆规
//以m_ptBegin为轴心点转动，m_ptEnd经过的路线即为圆规画出来的图形
void CRealia::DrawCompass(HDC dc, POINT pt1, POINT pt2, int angle) const
{
	HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	HPEN oldpen = (HPEN)SelectObject(dc, pen);

	HBRUSH hbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldbrush = (HBRUSH)SelectObject(dc, hbrush);

	//求顶点（圆规的头）坐标
	POINT pt3;
	double px = pt2.x - pt1.x;
	double py = pt2.y - pt1.y;
	double pl = sqrt(px * px + py * py);
	double theta1 = acos((pl * pl) / (2 * pl * m_iHeight));
	double theta2 = atan(py / px);
	pt3.x = pt1.x + m_iHeight * cos(theta1 - theta2);
	pt3.y = pt1.y - m_iHeight * sin(theta1 - theta2);

	MoveToEx(dc, pt1.x, pt1.y, NULL);
	LineTo(dc, pt3.x, pt3.y);
	LineTo(dc, pt2.x, pt2.y);

	SelectObject(dc, oldbrush);
	::DeleteObject(hbrush);
	::DeleteObject(oldbrush);
	SelectObject(dc, oldpen);
	::DeleteObject(pen);
	::DeleteObject(oldpen);
}
