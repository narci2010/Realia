
#include "stdafx.h"
#include "FormulaWnd.h"
#include "MenuWnd.h"
#include "Util.h"
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
	m_pCmbFontSize= static_cast<CComboUI*>(m_PaintManager.FindControl(_T("cmbfontsize")));
	m_pBtnNew = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnNew")));
	m_pBtnNum = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnNum")));
	m_pBtnSign = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnSign")));
	m_pBtnSpecialSign = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnSpecialSign")));

	m_pEditInput = static_cast<CRichEditUI*>(m_PaintManager.FindControl(pFormulaEditInputControlName));
	if (m_pEditInput == NULL)
		Close();

	//��ӦOnChange(EM_SETEVENTMASK ���� ENM_CHANGE)
	long lMask = m_pEditInput->GetEventMask();
	lMask |= ENM_CHANGE;
	lMask &= ~ENM_PROTECTED;
	m_pEditInput->SetEventMask(lMask);

	//CHARFORMAT2 cf;
	//ZeroMemory(&cf, sizeof(CHARFORMAT2));
	//cf.cbSize = sizeof(cf);
	//cf.dwMask = CFM_COLOR | CFM_SIZE | CFM_FACE;
	//cf.crTextColor = RGB(0, 0, 0);//������ɫ
	//cf.yHeight = -400;//���ø߶�
	//lstrcpy(cf.szFaceName, _T("΢���ź�"));//��������
	//m_pEditInput->SetDefaultCharFormat(cf);
	//m_pEditInput->SetSelectionCharFormat(cf);
	m_pEditInput->SetFont(20);

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
	m_rcEdit.right = m_rcEdit.left + 1;//�˴���+1�ᵼ�¿������ʱ�����ʾ�쳣
	m_rcEdit.bottom = m_rcEdit.top + 30;//20��������߶�Ϊ27
	m_pEditInput->SetPos(m_rcEdit);
	m_pEditInput->SetFocus();
}

void CFormulaWnd::OnFinalMessage(HWND hWnd)
{
	//���ø�����
	EnableWindow(m_hWndParent, true);

	delete m_pBinTree;
	m_pBinTree = NULL;

	__super::OnFinalMessage(hWnd);

	//�˴���bug��pRootָ��Ŀռ����ͷţ�����pRoot != NULL�����Ե���ģʽ�ܴ�������
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

LRESULT CFormulaWnd::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{


	bHandled = FALSE;
	return 0;
}

LRESULT CFormulaWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_COMMAND) {
		switch (LOWORD(wParam))
		{
		case MENUWNDCMD_BTNPLUS:
		{
			//AttachThreadInput(GetCurrentThreadId(), ::GetWindowThreadProcessId(m_hWnd, NULL), TRUE);
			POINT point;
			GetCaretPos(&point);
			m_pBinTree->CreateNode(NT_PLUS, point);
			InvalidateRect(m_hWnd, &m_rcWindow, true);
			UpdateWindow(m_hWnd);
		}
			break;
		case MENUWNDCMD_BTNEQUATION:
		{
			POINT point;
			GetCaretPos(&point);
			m_pBinTree->CreateNode(NT_EQUATION, point);
			InvalidateRect(m_hWnd, &m_rcWindow, true);
			UpdateWindow(m_hWnd);
		}
			break;
		default:
			break;
		}
	}
	else if (uMsg == WM_LBUTTONDOWN) {
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		if (PtInRect(&m_rcFormula, pt)) {
			RECT rc;
			int iUpdateStatus;
			TCHAR strText[MAX_PATH];
			::ZeroMemory(&strText, sizeof(strText));
			m_pBinTree->GetEditInputPos(pt, &iUpdateStatus, &rc, strText);

			if (iUpdateStatus == 1) {
				m_rcEdit.left = rc.left;
				m_rcEdit.top = rc.top;
				m_rcEdit.right = rc.right;
				m_rcEdit.bottom = rc.bottom;
				m_pEditInput->SetPos(m_rcEdit);
				m_pEditInput->SetText(strText);
				InvalidateRect(m_hWnd, &m_rcWindow, true);
				UpdateWindow(m_hWnd);
			}
		}
	}
	else if (uMsg == WM_PAINT) {
		if (m_bWindowInit) {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(m_hWnd, &ps);

			assert(SaveDC(hdc) != 0);

			OnPaint(hdc);

			RestoreDC(hdc, -1);

			EndPaint(m_hWnd, &ps);
			return 0;
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CFormulaWnd::OnPrepare(TNotifyUI& msg)
{
	//���ø�����
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
			CMenuWnd* pMenu = new CMenuWnd(m_hWnd, _T("MenuNum.xml"));
			CDuiPoint point = msg.ptMouse;
			ClientToScreen(m_hWnd, &point);
			pMenu->Init(m_hWnd, point);
		}
		else if (msg.pSender == m_pBtnSign) {
			CMenuWnd* pMenu = new CMenuWnd(m_hWnd, _T("MenuSign.xml"));
			CDuiPoint point = msg.ptMouse;
			ClientToScreen(m_hWnd, &point);
			pMenu->Init(m_hWnd, point);
		}
		else if (msg.pSender == m_pBtnSpecialSign) {

		}
	}
	else if (msg.sType == _T("itemselect")) {
		if (msg.pSender == m_pCmbFontSize) {
			m_pEditInput->SetFont(CDuiStringToInt(m_pCmbFontSize->GetText()));
		}
	}
	else if (msg.sType == _T("textchanged")) {
		if (msg.pSender == m_pEditInput) {
			if (m_pBinTree->m_pSelectNode) {
				m_rcEdit = m_pBinTree->UpdateSelectNode(m_pEditInput->GetText().GetData());
				m_pEditInput->SetPos(m_rcEdit);
				InvalidateRect(m_hWnd, &m_rcWindow, true);
				UpdateWindow(m_hWnd);
			}
			else
				m_pEditInput->Undo();
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

void CFormulaWnd::OnPaint(HDC pdc)
{
	//������windowDC���ݵ��ڴ��豸����  
	HDC hDcMem = CreateCompatibleDC(pdc);
	//λͼ�ĳ�ʼ��������λͼ     
	HBITMAP hBmpMem = CreateCompatibleBitmap(pdc, m_lWndWidth, m_lWndHeight);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hDcMem, hBmpMem);

	//���Ʊ���
	m_PaintManager.GetRoot()->Paint(hDcMem, m_rcWindow, NULL);
	m_pEditInput->SetFocus();


	//ʹ��GDI+��ʽȥ���
	Graphics graphics(hDcMem);

	//���ڴ��н���һ�顰���⻭����
	Bitmap memBmp(m_lWndWidth, m_lWndHeight);
	//��ȡ����ڴ滭����Graphics����
	Graphics memGr(&memBmp);
	//������Ϊ������ƽ��ģʽ
	memGr.SetSmoothingMode(SmoothingModeHighQuality);

	//����ʽ
	m_pBinTree->DrawTree(&memGr);

	//���ڴ滭������������
	graphics.DrawImage(&memBmp, 0, 0, m_lWndWidth, m_lWndHeight);


	//˫���弼��
	BitBlt(pdc, 0, 0, m_lWndWidth, m_lWndHeight, hDcMem, 0, 0, SRCCOPY);
	SelectObject(hDcMem, hBmpOld);
	//�ͷ���Դ
	DeleteDC(hDcMem);
	DeleteObject(hBmpMem);
	DeleteObject(hBmpOld);
}
