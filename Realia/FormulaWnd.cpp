
#include "stdafx.h"
#include "FormulaWnd.h"
#include "RichEditUtil.h"

const TCHAR* const pFormulaRichEditControlName = _T("reFormula");
const TCHAR* const pFormulaEditInputControlName = _T("editInput");

CFormulaWnd* CFormulaWnd::m_pInstance = NULL;

CFormulaWnd::CFormulaWnd()
{
	::ZeroMemory(&m_rcWindow, sizeof(m_rcWindow));
	m_lWndWidth = 0;
	m_lWndHeight = 0;
	::ZeroMemory(&m_rcFormula, sizeof(m_rcFormula));
	::ZeroMemory(&m_rcEdit, sizeof(m_rcEdit));
	m_bWindowInit = false;
	m_pBinTree = new CBinTree;
}

CFormulaWnd::~CFormulaWnd()
{
	
}

CFormulaWnd* CFormulaWnd::Instance()
{
	if (m_pInstance == NULL)
		m_pInstance = new CFormulaWnd;
	return m_pInstance;
}

void CFormulaWnd::SetParentWnd(HWND hWndParent)
{
	m_hWndParent = hWndParent;
}

HWND CFormulaWnd::GetParentWnd()
{
	return m_hWndParent;
}

LPCTSTR CFormulaWnd::GetWindowClassName() const
{
	return _T("UIFormulaFrame");
}

UINT CFormulaWnd::GetClassStyle() const
{
	return UI_CLASSSTYLE_DIALOG | CS_DBLCLKS;
}

CDuiString CFormulaWnd::GetSkinFile()
{
	return _T("formula.xml");
}

CDuiString CFormulaWnd::GetSkinFolder()
{
	return  _T("");
}

//LPCTSTR CFormulaWnd::GetResourceID() const
//{
//	return MAKEINTRESOURCE(IDR_ZIPRES1);
//}
//
//UILIB_RESOURCETYPE CFormulaWnd::GetResourceType() const
//{
//	return UILIB_ZIPRESOURCE;
//}

void CFormulaWnd::InitWindow()
{
	m_pBtnNew = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnNew")));
	m_pBtnNum = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnNum")));
	m_pBtnSign = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnSign")));
	m_pBtnSpecialSign = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnSpecialSign")));

	m_pEditInput = static_cast<CEditUI*>(m_PaintManager.FindControl(pFormulaEditInputControlName));
	if (m_pEditInput == NULL)
		Close();

	GetClientRect(m_hWnd, &m_rcWindow);
	m_lWndWidth = m_rcWindow.right - m_rcWindow.left;
	m_lWndHeight = m_rcWindow.bottom - m_rcWindow.top;
	m_rcEdit = m_rcWindow;
	m_rcFormula = m_rcWindow;
	m_rcFormula.left += 50;
	m_rcFormula.top += 74;
	m_rcFormula.right -= 20;
	m_rcFormula.bottom -= 80;
	m_rcEdit.left += 50;
	m_rcEdit.top += 74;
	m_rcEdit.right = m_rcEdit.left + 20;
	m_rcEdit.bottom = m_rcEdit.top + 20;
	m_pEditInput->SetPos(m_rcEdit);
	m_pEditInput->SetFocus();
}

void CFormulaWnd::OnFinalMessage(HWND hWnd)
{
	//启用父窗口
	EnableWindow(m_hWndParent, true);

	delete m_pBinTree;
	m_pBinTree = NULL;

	__super::OnFinalMessage(hWnd);

	//此处有bug，pRoot指向的空间已释放，但是pRoot != NULL，所以单例模式总存在问题
	//CControlUI* pRoot = m_PaintManager.GetRoot();
	//if (pRoot != NULL) {
	//	pRoot->Delete();
	//	pRoot = NULL;
	//}
	//DestroyWindow(m_hWnd);
	delete this;
	m_pInstance = NULL;
}

LRESULT CFormulaWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CFormulaWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_LBUTTONDOWN) {
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		if (PtInRect(&m_rcFormula, pt)) {
			m_rcEdit.left = pt.x;
			m_rcEdit.top = pt.y;
			m_rcEdit.right = m_rcEdit.left + 20;
			m_rcEdit.bottom = m_rcEdit.top + 20;
			m_pEditInput->SetPos(m_rcEdit);
			InvalidateRect(m_hWnd, &m_rcWindow, true);
			UpdateWindow(m_hWnd);
		}
	}
	else if (uMsg == WM_PAINT) {
		if (m_bWindowInit) {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(m_hWnd, &ps);

			assert(SaveDC(hdc) != 0);

			m_PaintManager.GetRoot()->Paint(hdc, m_rcWindow, NULL);
			m_pEditInput->SetFocus();

			//画公式
			m_pBinTree->DrawTree(hdc);

			RestoreDC(hdc, -1);

			EndPaint(m_hWnd, &ps);
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CFormulaWnd::OnPrepare(TNotifyUI& msg)
{
	//禁用父窗口
	EnableWindow(m_hWndParent, false);
}
void CFormulaWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("windowinit")) {
		OnPrepare(msg);
		m_bWindowInit = true;
	}
	else if (msg.sType == _T("click")) {
		if (msg.pSender == m_pBtnNew) {
			InvalidateRect(m_hWnd, &m_rcWindow, true);
			UpdateWindow(m_hWnd);
		}
		else if (msg.pSender == m_pBtnNum) {
			AttachThreadInput(GetCurrentThreadId(), ::GetWindowThreadProcessId(m_hWnd, NULL), TRUE);
			POINT point;
			GetCaretPos(&point);
			m_pBinTree->CreateNode(NT_PLUS, point);
			InvalidateRect(m_hWnd, &m_rcWindow, true);
			UpdateWindow(m_hWnd);
		}
		else if (msg.pSender == m_pBtnSign) {
			POINT point;
			GetCaretPos(&point);
			m_pBinTree->CreateNode(NT_EQUATION, point);
			InvalidateRect(m_hWnd, &m_rcWindow, true);
			UpdateWindow(m_hWnd);
		}
		else if (msg.pSender == m_pBtnSpecialSign) {

		}
	}
	else if (msg.sType == _T("textchanged")) {
		if (msg.pSender == m_pEditInput) {
			m_rcEdit.right += 20;
			m_pEditInput->SetPos(m_rcEdit);
			InvalidateRect(m_hWnd, &m_rcWindow, true);
			UpdateWindow(m_hWnd);
		}
	}
	return __super::Notify(msg);
}

void CFormulaWnd::OnExit(TNotifyUI& msg)
{
	Close();
}

void CFormulaWnd::OnTimer(TNotifyUI& msg)
{

}
