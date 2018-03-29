#include "Ruler.h"
#include <assert.h>
#include <cstddef>
#include <stdlib.h>

#define AFX_STATIC_DATA extern __declspec(selectany)

AFX_STATIC_DATA HCURSOR _afxCursors[10] = { 0, };
AFX_STATIC_DATA HBRUSH _afxHatchBrush = 0;
AFX_STATIC_DATA HPEN _afxBlackDottedPen = 0;
AFX_STATIC_DATA int _afxHandleSize = 0;

// the struct below is used to determine the qualities of a particular handle
struct AFX_HANDLEINFO
{
	size_t nOffsetX;    // offset within RECT for X coordinate
	size_t nOffsetY;    // offset within RECT for Y coordinate
	int nCenterX;       // adjust X by Width()/2 * this number
	int nCenterY;       // adjust Y by Height()/2 * this number
	int nHandleX;       // adjust X by handle size * this number
	int nHandleY;       // adjust Y by handle size * this number
	int nInvertX;       // handle converts to this when X inverted
	int nInvertY;       // handle converts to this when Y inverted
};

// this array describes all 8 handles (clock-wise)
AFX_STATIC_DATA const AFX_HANDLEINFO _afxHandleInfo[] =
{
	// corner handles (top-left, top-right, bottom-right, bottom-left
	{ offsetof(RECT, left), offsetof(RECT, top),        0, 0,  0,  0, 1, 3 },
	{ offsetof(RECT, right), offsetof(RECT, top),       0, 0, -1,  0, 0, 2 },
	{ offsetof(RECT, right), offsetof(RECT, bottom),    0, 0, -1, -1, 3, 1 },
	{ offsetof(RECT, left), offsetof(RECT, bottom),     0, 0,  0, -1, 2, 0 },

	// side handles (top, right, bottom, left)
	{ offsetof(RECT, left), offsetof(RECT, top),        1, 0,  0,  0, 4, 6 },
	{ offsetof(RECT, right), offsetof(RECT, top),       0, 1, -1,  0, 7, 5 },
	{ offsetof(RECT, left), offsetof(RECT, bottom),     1, 0,  0, -1, 6, 4 },
	{ offsetof(RECT, left), offsetof(RECT, top),        0, 1,  0,  0, 5, 7 }
};

// the struct below gives us information on the layout of a RECT struct and
//  the relationship between its members
struct AFX_RECTINFO
{
	size_t nOffsetAcross;   // offset of opposite point (ie. left->right)
	int nSignAcross;        // sign relative to that point (ie. add/subtract)
};

// this array is indexed by the offset of the RECT member / sizeof(int)
AFX_STATIC_DATA const AFX_RECTINFO _afxRectInfo[] =
{
	{ offsetof(RECT, right), +1 },
	{ offsetof(RECT, bottom), +1 },
	{ offsetof(RECT, left), -1 },
	{ offsetof(RECT, top), -1 },
};

CRuler::CRuler()
{
	Construct();
}

CRuler::~CRuler()
{
	DeleteObject((HGDIOBJ*)&_afxHatchBrush);
	DeleteObject((HGDIOBJ*)&_afxBlackDottedPen);
}

CRuler::CRuler(LPPOINT lpptBegin, LPPOINT lpptEnd, UINT nStyle)
{
	Construct();
	m_ptBegin = *lpptBegin;
	m_ptEnd = *lpptEnd;
	m_nStyle = nStyle;
}

