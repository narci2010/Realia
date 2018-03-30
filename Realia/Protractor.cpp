#include "Protractor.h"

CProtractor::CProtractor()
{
	m_nStyle = CRealia::Ruler;
	m_iHeight = 50;
}

CProtractor::~CProtractor()
{
}

CProtractor::CProtractor(UINT iHeight)
{
	m_nStyle = CRealia::Ruler;
	m_iHeight = iHeight;
}

int CProtractor::HitTest(POINT point) const
{
	TrackerHit hitResult = hitNothing;

	assert(m_ptBegin.x <= m_ptEnd.x);

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

void CProtractor::AdjustRgn(int nHandle, POINT ptBegin, POINT ptEnd)
{
	//m_rgn = CreateRectRgn(m_ptBegin.x, m_ptBegin.y, m_ptEnd.x, m_ptEnd.y + 50);
	int iHeight = 50;
	POINT pt1, pt2, pt3, pt4;
	pt1.x = ptBegin.x;
	pt1.y = ptBegin.y;
	pt2.x = ptEnd.x;
	pt2.y = ptEnd.y;
	double px = pt2.x - pt1.x;
	double py = pt2.y - pt1.y;
	double pl = sqrt(px * px + py * py);
	double theta = atan(py / px);
	pt3.x = pt2.x - iHeight * sin(theta);
	pt3.y = pt2.y + iHeight * cos(theta);
	pt4.x = pt1.x - iHeight * sin(theta);
	pt4.y = pt1.y + iHeight * cos(theta);
	POINT pts[4];
	EqualPoint(&pts[0], &pt1);//pts[0] = pt1;
	EqualPoint(&pts[1], &pt2);//pts[1] = pt2;
	EqualPoint(&pts[2], &pt3);//pts[2] = pt3;
	EqualPoint(&pts[3], &pt4);//pts[3] = pt4;
	m_rgn = CreatePolygonRgn(pts, 4, WINDING);
}

void CProtractor::DrawInside(HDC dc, POINT pt1, POINT pt2) const
{
	//画内部区域，有继承类自己实现
	DrawRuler(dc, pt1, pt2, m_iHeight);
}

//画直尺：5像素为1毫米，固定直尺总宽度为60像素
//普通刻度10像素长，5的倍数20像素长，10的倍数25像素长
void CProtractor::DrawRuler(HDC dc, POINT pt1, POINT pt2, int iHeight) const
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
