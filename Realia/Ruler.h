#pragma once

#include "stdafx.h"

static inline BOOL IsPointEqual(LPPOINT lppt1, LPPOINT lppt2)
{
	if ((*lppt1).x == (*lppt2).x && (*lppt1).y == (*lppt2).y)
		return TRUE;
	else
		return FALSE;
}

class CRuler
{
public:
	CRuler();
	CRuler(LPPOINT lpptBegin, LPPOINT lpptEnd, UINT nStyle);

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
	UINT m_nStyle;
	HRGN m_rgn;
	POINT m_ptBegin;
	POINT m_ptEnd;

	void Draw(HDC pDC) const;
	int HitTest(POINT pt) const;
	BOOL SetCursor(HWND pWnd, UINT nHitTest) const;
	BOOL Track(HWND pWnd, POINT point, BOOL bAllowInvert = FALSE,
		HWND pWndClipTo = NULL);
	BOOL TrackRubberBand(HWND pWnd, POINT point, BOOL bAllowInvert = TRUE);

	BOOL IsRegionNull();
	
public:
	virtual ~CRuler();

protected:
	BOOL m_bErase;
	BOOL m_bFinalErase;

	void Construct();
	int HitTestHandles(POINT point) const;
	BOOL TrackHandle(int nHandle, HWND pWnd, POINT point, HWND pWndClipTo);
	void GetModifyPointers(int nHandle, int**ppx, int**ppy, int* px, int*py);
	virtual void AdjustRect(int nHandle, LPPOINT lpBegin, LPPOINT lpEnd);

private:
	void DrawRuler(HDC dc, POINT pt1, POINT pt2, const int iHeight = 50) const;

};
