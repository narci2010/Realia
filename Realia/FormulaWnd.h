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

protected:
	void OnPrepare(TNotifyUI& msg);
	void Notify(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);
	void OnTimer(TNotifyUI& msg);

private:
	static CFormulaWnd* m_pInstance;
	HWND m_hWndParent;

	CRichEditUI* m_pRichEdit;
};
