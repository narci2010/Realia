#pragma once

#include "stdafx.h"
#include <math.h>
#include <vector>

#define AFX_STATIC_DATA extern __declspec(selectany)

AFX_STATIC_DATA double pi = 3.1415926;

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

//POINT->Point
static inline Point POINTTOPoint(POINT pt)
{
	return Point(pt.x, pt.y);
}

//��ȡֱ�����X����������pt1ΪԲ�İ�˳ʱ����ת�ĽǶ�
//�Ի����Ʊ�ʾ��ȡֵ��Χ[0, 2pi)
static inline double GetAngleOfLine(POINT pt1, POINT pt2)
{
	double dx = pt2.x - pt1.x;
	double dy = pt2.y - pt1.y;
	double theta;
	if (dx > 0 && dy >= 0)
		theta = atan(dy / dx);
	else if (dx == 0 && dy >= 0)
		theta = pi / 2;
	else if (dx < 0)
		theta = pi + atan(dy / dx);
	else if (dx == 0 && dy < 0)
		theta = pi * 3 / 2;
	else if (dx > 0 && dy < 0)
		theta = pi * 2 + atan(dy / dx);
	return theta;
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
		//Ruler---------------ֱ��
		//AcuteTriangle-------������ǳ�
		//IsoscelesTriangle---�������ǳ�
		//Protractor----------������
		//Goniometer----------�Ƕȳ�
		//Compass-------------Բ��
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
	HRGN m_rgn;//ͼ����������
	HRGN m_rgnTopRight;//�������������
	POINT m_ptBegin;
	POINT m_ptEnd;
	POINT m_ptTmp;//�м�㣬���ڽǶȳ�
	BOOL m_bSelect;

	void Draw(Graphics* pGraphics);
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

	void Construct();
	BOOL TrackHandle(int nHandle, HWND pWnd, POINT point, HWND pWndClipTo);
	void ModifyPointers(int nHandle, POINT ptBeginSave, POINT ptEndSave, POINT ptTmpSave, POINT ptDown, POINT ptLast);
	void AdjustRgn(int nHandle, POINT ptBegin, POINT ptEnd);

private:
	void DrawRuler(Graphics* pGraphics, POINT pt1, POINT pt2, const int iHeight = 50) const;
	void DrawTriangle(Graphics* pGraphics, POINT pt1, POINT pt2, int angle) const;
	void DrawProtractor(Graphics* pGraphics, POINT pt1, POINT pt2) const;
	void DrawGoniometer(Graphics* pGraphics, POINT pt1, POINT pt2, POINT pt3) const;
	void DrawCompass(Graphics* pGraphics, POINT pt1, POINT pt2, const int iHeight);
	void GetCompassPoints(const POINT pt1, const POINT pt2, LPPOINT lppt3, LPPOINT lppt4, LPPOINT lppt5, LPPOINT lppt6, LPPOINT lppt7, LPPOINT lppt8, LPPOINT lppt9);
	void DrawArc(Graphics* pGraphics);
};

class CArc
{
public:
	CArc()
	{
		m_ptCenter = { 0, 0 };
		m_iRadius = 0;
		m_fStartAngle = -1;
		m_fSweepAngle1 = 0;
		m_fSweepAngle2 = 0;
		m_dCurrentTheta = 0;
	}
	CArc(POINT ptCenter, UINT iRadius)
	{
		m_ptCenter.x = ptCenter.x;
		m_ptCenter.y = ptCenter.y;
		m_iRadius = iRadius;
		m_fStartAngle = -1;
		m_fSweepAngle1 = 0;
		m_fSweepAngle2 = 0;
		m_dCurrentTheta = 0;
	}
	~CArc()
	{
		
	}

	POINT m_ptCenter;//Բ��
	UINT m_iRadius;//�뾶
	REAL m_fStartAngle;//��ʼ�Ƕ�
	REAL m_fSweepAngle1;//˳ʱ����ת�Ƕȣ�������
	REAL m_fSweepAngle2;//��ʱ����ת�Ƕȣ�������
	double m_dCurrentTheta;//��ǰ���������Բ�ĵ�ֱ�ߵ�ƫת�ǣ������ƣ�,����ȷ����ת��������ʱ�뻹��˳ʱ������
};

extern std::vector<CArc> m_vecArcs;
