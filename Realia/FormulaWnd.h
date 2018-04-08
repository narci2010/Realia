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

protected:
	void OnPrepare(TNotifyUI& msg);
	void Notify(TNotifyUI& msg);

private:
	static CFormulaWnd* m_pInstance;
	HWND m_hWndParent;

	RECT m_rcWindow;//���ھ��Σ��ͻ��������λ�ã����Ϊ(0,0)��ʹ����Ļ����ʱ��Ҫת����
	LONG m_lWndWidth;//���ڿ��
	LONG m_lWndHeight;//���ڸ߶�
};
