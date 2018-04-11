
#include "stdafx.h"
#include "FormulaWnd.h"
#include "RichEditUtil.h"

const TCHAR* const pFormulaRichEditControlName = _T("reFormula");

CFormulaWnd* CFormulaWnd::m_pInstance = NULL;

CFormulaWnd::CFormulaWnd()
{

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

	CDuiString strRtfPath = m_PaintManager.GetInstancePath();
	strRtfPath.Append(_T("limit.rtf"));
	ReadFileByStreamIn(m_pRichEdit, strRtfPath.GetData());
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

void CFormulaWnd::OnPrepare(TNotifyUI& msg)
{
	//禁用父窗口
	EnableWindow(m_hWndParent, false);
}
void CFormulaWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("windowinit")) {
		OnPrepare(msg);
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