void CRuler::Construct()
{
	// do one-time initialization if necessary
	//AfxLockGlobals(CRIT_RECTTRACKER);
	static BOOL bInitialized;
	if (!bInitialized)
	{

		if (_afxHatchBrush == NULL)
		{
			// create the hatch pattern + bitmap
			WORD hatchPattern[8];
			WORD wPattern = 0x1111;
			for (int i = 0; i < 4; i++)
			{
				hatchPattern[i] = wPattern;
				hatchPattern[i + 4] = wPattern;
				wPattern <<= 1;
			}
			HBITMAP hatchBitmap = CreateBitmap(8, 8, 1, 1, hatchPattern);
			if (hatchBitmap == NULL)
			{
				//AfxUnlockGlobals(CRIT_RECTTRACKER);
				//AfxThrowResourceException();
			}

			// create black hatched brush
			_afxHatchBrush = CreatePatternBrush(hatchBitmap);
			DeleteObject(hatchBitmap);
			if (_afxHatchBrush == NULL)
			{
				//AfxUnlockGlobals(CRIT_RECTTRACKER);
				//AfxThrowResourceException();
			}
		}

		if (_afxBlackDottedPen == NULL)
		{
			// create black dotted pen
			_afxBlackDottedPen = CreatePen(PS_DOT, 0, RGB(0, 0, 0));
			if (_afxBlackDottedPen == NULL)
			{
				//AfxUnlockGlobals(CRIT_RECTTRACKER);
				//AfxThrowResourceException();
			}
		}

		// Note: all track cursors must live in same module
		//HINSTANCE hInst = AfxFindResourceHandle(
		//	ATL_MAKEINTRESOURCE(AFX_IDC_TRACK4WAY), ATL_RT_GROUP_CURSOR);

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
		_afxCursors[9] = ::LoadCursorW(NULL, IDC_SIZEALL);

		// get default handle size from Windows profile setting
		static const TCHAR szWindows[] = _T("windows");
		static const TCHAR szInplaceBorderWidth[] =
			_T("oleinplaceborderwidth");
		_afxHandleSize = GetProfileInt(szWindows, szInplaceBorderWidth, 4);
		bInitialized = TRUE;
	}

	m_nStyle = 0;
	//m_nHandleSize = _afxHandleSize;
	//m_sizeMin.cy = m_sizeMin.cx = m_nHandleSize * 2;

	m_rgn = CreateRectRgn(0, 0, 0, 0);
	m_ptBegin = m_ptEnd = { 0,0 };
	m_bErase = FALSE;
	m_bFinalErase = FALSE;
}

void CRuler::Draw(HDC pDC) const
{
	// set initial DC state
	assert(SaveDC(pDC) != 0);
	SetMapMode(pDC, MM_TEXT);
	//pDC->SetViewportOrg(0, 0);
	//pDC->SetWindowOrg(0, 0);

	//画内部区域
	DrawRuler(pDC, m_ptBegin, m_ptEnd);

	assert(RestoreDC(pDC, -1));
}

int CRuler::HitTest(POINT point) const
{
	TrackerHit hitResult = hitNothing;

	assert(m_ptBegin.x <= m_ptEnd.x);
	hitResult = (TrackerHit)HitTestHandles(point);

	return hitResult;
}

BOOL CRuler::SetCursor(HWND pWnd, UINT nHitTest) const
{
	// trackers should only be in client area
	if (nHitTest != HTCLIENT)
		return FALSE;

	// convert cursor position to client co-ordinates
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(pWnd, &point);

	// do hittest and normalize hit
	int nHandle = HitTestHandles(point);
	if (nHandle < 0)
		return FALSE;

	// need to normalize the hittest such that we get proper cursors
	nHandle = NormalizeHit(nHandle);

	// handle special case of hitting area between handles
	//  (logically the same -- handled as a move -- but different cursor)
	if (nHandle == hitMiddle && !PtInRegion(m_rgn, point.x, point.y))
	{
		// only for trackers with hatchedBorder (ie. in-place resizing)
		if (m_nStyle & hatchedBorder)
			nHandle = (TrackerHit)9;
	}

	assert(nHandle < _countof(_afxCursors));
	::SetCursor(_afxCursors[nHandle]);
	return TRUE;
}

BOOL CRuler::Track(HWND pWnd, POINT point, BOOL bAllowInvert,
	HWND pWndClipTo)
{
	// perform hit testing on the handles
	int nHandle = HitTestHandles(point);
	if (nHandle < 0)
	{
		// didn't hit a handle, so just return FALSE
		return FALSE;
	}

	// otherwise, call helper function to do the tracking
	m_bAllowInvert = bAllowInvert;
	return TrackHandle(nHandle, pWnd, point, pWndClipTo);
}

BOOL CRuler::TrackRubberBand(HWND pWnd, POINT point, BOOL bAllowInvert)
{
	// simply call helper function to track from bottom right handle
	m_bAllowInvert = bAllowInvert;
	m_ptBegin = m_ptEnd = point;
	m_rgn = CreateRectRgn(0, 0, 0, 0);
	return TrackHandle(hitBottomRight, pWnd, point, NULL);
}

