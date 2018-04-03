#pragma once

#include <vector>

//�ж������Ƿ����
static inline BOOL IsPointEqual(POINT pt1, POINT pt2)
{
	if (pt1.x == pt2.x && pt1.y == pt2.y)
		return TRUE;
	else
		return FALSE;
}

//��һ��������긳����һ����
static inline BOOL EqualPoint(LPPOINT lpptDest, LPPOINT lpptSrc)
{
	(*lpptDest).x = (*lpptSrc).x;
	(*lpptDest).y = (*lpptSrc).y;
	return TRUE;
}

//��֪������������루���ɶ���
static inline double DistanceOfTwoPoint(POINT pt1, POINT pt2)
{
	double px = pt2.x - pt1.x;
	double py = pt2.y - pt1.y;
	return sqrt(px * px + py * py);
}

class CRealia
{
public:
	CRealia();
	CRealia(POINT ptBegin, POINT ptEnd, UINT nStyle);
	CRealia(const CRealia &other);//�������캯��

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
	POINT m_ptTmp;//�м�㣬���ڽǶȳߺ�Բ��
	BOOL m_bSelect;

	void Draw(HDC pDC);
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
	UINT m_iHeight;//����ֱ�ߵĸ߶�
	UINT m_iAngle;//���ڻ�������ʱ�ĽǶȣ���30��,45��,60��
	std::vector<POINT> m_vecPoints;

	void Construct();
	BOOL TrackHandle(int nHandle, HWND pWnd, POINT point, HWND pWndClipTo);
	void ModifyPointers(int nHandle, POINT ptBeginSave, POINT ptEndSave, POINT ptTmpSave, POINT ptDown, POINT ptLast);
	void AdjustRgn(int nHandle, POINT ptBegin, POINT ptEnd);

private:
	void DrawRuler(HDC dc, POINT pt1, POINT pt2, const int iHeight = 50) const;
	void DrawTriangle(HDC dc, POINT pt1, POINT pt2, int angle) const;
	void DrawProtractor(HDC dc, POINT pt1, POINT pt2) const;
	void DrawGoniometer(HDC dc, POINT pt1, POINT pt2, POINT pt3) const;
	void DrawCompass(HDC dc, POINT pt1, POINT pt2, const int iHeight);
	void GetCompassPoints(const POINT pt1, const POINT pt2, LPPOINT lppt3, LPPOINT lppt4, LPPOINT lppt5, LPPOINT lppt6, LPPOINT lppt7, LPPOINT lppt8, LPPOINT lppt9);
};

class CArc
{
public:
	CArc()
	{
		m_ptCenter = { 0, 0 };
		m_iRadius = 0;
	}
	CArc(POINT ptCenter, UINT iRadius)
	{
		m_ptCenter.x = ptCenter.x;
		m_ptCenter.y = ptCenter.y;
		m_iRadius = iRadius;
	}
	~CArc()
	{
		m_vecPoints.clear();
	}

	POINT m_ptCenter;//Բ��
	UINT m_iRadius;//�뾶
	std::vector<POINT> m_vecPoints;//Բ���ϵĵ㣬ÿ���㻭һ��
};

extern std::vector<CArc> m_vecArcs;
