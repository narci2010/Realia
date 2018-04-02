#pragma once

static inline BOOL IsPointEqual(POINT pt1, POINT pt2)
{
	if (pt1.x == pt2.x && pt1.y == pt2.y)
		return TRUE;
	else
		return FALSE;
}

//把一个点的坐标赋给另一个点
static inline BOOL EqualPoint(LPPOINT lpptDest, LPPOINT lpptSrc)
{
	(*lpptDest).x = (*lpptSrc).x;
	(*lpptDest).y = (*lpptSrc).y;
	return TRUE;
}

class CRealia
{
public:
	CRealia();
	CRealia(POINT ptBegin, POINT ptEnd, UINT nStyle);
	CRealia(const CRealia &other);//拷贝构造函数

	// Style Flags
	enum StyleFlags
	{
		Ruler = 1, AcuteTriangle = 2, IsoscelesTriangle = 3,
		Protractor = 4, Goniometer = 5, Compass = 6,
	};

	// Hit-Test codes
	enum TrackerHit
	{
		hitNothing = -1,
		hitTopLeft = 0, hitTopRight = 1, hitBottomRight = 2, hitBottomLeft = 3,
		hitTop = 4, hitRight = 5, hitBottom = 6, hitLeft = 7, hitMiddle = 8,
		hitDrag = 9
	};

	// Attributes
	UINT m_nStyle;
	HRGN m_rgn;
	POINT m_ptBegin;
	POINT m_ptEnd;
	POINT m_ptTmp;//中间点，用于角度尺和圆规
	BOOL m_bSelect;

	void Draw(HDC pDC) const;
	int HitTest(POINT pt);
	BOOL SetCursor(HWND pWnd, UINT nHitTest);
	BOOL Track(HWND pWnd, POINT point, BOOL bAllowInvert = FALSE,
		HWND pWndClipTo = NULL);
	BOOL TrackRubberBand(HWND pWnd, POINT point, BOOL bAllowInvert = TRUE);

	BOOL IsRegionNull() const;

public:
	virtual ~CRealia();

protected:
	BOOL m_bAllowInvert;
	UINT m_iHeight;//用于直尺的高度
	UINT m_iAngle;//用于画三角形时的角度，如30°,45°,60°

	void Construct();
	BOOL TrackHandle(int nHandle, HWND pWnd, POINT point, HWND pWndClipTo);
	void ModifyPointers(int nHandle, POINT ptBeginSave, POINT ptEndSave, POINT ptTmpSave, POINT ptDown, POINT ptLast);
	void AdjustRgn(int nHandle, POINT ptBegin, POINT ptEnd);

private:
	void DrawRuler(HDC dc, POINT pt1, POINT pt2, const int iHeight = 50) const;
	void DrawTriangle(HDC dc, POINT pt1, POINT pt2, int angle) const;
	void DrawProtractor(HDC dc, POINT pt1, POINT pt2) const;
	void DrawGoniometer(HDC dc, POINT pt1, POINT pt2, POINT pt3) const;
	void DrawCompass(HDC dc, POINT pt1, POINT pt2, int angle) const;
};
