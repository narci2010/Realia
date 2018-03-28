#pragma once

#include "stdafx.h"

class CRuler
{
public:
	CRuler();
	~CRuler();

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
	RECT m_rect;        // current position (always in pixels)
	SIZE m_sizeMin;    // minimum X and Y size during track operation
	int m_nHandleSize;  // size of resize handles (default from WIN.INI)

						// Operations
	void Draw(HDC* pDC) const;
	//void GetTrueRect(LPRECT lpTrueRect) const;
	//BOOL SetCursor(HWND* pWnd, UINT nHitTest) const;
	//BOOL Track(HWND* pWnd, POINT point, BOOL bAllowInvert = FALSE,
	//	HWND* pWndClipTo = NULL);
	//BOOL TrackRubberBand(HWND* pWnd, POINT point, BOOL bAllowInvert = TRUE);
	//int HitTest(POINT point) const;
	//int NormalizeHit(int nHandle) const;

	//// Overridables
	//virtual void DrawTrackerRect(LPCRECT lpRect, HWND* pWndClipTo,
	//	HDC* pDC, HWND* pWnd);
	//virtual void AdjustRect(int nHandle, LPRECT lpRect);
	//virtual void OnChangedRect(const RECT& rectOld);
	//virtual UINT GetHandleMask() const;

private:
	BOOL m_bAllowInvert;    // flag passed to Track or TrackRubberBand
	RECT m_rectLast;
	RECT m_sizeLast;
	BOOL m_bErase;          // TRUE if DrawTrackerRect is called for erasing
	BOOL m_bFinalErase;     // TRUE if DragTrackerRect called for final erase

							// implementation helpers
	//int HitTestHandles(POINT point) const;
	//void GetHandleRect(int nHandle, RECT* pHandleRect) const;
	//void GetModifyPointers(int nHandle, int**ppx, int**ppy, int* px, int*py);
	//virtual int GetHandleSize(LPCRECT lpRect = NULL) const;
	//BOOL TrackHandle(int nHandle, HWND* pWnd, POINT point, POINT* pWndClipTo);
	//void Construct();
};