int CRuler::NormalizeHit(int nHandle) const
{
	assert(nHandle <= 8 && nHandle >= -1);
	if (nHandle == hitMiddle || nHandle == hitNothing)
		return nHandle;
	assert(0 <= nHandle && nHandle < _countof(_afxHandleInfo));
	const AFX_HANDLEINFO* pHandleInfo = &_afxHandleInfo[nHandle];
	if (m_ptEnd.x - m_ptBegin.x < 0)
	{
		nHandle = (TrackerHit)pHandleInfo->nInvertX;
		assert(0 <= nHandle && nHandle < _countof(_afxHandleInfo));
		pHandleInfo = &_afxHandleInfo[nHandle];
	}
	if (m_ptEnd.y - m_ptBegin.y < 0)
		nHandle = (TrackerHit)pHandleInfo->nInvertY;
	return nHandle;
}

int CRuler::HitTestHandles(POINT point) const
{
	HRGN rgn = CreateEllipticRgn(m_ptEnd.x - 5, m_ptEnd.y - 5, m_ptEnd.x + 5, m_ptEnd.y + 5);
	if (PtInRegion(rgn, point.x, point.y)) {
		return hitTopRight;
	}
	else if (PtInRegion(m_rgn, point.x, point.y)) {
		return hitMiddle;
	}
	else
		return hitNothing;
}

BOOL CRuler::TrackHandle(int nHandle, HWND pWnd, POINT point,
	HWND pWndClipTo)
{
	assert(nHandle >= 0);
	assert(nHandle <= 8);   // handle 8 is inside the rect

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
	POINT ptBeginSave = m_ptBegin;
	POINT ptEndSave = m_ptEnd;

	// save original width & height in pixels
	int nWidth = m_ptEnd.x - m_ptBegin.x;
	int nHeight = m_ptEnd.y - m_ptBegin.y;

	// find out what x/y coords we are supposed to modify
	int *px, *py;
	int xDiff, yDiff;
	GetModifyPointers(nHandle, &px, &py, &xDiff, &yDiff);
	xDiff = point.x - xDiff;
	yDiff = point.y - yDiff;

	// get DC for drawing
	HDC pDrawDC;
	if (pWndClipTo != NULL)
	{
		// clip to arbitrary window by using adjusted Window DC
		pDrawDC = GetDCEx(pWndClipTo, NULL, DCX_CACHE);
	}
	else
	{
		// otherwise, just use normal DC
		pDrawDC = GetDC(pWnd);
	}
	//assert_VALID(pDrawDC);

	POINT ptBeginOld, ptEndOld;
	BOOL bMoved = FALSE;

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
			ptBeginOld = m_ptBegin;
			ptEndOld = m_ptEnd;
			// handle resize cases (and part of move)
			if (px != NULL)
				*px = GET_X_LPARAM(msg.lParam) - xDiff;
			if (py != NULL)
				*py = GET_Y_LPARAM(msg.lParam) - yDiff;

			// handle move case
			if (nHandle == hitMiddle)
			{
				m_ptEnd.x = m_ptEnd.x + nWidth;
				m_ptEnd.y = m_ptEnd.y + nHeight;
			}
			// allow caller to adjust the rectangle if necessary
			AdjustRgn(nHandle, &m_ptBegin, &m_ptEnd);

			// only redraw and callback if the rect actually changed!
			m_bFinalErase = (msg.message == WM_LBUTTONUP);
			if (!IsPointEqual(&ptBeginOld, &m_ptBegin) || !IsPointEqual(&ptEndOld, &m_ptEnd) || m_bFinalErase)
			{
				if (bMoved)
				{
					m_bErase = TRUE;
					//DrawTrackerRect(&rectOld, pWndClipTo, pDrawDC, pWnd);
				}
				//OnChangedRect(rectOld);
				if (msg.message != WM_LBUTTONUP)
					bMoved = TRUE;
			}
			if (m_bFinalErase)
				goto ExitLoop;

			if (!IsPointEqual(&ptBeginOld, &m_ptBegin) || !IsPointEqual(&ptEndOld, &m_ptEnd))
			{
				m_bErase = FALSE;
				//DrawTrackerRect(&m_rect, pWndClipTo, pDrawDC, pWnd);
			}
			InvalidateRect(pWnd, NULL, false);
			break;

			// handle cancel messages
		case WM_KEYDOWN:
			if (msg.wParam != VK_ESCAPE)
				break;
		case WM_RBUTTONDOWN:
			if (bMoved)
			{
				m_bErase = m_bFinalErase = TRUE;
				//DrawTrackerRect(&m_rect, pWndClipTo, pDrawDC, pWnd);
			}
			m_ptBegin = ptBeginSave;
			m_ptEnd = ptEndSave;
			goto ExitLoop;

			// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

ExitLoop:
	if (pWndClipTo != NULL)
		ReleaseDC(pWndClipTo, pDrawDC);
	else
		ReleaseDC(pWnd, pDrawDC);
	ReleaseCapture();

	//AfxUnlockTempMaps(FALSE);

	// restore rect in case bMoved is still FALSE
	if (!bMoved) {
		m_ptBegin = ptBeginSave;
		m_ptEnd = ptEndSave;
	}
	m_bFinalErase = FALSE;
	m_bErase = FALSE;

	// return TRUE only if rect has changed
	return !IsPointEqual(&ptBeginSave, &m_ptBegin) || !IsPointEqual(&ptEndSave, &m_ptEnd);
}

