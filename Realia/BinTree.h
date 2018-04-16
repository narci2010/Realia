#pragma once

#include "Node.h"

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
	CNode* CreateTree(UINT iType, POINT pt);

	//creates a node of type strType
	CNode* CreateNode(UINT iType, int iCharPos);
	CNode* CreateOperatorNode(UINT iType, int iCharPos);
	CNode* CreateFractionalNode(UINT iType, int iCharPos);

	void ReplaceNode(CNode* pDestNode, CNode* pSrcNode);

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
	void preTraverse(CNode* T, Graphics* pGraphics);
	//中序遍历二叉树
	void midTraverse(CNode* T, Graphics* pGraphics);
	//后续遍历二叉树
	void postTraverse(CNode* T, Graphics* pGraphics);

	// draw the tree on a dc
	void DrawTree(Graphics* pGraphics);

	//重新定位并更新文本框
	void GetEditInputPos(POINT pt, int* iUpdateStatus, LPRECT lprc, LPTSTR strText);

	//更新选中节点的文字内容和区域，并传回区域重定位文本框
	RECT UpdateSelectNode(LPCTSTR strText);

	//pNode1存在RectInc，后面的结点也要跟着调整
	void UpdateNodeRectByRectInc(CNode* pNode1, CNode* pNode2, int iMode = 0);
};
