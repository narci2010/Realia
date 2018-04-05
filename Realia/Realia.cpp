#include "Realia.h"
#include <cstddef>
#include <stdlib.h>

AFX_STATIC_DATA HCURSOR _afxCursors[10] = { 0, };
AFX_STATIC_DATA UINT rDrag = 5;//用于伸缩区的半径
AFX_STATIC_DATA UINT rGoniometer = 30;//用于角度尺的半径
AFX_STATIC_DATA UINT lCompass = 200;//用于圆规的腿长
AFX_STATIC_DATA TCHAR* pFont = _T("微软雅黑");//字体类型
AFX_STATIC_DATA UINT iFontSize = 8;//用于字体大小

std::vector<CArc> m_vecArcs;
CArc *m_pArc = NULL;

CRealia::CRealia()
{
	Construct();
}

CRealia::~CRealia()
{
	//DeleteObject(m_rgn);
	//DeleteObject(m_rgnTopRight);
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
		m_iHeight = lCompass;
	}
}

CRealia::CRealia(const CRealia &other)
{
	Construct();
	m_nStyle = other.m_nStyle;
	DeleteObject(m_rgn);
	m_rgn = other.m_rgn;
	DeleteObject(m_rgnTopRight);
	m_rgnTopRight = other.m_rgnTopRight;
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
		_afxCursors[4] = ::LoadCursorW(NULL, IDC_CROSS); //::LoadCursorW(NULL, IDC_SIZENS);
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
	m_rgnTopRight = CreateRectRgn(0, 0, 0, 0);
	m_ptBegin = m_ptEnd = m_ptTmp = { 0, 0 };
	m_bSelect = FALSE;
}

void CRealia::Draw(Graphics* pGraphics)
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
		DrawRuler(pGraphics, m_ptBegin, m_ptEnd, m_iHeight);
	}
	else if (m_nStyle == CRealia::AcuteTriangle) {
		DrawTriangle(pGraphics, m_ptBegin, m_ptEnd, m_iAngle);
	}
	else if (m_nStyle == CRealia::IsoscelesTriangle) {
		DrawTriangle(pGraphics, m_ptBegin, m_ptEnd, m_iAngle);
	}
	else if (m_nStyle == CRealia::Protractor) {
		DrawProtractor(pGraphics, m_ptBegin, m_ptEnd);
	}
	else if (m_nStyle == CRealia::Goniometer) {
		DrawGoniometer(pGraphics, m_ptBegin, m_ptEnd, m_ptTmp);
	}
	else if (m_nStyle == CRealia::Compass) {
		DrawCompass(pGraphics, m_ptBegin, m_ptEnd, m_iHeight);
	}

	DrawArc(pGraphics);

	//assert(RestoreDC(pDC, -1));
}

