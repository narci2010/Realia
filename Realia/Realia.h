#pragma once

#include "stdafx.h"
#include "RectTracker.h"

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

private:
	HWND m_hWnd;

	RECT m_rcWindow;//窗口矩形（客户区的相对位置，起点为(0,0)，使用屏幕坐标时需要转换）
	LONG m_lWndWidth;//窗口宽度
	LONG m_lWndHeight;//窗口高度

	BOOL m_bIsLButtonDown;
	POINT m_ptBegin;
	POINT m_ptEnd;

	CRectTracker m_RectTracker;

	int m_iRealiaType;//教具类型

	//画背景
	void DrawBackground(HDC dc, RECT rc);
	void DrawRuler(HDC dc, POINT pt1, POINT pt2, const int iHeight = 50);
	void DrawProtractor(HDC dc, POINT pt1, POINT pt2);
};