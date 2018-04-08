#pragma once

#include "Realia.h"
#include <vector>

class CRealiaWnd
{
public:
	CRealiaWnd();
	~CRealiaWnd();

	void InitWindow(HWND hWnd);
	void OnPaint(HDC pDc);
	void OnLButtonDown(POINT pt);
	void OnLButtonUp(POINT pt);
	void OnMouseMove(POINT pt);
	BOOL OnSetCursor(HWND pWnd, UINT nHitTest);
	void SetRealiaType(int nType);
	UINT DeleteRealia();//删除选中的图形，返回删除的个数

private:
	HWND m_hWnd;

	RECT m_rcWindow;//窗口矩形（客户区的相对位置，起点为(0,0)，使用屏幕坐标时需要转换）
	LONG m_lWndWidth;//窗口宽度
	LONG m_lWndHeight;//窗口高度

	int m_iRealiaType;//教具类型

	std::vector<CRealia> m_vecRealias;//存放所画的全部教具

	void DrawBackground(HDC dc, RECT rc);//画背景
};

extern std::vector<CArc> m_vecArcs;//存放圆规所画的所有圆弧