int CRealia::HitTest(POINT point)
{
	TrackerHit hitResult = hitNothing;

	//AdjustRgn(0, m_ptBegin, m_ptEnd);

	if (m_nStyle == CRealia::Goniometer) {//角度尺
		HRGN rgnTopLeft = CreateEllipticRgn(m_ptBegin.x - rDrag, m_ptBegin.y - rDrag, m_ptBegin.x + rDrag, m_ptBegin.y + rDrag);
		if (PtInRegion(m_rgnTopRight, point.x, point.y))
			hitResult = hitTopRight;
		else if (PtInRegion(rgnTopLeft, point.x, point.y))
			hitResult = hitTopLeft;
		else if (PtInRegion(m_rgn, point.x, point.y))
			hitResult = hitMiddle;
		::DeleteObject(rgnTopLeft);
	}
	else if (m_nStyle == CRealia::Compass) {//圆规
		if (PtInRegion(m_rgn, point.x, point.y)) {
			hitResult = hitMiddle;
		}
		else {
			POINT pt3, pt4, pt5, pt6, pt7, pt8, pt9;
			GetCompassPoints(m_ptBegin, m_ptEnd, &pt3, &pt4, &pt5, &pt6, &pt7, &pt8, &pt9);

			POINT ptsTop[4];

			EqualPoint(&ptsTop[0], &pt6);
			EqualPoint(&ptsTop[1], &pt8);
			EqualPoint(&ptsTop[2], &pt9);
			EqualPoint(&ptsTop[3], &pt7);
			HRGN rgnTop = CreatePolygonRgn(ptsTop, 4, WINDING);

			if (PtInRegion(m_rgnTopRight, point.x, point.y)) {
				hitResult = hitTopRight;
			}
			else if (PtInRegion(rgnTop, point.x, point.y)) {
				hitResult = hitTop;
			}

			::DeleteObject(rgnTop);
		}
	}
	else if (PtInRegion(m_rgnTopRight, point.x, point.y)) {
		hitResult = hitDrag;
	}
	else if (PtInRegion(m_rgn, point.x, point.y)) {
		hitResult = hitMiddle;
	}

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

	if (m_nStyle == CRealia::Compass && nHandle == hitTop) {
		m_pArc = new CArc(m_ptBegin, DistanceOfTwoPoint(m_ptBegin, m_ptEnd));
		m_vecArcs.push_back(*m_pArc);
		delete m_pArc;
		m_pArc = &m_vecArcs.at(m_vecArcs.size() - 1);
	}
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
	SetRectRgn(m_rgnTopRight, point.x, point.y, point.x, point.y);
	int nHandle = hitDrag;
	if (m_nStyle == CRealia::Goniometer)
		nHandle = hitTopLeft;
	else if (m_nStyle == CRealia::Compass)
		nHandle = hitTopRight;
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
			if (m_pArc != NULL) {
				m_pArc = NULL;
			}

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

	if (nHandle == hitTopLeft) {
		if (ptBeginSave.x == ptEndSave.x && ptBeginSave.y == ptEndSave.y) {//首次画
			m_ptBegin = ptBeginSave;
			m_ptEnd = ptLast;
		}
		else {
			m_ptBegin.x = ptBeginSave.x + ptLast.x - ptDown.x;
			m_ptBegin.y = ptBeginSave.y + ptLast.y - ptDown.y;
		}
	}
	else if (nHandle == hitTopRight) {
		m_ptEnd.x = ptEndSave.x + ptLast.x - ptDown.x;
		m_ptEnd.y = ptEndSave.y + ptLast.y - ptDown.y;
	}
	else if (nHandle == hitTop) {
		if (m_nStyle == CRealia::Compass) {//圆规画圆弧
			//已知圆心、半径和圆上一点坐标ptEndSave
			double r = DistanceOfTwoPoint(ptBeginSave, ptEndSave);
			double theta, theta2;
			theta = GetAngleOfLine(ptBeginSave, ptEndSave);

			//求旋转角
			double dtheta1 = GetAngleOfLine(ptBeginSave, ptDown);
			double dtheta2 = GetAngleOfLine(ptBeginSave, ptLast);

			theta2 = dtheta2 - dtheta1;

			if (theta2 == 0)
				return;

			//求ptEndSave绕ptBeginSave旋转theta度后的坐标
			m_ptEnd.x = ptBeginSave.x + r * cos(theta + theta2);
			m_ptEnd.y = ptBeginSave.y + r * sin(theta + theta2);

			//计算并存储所画的圆弧
			if (m_pArc->m_fStartAngle == -1) {//第一次记录圆规的旋转情况
				m_pArc->m_fStartAngle = theta * 180 / pi;
				m_pArc->m_dCurrentTheta = dtheta1;
			}
			else {
				double lastTheta = m_pArc->m_dCurrentTheta;
				double curTheta = dtheta2;

				if (curTheta - lastTheta < -5)
					curTheta = curTheta + pi * 2;
				if (curTheta - lastTheta > 5)
					curTheta = curTheta - pi * 2;

				REAL sweepAngle = (curTheta - dtheta1) * 180 / pi;

				if (sweepAngle > m_pArc->m_fSweepAngle1)
					m_pArc->m_fSweepAngle1 = sweepAngle;

				if (sweepAngle < m_pArc->m_fSweepAngle2)
					m_pArc->m_fSweepAngle2 = sweepAngle;

				m_pArc->m_dCurrentTheta = curTheta;
			}
		}
	}
	else if (nHandle == hitMiddle) {//移动区域
		m_ptBegin.x = ptBeginSave.x + ptLast.x - ptDown.x;
		m_ptBegin.y = ptBeginSave.y + ptLast.y - ptDown.y;
		m_ptEnd.x = ptEndSave.x + ptLast.x - ptDown.x;
		m_ptEnd.y = ptEndSave.y + ptLast.y - ptDown.y;
		if (m_nStyle == CRealia::Goniometer) {
			m_ptTmp.x = ptTmpSave.x + ptLast.x - ptDown.x;
			m_ptTmp.y = ptTmpSave.y + ptLast.y - ptDown.y;
		}
	}
	else if (nHandle == hitDrag) {
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

		//保证m_ptBegin.x <= m_ptEnd.x
		if (m_ptBegin.x > m_ptEnd.x) {
			LONG tmp = m_ptBegin.x;
			m_ptBegin.x = m_ptEnd.x;
			m_ptEnd.x = tmp;
			tmp = m_ptBegin.y;
			m_ptBegin.y = m_ptEnd.y;
			m_ptEnd.y = tmp;
		}
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
	else if (m_nStyle == CRealia::Compass) {
		POINT pt3, pt4, pt5, pt6, pt7, pt8, pt9;
		GetCompassPoints(m_ptBegin, m_ptEnd, &pt3, &pt4, &pt5, &pt6, &pt7, &pt8, &pt9);

		POINT pts[4], ptsRight[3];
		EqualPoint(&pts[0], &m_ptBegin);
		EqualPoint(&pts[1], &pt3);
		EqualPoint(&pts[2], &pt5);
		EqualPoint(&pts[3], &pt4);
		::DeleteObject(m_rgn);
		m_rgn = CreatePolygonRgn(pts, 4, WINDING);

		EqualPoint(&ptsRight[0], &m_ptEnd);
		EqualPoint(&ptsRight[1], &pt3);
		EqualPoint(&ptsRight[2], &pt5);
		::DeleteObject(m_rgnTopRight);
		m_rgnTopRight = CreatePolygonRgn(ptsRight, 3, WINDING);
		return;
	}
	::DeleteObject(m_rgnTopRight);
	m_rgnTopRight = CreateEllipticRgn(ptEnd.x - rDrag, ptEnd.y - rDrag, ptEnd.x + rDrag, ptEnd.y + rDrag);
}

//画直尺：5像素为1毫米，固定直尺总宽度为60像素
//普通刻度10像素长，5的倍数20像素长，10的倍数25像素长
void CRealia::DrawRuler(Graphics* pGraphics, POINT pt1, POINT pt2, int iHeight) const
{
	Pen penBlue(Color(0, 0, 255), 2);

	POINT pt3, pt4;
	double px = pt2.x - pt1.x;
	double py = pt2.y - pt1.y;
	double pl = sqrt(px * px + py * py);
	double theta = atan(py / px);
	pt3.x = pt2.x - iHeight * sin(theta);
	pt3.y = pt2.y + iHeight * cos(theta);
	pt4.x = pt1.x - iHeight * sin(theta);
	pt4.y = pt1.y + iHeight * cos(theta);
	pGraphics->DrawLine(&penBlue, POINTTOPoint(pt1), POINTTOPoint(pt2));
	pGraphics->DrawLine(&penBlue, POINTTOPoint(pt2), POINTTOPoint(pt3));
	pGraphics->DrawLine(&penBlue, POINTTOPoint(pt3), POINTTOPoint(pt4));
	pGraphics->DrawLine(&penBlue, POINTTOPoint(pt4), POINTTOPoint(pt1));

	pGraphics->DrawEllipse(&penBlue, Rect(pt2.x - rDrag, pt2.y - rDrag, rDrag * 2, rDrag * 2));

	Pen penGray(Color(128, 128, 128), 1);//刻度线
	Font font(pFont, iFontSize);
	SolidBrush brush(Color(0, 0, 255));

	int count = pl / 5;
	int j = 0;
	Point tmpBegin, tmpEnd;
	for (int i = 0; i < count - 2; i++) {
		j = i + 1;//为了下面的计算少写一个括号
		tmpBegin = Point(pt1.x + 5 * j * cos(theta), pt1.y + 5 * j * sin(theta));
		if (i % 10 == 0) {
			tmpEnd = Point(pt1.x + 5 * j * cos(theta) - 25 * sin(theta), pt1.y + 5 * j * sin(theta) + 25 * cos(theta));
			TCHAR mark[8];
			wsprintf(mark, _T("%d"), i / 10);
			pGraphics->DrawString(mark, lstrlen(mark), &font, PointF(tmpEnd.X - 3, tmpEnd.Y), &brush);
		}
		else if (i % 5 == 0) {
			tmpEnd = Point(pt1.x + 5 * j * cos(theta) - 20 * sin(theta), pt1.y + 5 * j * sin(theta) + 20 * cos(theta));
		}
		else {
			tmpEnd = Point(pt1.x + 5 * j * cos(theta) - 10 * sin(theta), pt1.y + 5 * j * sin(theta) + 10 * cos(theta));
		}
		pGraphics->DrawLine(&penGray, tmpBegin, tmpEnd);
	}
}

//画三角形
//当angle = 30度，画出来的就是锐角三角形
//当angle = 45度，画出来的就是等腰三角形
void CRealia::DrawTriangle(Graphics* pGraphics, POINT pt1, POINT pt2, int angle) const
{
	assert(angle >= 0);
	assert(angle <= 90);
	Pen penBlue(Color(0, 0, 255), 2);

	POINT pt3;
	double px = pt2.x - pt1.x;
	double py = pt2.y - pt1.y;
	double pl = sqrt(px * px + py * py);
	double theta = atan(py / px);
	double ptheta = angle * pi / 180;
	pt3.x = pt1.x - pl * tan(ptheta) * sin(theta);
	pt3.y = pt1.y + pl * tan(ptheta) * cos(theta);
	pGraphics->DrawLine(&penBlue, POINTTOPoint(pt1), POINTTOPoint(pt2));
	pGraphics->DrawLine(&penBlue, POINTTOPoint(pt2), POINTTOPoint(pt3));
	pGraphics->DrawLine(&penBlue, POINTTOPoint(pt3), POINTTOPoint(pt1));

	pGraphics->DrawEllipse(&penBlue, Rect(pt2.x - rDrag, pt2.y - rDrag, rDrag * 2, rDrag * 2));

	Pen penGray(Color(128, 128, 128), 1);//刻度线
	Font font(pFont, iFontSize);
	SolidBrush brush(Color(0, 0, 255));

	int count = pl / 5;
	int j = 0;
	Point tmpBegin, tmpEnd;
	for (int i = 0; i < count - 5; i++) {
		j = i + 1;//为了下面的计算少写一个括号
		tmpBegin = Point(pt1.x + 5 * j * cos(theta), pt1.y + 5 * j * sin(theta));
		if (i % 10 == 0) {
			tmpEnd = Point(pt1.x + 5 * j * cos(theta) - 25 * sin(theta), pt1.y + 5 * j * sin(theta) + 25 * cos(theta));
			TCHAR mark[8];
			wsprintf(mark, _T("%d"), i / 10);
			pGraphics->DrawString(mark, lstrlen(mark), &font, PointF(tmpEnd.X - 3, tmpEnd.Y + 2), &brush);
		}
		else if (i % 5 == 0) {
			tmpEnd = Point(pt1.x + 5 * j * cos(theta) - 20 * sin(theta), pt1.y + 5 * j * sin(theta) + 20 * cos(theta));
		}
		else {
			tmpEnd = Point(pt1.x + 5 * j * cos(theta) - 10 * sin(theta), pt1.y + 5 * j * sin(theta) + 10 * cos(theta));
		}
		pGraphics->DrawLine(&penGray, tmpBegin, tmpEnd);
	}

	count = pl * tan(ptheta) / 5;
	for (int i = 0; i < count - 5; i++) {
		j = i + 1;//为了下面的计算少写一个括号
		tmpBegin = Point(pt1.x - 5 * j * sin(theta), pt1.y + 5 * j * cos(theta));
		if (i % 10 == 0) {
			tmpEnd = Point(pt1.x - 5 * j * sin(theta) + 25 * cos(theta), pt1.y + 5 * j * cos(theta) + 25 * sin(theta));
			TCHAR mark[8];
			wsprintf(mark, _T("%d"), i / 10);
			pGraphics->DrawString(mark, lstrlen(mark), &font, PointF(tmpEnd.X + 3, tmpEnd.Y - 4), &brush);
		}
		else if (i % 5 == 0) {
			tmpEnd = Point(pt1.x - 5 * j * sin(theta) + 20 * cos(theta), pt1.y + 5 * j * cos(theta) + 20 * sin(theta));
		}
		else {
			tmpEnd = Point(pt1.x - 5 * j * sin(theta) + 10 * cos(theta), pt1.y + 5 * j * cos(theta) + 10 * sin(theta));
		}
		pGraphics->DrawLine(&penGray, tmpBegin, tmpEnd);
	}
}

//画量角器
//普通刻度10像素长，5的倍数20像素长，10的倍数25像素长
void CRealia::DrawProtractor(Graphics* pGraphics, POINT pt1, POINT pt2) const
{
	Pen penBlue(Color(0, 0, 255), 2);

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
	REAL startAngle, sweepAngle;
	startAngle = (theta + pi) * 180 / pi;
	sweepAngle = 180;
	pGraphics->DrawArc(&penBlue, Rect(ptLeftTop.x, ptLeftTop.y, r * 2, r * 2), startAngle, sweepAngle);
	//Arc函数从起点至终点逆时针画弧
	//Arc(dc, ptLeftTop.x, ptLeftTop.y, ptRightBottom.x, ptRightBottom.y, pt2.x, pt2.y, pt1.x, pt1.y);//上半圆

	//在量角器的圆心处画一个准心
	pGraphics->DrawEllipse(&penBlue, Rect(ptCenter.x - rDrag, ptCenter.y - rDrag, rDrag * 2, rDrag * 2));

	//画量角器下面的直尺
	DrawRuler(pGraphics, pt1, pt2, m_iHeight);

	Pen penGray(Color(128, 128, 128), 1);//刻度线
	Font font(pFont, iFontSize);
	SolidBrush brush(Color(0, 0, 255));

	int count = 180;
	double rad = pi / 180;
	Point tmpBegin, tmpEnd;
	for (int i = 0; i <= count; i++) {
		tmpBegin = Point(ptCenter.x - r * cos(rad * i + theta), ptCenter.y - r * sin(rad * i + theta));
		if (i % 10 == 0) {
			tmpEnd = Point(ptCenter.x - (r - 25) * cos(rad * i + theta), ptCenter.y - (r - 25) * sin(rad * i + theta));
			TCHAR mark[8];
			wsprintf(mark, _T("%d"), 180 - i);
			pGraphics->DrawString(mark, lstrlen(mark), &font, PointF(tmpEnd.X, tmpEnd.Y), &brush);
		}
		else if (i % 5 == 0) {
			tmpEnd = Point(ptCenter.x - (r - 20) * cos(rad * i + theta), ptCenter.y - (r - 20) * sin(rad * i + theta));
		}
		else {
			tmpEnd = Point(ptCenter.x - (r - 10) * cos(rad * i + theta), ptCenter.y - (r - 10) * sin(rad * i + theta));
		}
		pGraphics->DrawLine(&penGray, tmpBegin, tmpEnd);
	}
}

//画角度尺
//余弦定理a^2 = b^2 + c^2 - 2bc * cosA
//cosA = (b^2 + c^2 - a^2) / (2bc)
void CRealia::DrawGoniometer(Graphics* pGraphics, POINT pt1, POINT pt2, POINT pt3) const
{
	Pen penBlue(Color(0, 0, 255), 2);

	if (pt3.x == 0 && pt3.y == 0) {
		pGraphics->DrawLine(&penBlue, POINTTOPoint(pt1), POINTTOPoint(pt2));
	}
	else {
		pGraphics->DrawLine(&penBlue, POINTTOPoint(pt1), POINTTOPoint(pt3));
		pGraphics->DrawLine(&penBlue, POINTTOPoint(pt3), POINTTOPoint(pt2));

		pGraphics->DrawEllipse(&penBlue, Rect(pt1.x - rDrag, pt1.y - rDrag, rDrag * 2, rDrag * 2));
		pGraphics->DrawEllipse(&penBlue, Rect(pt2.x - rDrag, pt2.y - rDrag, rDrag * 2, rDrag * 2));

		double theta1 = GetAngleOfLine(pt3, pt1);
		double theta2 = GetAngleOfLine(pt3, pt2);
		double theta = theta2 - theta1;
		if (theta < 0)
			theta = theta + pi * 2;

		//计算圆的外接正方形的左上角和右下角的坐标
		POINT ptLeftTop, ptRightBottom;
		ptLeftTop.x = pt3.x - rGoniometer;
		ptLeftTop.y = pt3.y - rGoniometer;
		ptRightBottom.x = pt3.x + rGoniometer;
		ptRightBottom.y = pt3.y + rGoniometer;
		REAL startAngle, sweepAngle;
		startAngle = theta1 * 180 / pi;
		sweepAngle = theta * 180 / pi;
		pGraphics->DrawArc(&penBlue, Rect(ptLeftTop.x, ptLeftTop.y, rGoniometer * 2, rGoniometer * 2), startAngle, sweepAngle);
		//Arc函数从起点至终点逆时针画弧
		//Arc(dc, ptLeftTop.x, ptLeftTop.y, ptRightBottom.x, ptRightBottom.y, pt2.x, pt2.y, pt1.x, pt1.y);

		Font font(pFont, iFontSize);
		SolidBrush brush(Color(0, 0, 255));

		int angle = theta * 180 / pi;
		TCHAR mark[8];
		wsprintf(mark, _T("%d°"), angle);
		pGraphics->DrawString(mark, lstrlen(mark), &font, PointF(pt3.x, pt3.y), &brush);
	}
}

//画圆规
void CRealia::DrawCompass(Graphics* pGraphics, POINT pt1, POINT pt2, int iHeight)
{
	const int iWidth = 30;
	const int iHeight2 = 50;
	//Pen penBlue(Color(0, 0, 255), 2);

	//求各点坐标
	POINT pt3, pt4, pt5, pt6, pt7, pt8, pt9;
	GetCompassPoints(pt1, pt2, &pt3, &pt4, &pt5, &pt6, &pt7, &pt8, &pt9);

	//pGraphics->DrawLine(&penBlue, POINTTOPoint(pt1), POINTTOPoint(pt3));
	//pGraphics->DrawLine(&penBlue, POINTTOPoint(pt3), POINTTOPoint(pt2));

	//pGraphics->DrawLine(&penBlue, POINTTOPoint(pt3), POINTTOPoint(pt4));
	//pGraphics->DrawLine(&penBlue, POINTTOPoint(pt4), POINTTOPoint(pt5));
	//pGraphics->DrawLine(&penBlue, POINTTOPoint(pt5), POINTTOPoint(pt3));

	//pGraphics->DrawLine(&penBlue, POINTTOPoint(pt1), POINTTOPoint(pt4));
	//pGraphics->DrawLine(&penBlue, POINTTOPoint(pt2), POINTTOPoint(pt5));
	//
	//pGraphics->DrawLine(&penBlue, POINTTOPoint(pt6), POINTTOPoint(pt8));
	//pGraphics->DrawLine(&penBlue, POINTTOPoint(pt8), POINTTOPoint(pt9));
	//pGraphics->DrawLine(&penBlue, POINTTOPoint(pt9), POINTTOPoint(pt7));

	//Point ptsLeft[4], ptsRight[3], ptsTop[4];
	Point ptsLeft[4] = { POINTTOPoint(pt1), POINTTOPoint(pt3), POINTTOPoint(pt5), POINTTOPoint(pt4) };
	Point ptsRight[3] = { POINTTOPoint(pt2), POINTTOPoint(pt3), POINTTOPoint(pt5) };
	Point ptsTop[4] = { POINTTOPoint(pt6), POINTTOPoint(pt8), POINTTOPoint(pt9), POINTTOPoint(pt7) };

	//给区域着色
	SolidBrush hbTop(Color(83, 83, 199));
	SolidBrush hbLeft(Color(146, 146, 192));
	SolidBrush hbRight(Color(123, 123, 192));
	
	pGraphics->FillPolygon(&hbTop, ptsTop, 4);
	pGraphics->FillPolygon(&hbLeft, ptsLeft, 4);
	pGraphics->FillPolygon(&hbRight, ptsRight, 3);
}

//点的位置如下
//pt1,pt2,pt3为圆规的框架，其余点均为画饱满图形所用
//pt1,pt2,pt3三点组成一个等腰三角形，腰长m_iHeight
//pt3,pt4,pt5为边长iWidth的等边三角形
//pt6,pt7,pt8,pt9是长iWidth / 3，宽iHeight的矩形
//以pt1为轴心点转动，pt2经过的路线即为圆规画出来的图形
/*
          pt8 - pt9
           |     |
           |     |
           |     |
    pt4 - pt6 - pt7 - pt5
       \             /
        \           /
         \         /
          \       /
           \     /
            \   /
             pt3
            /   \
           /     \
          /       \
         /         \
        /           \
       /             \
      /               \
     /                 \
    /                   \
   /                     \
pt1                       pt2
*/
void CRealia::GetCompassPoints(POINT pt1, POINT pt2, LPPOINT lppt3, LPPOINT lppt4, LPPOINT lppt5, LPPOINT lppt6, LPPOINT lppt7, LPPOINT lppt8, LPPOINT lppt9)
{
	const int iWidth = 30;
	const int iHeight2 = 50;
	
	double px = pt2.x - pt1.x;
	double py = pt2.y - pt1.y;
	double pl = sqrt(px * px + py * py);
	if (pl >= lCompass * 2)
		m_iHeight = pl / 2 + 10;
	else
		m_iHeight = lCompass;
	double theta1 = acos((pl * pl) / (2 * pl * m_iHeight));
	double theta2 = atan(py / px);
	if (px >= 0) {
		lppt3->x = pt1.x + m_iHeight * cos(theta1 - theta2);
		lppt3->y = pt1.y - m_iHeight * sin(theta1 - theta2);
	}
	else {
		lppt3->x = pt1.x - m_iHeight * cos(theta1 - theta2);
		lppt3->y = pt1.y + m_iHeight * sin(theta1 - theta2);
	}

	if (px >= 0) {
		lppt4->x = lppt3->x - iWidth * sin(pi / 2 - pi / 180 * 60 - theta2);
		lppt4->y = lppt3->y - iWidth * cos(pi / 2 - pi / 180 * 60 - theta2);
		lppt5->x = lppt3->x + iWidth * cos(pi / 180 * 60 - theta2);
		lppt5->y = lppt3->y - iWidth * sin(pi / 180 * 60 - theta2);
	}
	else {
		lppt4->x = lppt3->x + iWidth * sin(pi / 2 - pi / 180 * 60 - theta2);
		lppt4->y = lppt3->y + iWidth * cos(pi / 2 - pi / 180 * 60 - theta2);
		lppt5->x = lppt3->x - iWidth * cos(pi / 180 * 60 - theta2);
		lppt5->y = lppt3->y + iWidth * sin(pi / 180 * 60 - theta2);
	}

	if (px >= 0) {
		lppt6->x = lppt4->x + iWidth / 3 * cos(theta2);
		lppt6->y = lppt4->y + iWidth / 3 * sin(theta2);
		lppt7->x = lppt4->x + iWidth / 3 * 2 * cos(theta2);
		lppt7->y = lppt4->y + iWidth / 3 * 2 * sin(theta2);
	}
	else {
		lppt6->x = lppt4->x - iWidth / 3 * cos(theta2);
		lppt6->y = lppt4->y - iWidth / 3 * sin(theta2);
		lppt7->x = lppt4->x - iWidth / 3 * 2 * cos(theta2);
		lppt7->y = lppt4->y - iWidth / 3 * 2 * sin(theta2);
	}

	if (px >= 0) {
		lppt8->x = lppt6->x + iHeight2 * sin(theta2);
		lppt8->y = lppt6->y - iHeight2 * cos(theta2);
		lppt9->x = lppt7->x + iHeight2 * sin(theta2);
		lppt9->y = lppt7->y - iHeight2 * cos(theta2);
	}
	else {
		lppt8->x = lppt6->x - iHeight2 * sin(theta2);
		lppt8->y = lppt6->y + iHeight2 * cos(theta2);
		lppt9->x = lppt7->x - iHeight2 * sin(theta2);
		lppt9->y = lppt7->y + iHeight2 * cos(theta2);
	}
}

//画圆弧
void CRealia::DrawArc(Graphics* pGraphics)
{
	if (m_vecArcs.size() <= 0)
		return;

	Pen penBlue(Color(0, 0, 255), 2);

	POINT ptLeftTop;
	double r;
	REAL startAngle, sweepAngle;
	for (std::vector<CArc>::iterator it = m_vecArcs.begin(); it != m_vecArcs.end(); it++) {
		ptLeftTop.x = it->m_ptCenter.x - it->m_iRadius;
		ptLeftTop.y = it->m_ptCenter.y - it->m_iRadius;
		r = it->m_iRadius;
		startAngle = it->m_fStartAngle;
		sweepAngle = it->m_fSweepAngle1;
		pGraphics->DrawArc(&penBlue, Rect(ptLeftTop.x, ptLeftTop.y, r * 2, r * 2), startAngle, sweepAngle);
		sweepAngle = it->m_fSweepAngle2;
		pGraphics->DrawArc(&penBlue, Rect(ptLeftTop.x, ptLeftTop.y, r * 2, r * 2), startAngle, sweepAngle);
	}
}
