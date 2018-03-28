#pragma once

#include "stdafx.h"
#include "atltypes.h"

//CRectTracker使用的MFC宏定义
// AFX_CDECL is used for rare functions taking variable arguments
#ifndef AFX_CDECL
#define AFX_CDECL __cdecl
#endif
#ifndef AFX_STATIC
#define AFX_STATIC extern
#define AFX_STATIC_DATA extern __declspec(selectany)
#endif

BOOL AfxIsValidAddress(const void* lp, UINT nBytes,
	BOOL bReadWrite /* = TRUE */)
{
	// simple version using Win-32 APIs for pointer validation.
	return (lp != NULL && !IsBadReadPtr(lp, nBytes) &&
		(!bReadWrite || !IsBadWritePtr((LPVOID)lp, nBytes)));
}

#define CRIT_RECTTRACKER    5
// MFC Control bar compatibility
#define AFX_CX_BORDER   1
#define AFX_CY_BORDER   1

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#endif

#ifndef __AFX_H__
#if defined _DEBUG
#include <crtdbg.h>

BOOL AfxAssertFailedLine(LPCSTR lpszFileName, int nLine)
{
	// we remove WM_QUIT because if it is in the queue then the message box
	// won't display
	MSG msg;
	BOOL bQuit = PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
	BOOL bResult = _CrtDbgReport(_CRT_ASSERT, lpszFileName, nLine, NULL, NULL);
	if (bQuit)
		PostQuitMessage((int)msg.wParam);
	return bResult;
}

#define ASSERT(f)          DEBUG_ONLY((void) ((f) || !::AfxAssertFailedLine(THIS_FILE, __LINE__) || (AfxDebugBreak(), 0)))
#define VERIFY(f)          ASSERT(f)

#else

#define VERIFY(f)          ((void)(f))

#endif
#endif

class CRectTracker
{
public:
	// Constructors
	CRectTracker();
	CRectTracker(LPCRECT lpSrcRect, UINT nStyle);

	// Style Flags
	enum StyleFlags
	{
		solidLine = 1, dottedLine = 2, hatchedBorder = 4,
		resizeInside = 8, resizeOutside = 16, hatchInside = 32,
	};

	// Hit-Test codes
	enum TrackerHit
	{
		hitNothing = -1,
		hitTopLeft = 0, hitTopRight = 1, hitBottomRight = 2, hitBottomLeft = 3,
		hitTop = 4, hitRight = 5, hitBottom = 6, hitLeft = 7, hitMiddle = 8
	};

	// Attributes
	UINT m_nStyle;      // current state
	CRect m_rect;       // current position (always in pixels)
	CSize m_sizeMin;    // minimum X and Y size during track operation
	int m_nHandleSize;  // size of resize handles (default from WIN.INI)

						// Operations
	void Draw(HDC pDC) const;
	void GetTrueRect(LPRECT lpTrueRect) const;
	BOOL SetCursor(HWND pWnd, UINT nHitTest) const;
	BOOL Track(HWND pWnd, CPoint point, BOOL bAllowInvert = FALSE,
		HWND pWndClipTo = NULL);
	BOOL TrackRubberBand(HWND pWnd, CPoint point, BOOL bAllowInvert = TRUE);
	int HitTest(CPoint point) const;
	int NormalizeHit(int nHandle) const;

	// Overridables
	//virtual void DrawTrackerRect(LPCRECT lpRect, HWND pWndClipTo,
	//	HDC pDC, HWND pWnd);
	virtual void AdjustRect(int nHandle, LPRECT lpRect);
	virtual void OnChangedRect(const CRect& rectOld);
	virtual UINT GetHandleMask() const;

	// Implementation
public:
	virtual ~CRectTracker();

protected:
	BOOL m_bAllowInvert;    // flag passed to Track or TrackRubberBand
	CRect m_rectLast;
	CSize m_sizeLast;
	BOOL m_bErase;          // TRUE if DrawTrackerRect is called for erasing
	BOOL m_bFinalErase;     // TRUE if DragTrackerRect called for final erase

							// implementation helpers
	int HitTestHandles(CPoint point) const;
	void GetHandleRect(int nHandle, CRect* pHandleRect) const;
	void GetModifyPointers(int nHandle, int**ppx, int**ppy, int* px, int*py);
	virtual int GetHandleSize(LPCRECT lpRect = NULL) const;
	BOOL TrackHandle(int nHandle, HWND pWnd, CPoint point, HWND pWndClipTo);
	void Construct();
};

