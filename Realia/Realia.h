#pragma once

#include "stdafx.h"

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

private:
	HWND m_hWnd;

	RECT m_rcWindow;//���ھ��Σ��ͻ��������λ�ã����Ϊ(0,0)��ʹ����Ļ����ʱ��Ҫת����
	LONG m_lWndWidth;//���ڿ��
	LONG m_lWndHeight;//���ڸ߶�

	BOOL m_bIsLButtonDown;
	POINT m_ptBegin;
	POINT m_ptEnd;

	//������
	void DrawBackground(HDC dc, RECT rc);
	void DrawRuler(HDC dc, POINT pt1, POINT pt2);
	void DrawProtractor(HDC dc, POINT pt1, POINT pt2);
};