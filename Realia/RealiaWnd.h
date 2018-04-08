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
	UINT DeleteRealia();//ɾ��ѡ�е�ͼ�Σ�����ɾ���ĸ���

private:
	HWND m_hWnd;

	RECT m_rcWindow;//���ھ��Σ��ͻ��������λ�ã����Ϊ(0,0)��ʹ����Ļ����ʱ��Ҫת����
	LONG m_lWndWidth;//���ڿ��
	LONG m_lWndHeight;//���ڸ߶�

	int m_iRealiaType;//�̾�����

	std::vector<CRealia> m_vecRealias;//���������ȫ���̾�

	void DrawBackground(HDC dc, RECT rc);//������
};

extern std::vector<CArc> m_vecArcs;//���Բ������������Բ��
