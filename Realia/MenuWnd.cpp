
#include "MenuWnd.h"

CMenuWnd::CMenuWnd(HWND hParent, LPCTSTR strXMLPath):m_hParent(hParent),m_strXMLPath(strXMLPath)
{

}

CMenuWnd::~CMenuWnd()
{

}

void CMenuWnd::Init(HWND hWndParent, POINT point)
{
	Create(m_hParent, _T("MenuWnd"), WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, CDuiRect());
	::SetWindowPos(m_hWnd, NULL, point.x, point.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	::ShowWindow(m_hWnd, SW_SHOW);
}

LPCTSTR CMenuWnd::GetWindowClassName() const
{
	return _T("MenuWnd");
}

UINT CMenuWnd::GetClassStyle() const
{
	return UI_CLASSSTYLE_DIALOG | CS_DBLCLKS;
}

CDuiString CMenuWnd::GetSkinFile()
{
	return m_strXMLPath;
}

CDuiString CMenuWnd::GetSkinFolder()
{
	return _T("");
}

void CMenuWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("click")) {
		WPARAM wparam = GetMenuWndCmdByBtn(msg.pSender->GetName().GetData());
		if (wparam != 0)
			::SendMessage(m_hParent, WM_COMMAND, wparam, msg.lParam);
	}
	return __super::Notify(msg);
}

void CMenuWnd::OnFinalMessage(HWND hWnd)
{
	delete this;
}

LRESULT CMenuWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Close();
	bHandled = FALSE;
	return 0;
}

LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL    bHandled = TRUE;

	switch (uMsg)
	{
	case WM_KILLFOCUS:
		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		break;
	default:
		bHandled = FALSE;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

WPARAM CMenuWnd::GetMenuWndCmdByBtn(LPCTSTR strName)
{
	WPARAM result = 0;
	if (!_tcscmp(strName, _T("btnNum0")))
		result = MENUWNDCMD_BTNNUMBER0;
	else if (!_tcscmp(strName, _T("btnNum1")))
		result = MENUWNDCMD_BTNNUMBER1;
	else if (!_tcscmp(strName, _T("btnNum2")))
		result = MENUWNDCMD_BTNNUMBER2;
	else if (!_tcscmp(strName, _T("btnNum3")))
		result = MENUWNDCMD_BTNNUMBER3;
	else if (!_tcscmp(strName, _T("btnNum4")))
		result = MENUWNDCMD_BTNNUMBER4;
	else if (!_tcscmp(strName, _T("btnNum5")))
		result = MENUWNDCMD_BTNNUMBER5;
	else if (!_tcscmp(strName, _T("btnNum6")))
		result = MENUWNDCMD_BTNNUMBER6;
	else if (!_tcscmp(strName, _T("btnNum7")))
		result = MENUWNDCMD_BTNNUMBER7;
	else if (!_tcscmp(strName, _T("btnNum8")))
		result = MENUWNDCMD_BTNNUMBER8;
	else if (!_tcscmp(strName, _T("btnNum9")))
		result = MENUWNDCMD_BTNNUMBER9;
	else if (!_tcscmp(strName, _T("btnPoint")))
		result = MENUWNDCMD_BTNPOINT;
	else if (!_tcscmp(strName, _T("btnPlus")))
		result = MENUWNDCMD_BTNPLUS;
	else if (!_tcscmp(strName, _T("btnMinus")))
		result = MENUWNDCMD_BTNMINUS;
	else if (!_tcscmp(strName, _T("btnMultiply")))
		result = MENUWNDCMD_BTNMULTIPLY;
	else if (!_tcscmp(strName, _T("btnDivide")))
		result = MENUWNDCMD_BTNDIVIDE;
	else if (!_tcscmp(strName, _T("btnEquation")))
		result = MENUWNDCMD_BTNEQUATION;

	return result;
}
