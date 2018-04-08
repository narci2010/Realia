
#include "stdafx.h"
#include "FormulaWnd.h"

CFormulaWnd* CFormulaWnd::m_pInstance = NULL;

CFormulaWnd::CFormulaWnd()
{

}

CFormulaWnd::~CFormulaWnd()
{
	delete this;
}

CFormulaWnd* CFormulaWnd::Instance()
{
	if (m_pInstance == NULL)
		m_pInstance = new CFormulaWnd;
	return m_pInstance;
}

void CFormulaWnd::InitWindow(HWND hWnd)
{
	m_hWnd = hWnd;

	GetClientRect(m_hWnd, &m_rcWindow);
	m_lWndWidth = m_rcWindow.right - m_rcWindow.left;
	m_lWndHeight = m_rcWindow.bottom - m_rcWindow.top;

}

void CFormulaWnd::SetParentWnd(HWND hWndParent)
{
	m_hWndParent = hWndParent;
}

HWND CFormulaWnd::GetParentWnd()
{
	return m_hWndParent;
}

HWND CFormulaWnd::GetSelfWnd()
{
	return m_hWnd;
}