BOOL CRuler::IsRegionNull()
{
	return IsPointEqual(&m_ptBegin, &m_ptEnd);
}

void CRuler::GetModifyPointers(
	int nHandle, int** ppx, int** ppy, int* px, int* py)
{
	assert(nHandle >= 0);
	assert(nHandle <= 8);

	if (nHandle == hitMiddle)
		nHandle = hitTopLeft;   // same as hitting top-left

	*ppx = NULL;
	*ppy = NULL;

	// fill in the part of the rect that this handle modifies
	//  (Note: handles that map to themselves along a given axis when that
	//   axis is inverted don't modify the value on that axis)

	const AFX_HANDLEINFO* pHandleInfo = &_afxHandleInfo[nHandle];
	if (pHandleInfo->nInvertX != nHandle)
	{
		*ppx = (int*)((BYTE*)&m_ptBegin + pHandleInfo->nOffsetX);
		if (px != NULL)
			*px = **ppx;
	}
	else
	{
		// middle handle on X axis
		if (px != NULL)
			*px = m_ptBegin.x + abs(m_ptEnd.x - m_ptBegin.x) / 2;
	}
	if (pHandleInfo->nInvertY != nHandle)
	{
		*ppy = (int*)((BYTE*)&m_ptBegin + pHandleInfo->nOffsetY);
		if (py != NULL)
			*py = **ppy;
	}
	else
	{
		// middle handle on Y axis
		if (py != NULL)
			*py = m_ptBegin.y + abs(m_ptEnd.y - m_ptBegin.y) / 2;
	}
}

void CRuler::AdjustRgn(int nHandle, LPPOINT lpBegin, LPPOINT lpEnd)
{
	if (nHandle == hitMiddle)
		return;

	// convert the handle into locations within m_rect
	int *px, *py;
	GetModifyPointers(nHandle, &px, &py, NULL, NULL);

	//m_rgn = CreateRectRgn(m_ptBegin.x, m_ptBegin.y, m_ptEnd.x, m_ptEnd.y + 50);
	int iHeight = 50;
	POINT pt3, pt4;
	double ptx = m_ptEnd.x - m_ptBegin.x;
	double pty = m_ptEnd.y - m_ptBegin.y;
	double pl = sqrt(ptx * ptx + pty * pty);
	double theta = atan(pty / ptx);
	pt3.x = m_ptEnd.x - iHeight * sin(theta);
	pt3.y = m_ptEnd.y + iHeight * cos(theta);
	pt4.x = m_ptBegin.x - iHeight * sin(theta);
	pt4.y = m_ptBegin.y + iHeight * cos(theta);
	POINT pts[4];
	pts[0] = m_ptBegin;
	pts[1] = m_ptEnd;
	pts[2] = pt3;
	pts[3] = pt4;
	m_rgn = CreatePolygonRgn(pts, 4, WINDING);
}

//画直尺：5像素为1毫米，固定直尺总宽度为60像素
//普通刻度10像素长，5的倍数20像素长，10的倍数25像素长
void CRuler::DrawRuler(HDC dc, POINT pt1, POINT pt2, const int iHeight) const
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
