#pragma once

#include "Node.h"

static inline DWORD DistanceBetweenPointAndRect(RECT rc, POINT pt)
{
	if (PtInRect(&rc, pt))
		return 0;
	else if (pt.y < rc.top || pt.y > rc.bottom)
		return -1;
	else {
		DWORD mx = (rc.left + rc.right) / 2;
		if (pt.x < mx)
			return mx - pt.x;
		else
			return pt.x - mx;
	}
}

class CBinTree
{
public:
	CBinTree();
	virtual ~CBinTree();

	// the rootnode of the tree
	CNode* m_pRootNode;
	// the selected node
	CNode* m_pSelectNode;
	// the node-selection type (one of NS_*, see node.h)
	DWORD m_dwSelectType;
	// the name of the tree
	TCHAR m_strName[MAX_PATH];

	// creates a tree with a root node of type strType
	CNode* CreateTree(UINT iType);

	//creates a node of type strType
	CNode* CreateNode(UINT iType, POINT pt);

	// returns the root node
	CNode* GetRootNode();
	// sets the root node
	void SetRootNode(CNode* pNode);

	// Sets and Gets the Name of the Bintree
	void SetName(LPCTSTR strName);
	LPCTSTR GetName();

	// Select the node pNode with nodeselectiontype dwSelectType
	void SelectNode(CNode* pNode, DWORD dwSelectType);
	// Get the selected node
	CNode* GetSelectedNode();

	//前序遍历二叉树
	void preTraverse(CNode* T, HDC pDC);
	//中序遍历二叉树
	void midTraverse(CNode* T, HDC pDC);
	//后续遍历二叉树
	void postTraverse(CNode* T, HDC pDC);

	// draw the tree on a dc
	void DrawTree(HDC pDC);

	//重新定位并更新文本框
	void GetEditInputPos(POINT pt, int* iUpdateStatus, LPRECT lprc, LPCTSTR strText);

	//更新选中节点的文字内容和区域，并传回区域重定位文本框
	RECT UpdateSelectNode(LPCTSTR strText);
};
