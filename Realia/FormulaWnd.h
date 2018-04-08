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

	RECT m_rcWindow;//窗口矩形（客户区的相对位置，起点为(0,0)，使用屏幕坐标时需要转换）
	LONG m_lWndWidth;//窗口宽度
	LONG m_lWndHeight;//窗口高度
};
