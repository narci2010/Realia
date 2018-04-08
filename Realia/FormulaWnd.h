#pragma once

class CFormulaWnd
{
public:
	CFormulaWnd();
	~CFormulaWnd();

	static CFormulaWnd* Instance();
	void InitWindow(HWND hWnd);
	void SetParentWnd(HWND hWndParent);
	HWND GetParentWnd();
	HWND GetSelfWnd();

private:
	static CFormulaWnd* m_pInstance;
	HWND m_hWnd;
	HWND m_hWndParent;

	RECT m_rcWindow;//���ھ��Σ��ͻ��������λ�ã����Ϊ(0,0)��ʹ����Ļ����ʱ��Ҫת����
	LONG m_lWndWidth;//���ڿ��
	LONG m_lWndHeight;//���ڸ߶�
};
