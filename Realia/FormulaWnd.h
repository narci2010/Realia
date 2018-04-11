#pragma once

class CFormulaWnd : public WindowImplBase
{
public:
	CFormulaWnd();
	~CFormulaWnd();

	static CFormulaWnd* Instance();
	//void InitWindow(HWND hWnd);
	void SetParentWnd(HWND hWndParent);
	HWND GetParentWnd();

	LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	//virtual LPCTSTR GetResourceID() const;
	//virtual UILIB_RESOURCETYPE GetResourceType() const;

	virtual void InitWindow();
	void OnFinalMessage(HWND hWnd);
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void OnPrepare(TNotifyUI& msg);
	void Notify(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);
	void OnTimer(TNotifyUI& msg);

private:
	static CFormulaWnd* m_pInstance;
	HWND m_hWndParent;

	RECT m_rcWindow;//���ھ���
	LONG m_lWndWidth;//���ڿ��
	LONG m_lWndHeight;//���ڸ߶�
	RECT m_rcEdit;//��ʽ�༭����

	bool m_bWindowInit;

	CRichEditUI* m_pRichEdit;
	CButtonUI* m_pBtnNew;
	CButtonUI* m_pBtnNum;
	CButtonUI* m_pBtnColor;
};
