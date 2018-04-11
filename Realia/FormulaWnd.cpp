
#include "stdafx.h"
#include "FormulaWnd.h"
#include "RichEditUtil.h"

const TCHAR* const pFormulaRichEditControlName = _T("reFormula");

CFormulaWnd* CFormulaWnd::m_pInstance = NULL;

CFormulaWnd::CFormulaWnd()
{
	m_bWindowInit = false;
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

static DWORD CALLBACK streamInCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	ReadFile((HANDLE)dwCookie, pbBuff, cb, (LPDWORD)pcb, NULL);//读取数据
	return 0;
}

void CFormulaWnd::InitWindow()
{
	m_pRichEdit = static_cast<CRichEditUI*>(m_PaintManager.FindControl(pFormulaRichEditControlName));
	if (m_pRichEdit == NULL)
		Close();

	m_pBtnNew = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnNew")));
	m_pBtnNum = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnNum")));

	GetClientRect(m_hWnd, &m_rcWindow);
	m_lWndWidth = m_rcWindow.right - m_rcWindow.left;
	m_lWndHeight = m_rcWindow.bottom - m_rcWindow.top;
	m_rcEdit = m_rcWindow;
	m_rcEdit.left += 20;
	m_rcEdit.top += 20;
	m_rcEdit.right -= 50;
	m_rcEdit.bottom -= 134;
	m_pRichEdit->SetPos(m_rcEdit);

	//CDuiString strRtfPath = m_PaintManager.GetInstancePath();
	//strRtfPath.Append(_T("limit.rtf"));
	//ReadFileByStreamIn(m_pRichEdit, strRtfPath.GetData());
	//InsertObject(m_pRichEdit, strRtfPath.GetData());
}

void CFormulaWnd::OnFinalMessage(HWND hWnd)
{
	//启用父窗口
	EnableWindow(m_hWndParent, true);

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
	}
	else if (uMsg == WM_PAINT) {
		if (m_bWindowInit) {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(m_hWnd, &ps);

			assert(SaveDC(hdc) != 0);

			//m_PaintManager.GetRoot()->Paint(hdc, m_rcWindow, NULL);

			HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
			HPEN oldpen = (HPEN)SelectObject(hdc, pen);

			HBRUSH hbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			SelectObject(hdc, hbrush);
			Rectangle(hdc, 40, 80, 200, 200);
			DeleteObject(hbrush);

			SelectObject(hdc, oldpen);
			::DeleteObject(pen);
			::DeleteObject(oldpen);

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
