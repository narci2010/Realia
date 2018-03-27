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

	RECT m_rcWindow;//���ھ���
	LONG m_lWndWidth;//���ڿ��
	LONG m_lWndHeight;//���ڸ߶�

	//������
	void DrawBackground(HDC dc, RECT rc);
	void DrawRuler(HDC dc, POINT pt1, POINT pt2);
};