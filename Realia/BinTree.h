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

	//ǰ�����������
	void preTraverse(CNode* T, Graphics* pGraphics);
	//�������������
	void midTraverse(CNode* T, Graphics* pGraphics);
	//��������������
	void postTraverse(CNode* T, Graphics* pGraphics);

	// draw the tree on a dc
	void DrawTree(Graphics* pGraphics);

	//���¶�λ�������ı���
	void GetEditInputPos(POINT pt, int* iUpdateStatus, LPRECT lprc, LPTSTR strText);

	//����ѡ�нڵ���������ݺ����򣬲����������ض�λ�ı���
	RECT UpdateSelectNode(LPCTSTR strText);

	//pNode1����RectInc������Ľ��ҲҪ���ŵ���
	void UpdateNodeRectByRectInc(CNode* pNode1, CNode* pNode2, int iMode = 0);
};
