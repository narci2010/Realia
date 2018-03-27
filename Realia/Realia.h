#pragma once

#include "stdafx.h"

class CRealiaWnd
{
public:
	CRealiaWnd();
	~CRealiaWnd();

	void InitWindow(HWND hWnd);
	void OnPaint(HDC pDc);

private:
	HWND m_hWnd;

	RECT m_rcWindow;//窗口矩形
	LONG m_lWndWidth;//窗口宽度
	LONG m_lWndHeight;//窗口高度

	//画背景
	void DrawBackground(HDC dc, RECT rc);
	void DrawRuler(HDC dc, POINT pt1, POINT pt2);
};