#pragma once

#include "stdafx.h"

class CMenuWnd :public WindowImplBase
{
public:
	CMenuWnd(HWND hParent = NULL, LPCTSTR strXMLPath = _T(""));

	void Init(HWND hWndParent, POINT point);

	virtual LPCTSTR GetWindowClassName() const;
	virtual UINT GetClassStyle() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();

	void Notify(TNotifyUI& msg);

	virtual void OnFinalMessage(HWND hWnd);
	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual ~CMenuWnd();// 私有化析构函数，这样此对象只能通过new来生成，而不能直接定义变量。就保证了delete this不会出错

public:
	HWND m_hParent;
	CDuiString  m_strXMLPath;

private:
	WPARAM GetMenuWndCmdByBtn(LPCTSTR strName);
};
