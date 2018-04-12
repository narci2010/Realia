
#include "BinTree.h"

CBinTree::CBinTree()
{
	m_pRootNode = NULL;
	m_pSelectNode = NULL;
	m_dwSelectType = NULL;
	::ZeroMemory(&m_strName, sizeof(m_strName));
}

CBinTree::~CBinTree()
{
	// clean up the memory
	if (m_pRootNode)
		delete m_pRootNode;
}

// create a tree with node of type strType as root
CNode* CBinTree::CreateTree(UINT iType)
{
	m_pRootNode = new CNode;
	m_pRootNode->m_iNodeType = iType;
	if (iType == NT_EQUATION) {
		memcpy(m_pRootNode->m_strText, _T("="), 1);
	}
	else if (iType == NT_PLUS) {
		memcpy(m_pRootNode->m_strText, _T("+"), 1);
	}
	return m_pRootNode;
}

CNode* CBinTree::CreateNode(UINT iType, POINT pt)
{
	if (m_pRootNode == NULL) {
		m_pRootNode = new CNode;
		m_pRootNode->m_iNodeType = iType;

		if (iType == NT_EQUATION) {
			memcpy(m_pRootNode->m_strText, _T("="), 1);
		}
		else if (iType == NT_PLUS) {
			memcpy(m_pRootNode->m_strText, _T("+"), 1);
		}

		m_pRootNode->m_rcNode.left = pt.x + 1;
		m_pRootNode->m_rcNode.top = pt.y;
		m_pRootNode->m_rcNode.right = pt.x + 21;
		m_pRootNode->m_rcNode.bottom = pt.y + 20;

		//创建左孩子
		CNode* lpNode = new CNode;
		lpNode->m_iNodeType = NT_STANDARD;
		lpNode->m_rcNode.left = pt.x;
		lpNode->m_rcNode.top = pt.y;
		lpNode->m_rcNode.right = pt.x;
		lpNode->m_rcNode.bottom = pt.y;
		m_pRootNode->m_pLeftChild = lpNode;

		//创建右孩子
		CNode* rpNode = new CNode;
		rpNode->m_iNodeType = NT_STANDARD;
		rpNode->m_rcNode.left = pt.x + 22;
		rpNode->m_rcNode.top = pt.y;
		rpNode->m_rcNode.left = pt.x + 22;
		rpNode->m_rcNode.bottom = pt.y;
	}

	return m_pRootNode;
}

// return the root node of the tree
CNode* CBinTree::GetRootNode()
{
	return m_pRootNode;
}

// sets the root node of the tree
void CBinTree::SetRootNode(CNode* pNode)
{
	m_pRootNode = pNode;
}

// return the name of the tree
LPCTSTR CBinTree::GetName()
{
	return m_strName;
}

// set the name of the tree
void CBinTree::SetName(LPCTSTR strName)
{
	memcpy(m_strName, strName, lstrlen(strName));
}

void CBinTree::preTraverse(CNode* T, HDC pDC)
{
	if (T)
	{
		T->DrawNode(pDC);
		preTraverse(T->m_pLeftChild, pDC);
		preTraverse(T->m_pRightChild, pDC);
	}
}

void CBinTree::midTraverse(CNode* T, HDC pDC)
{
	if (T)
	{
		midTraverse(T->m_pLeftChild, pDC);
		T->DrawNode(pDC);
		midTraverse(T->m_pRightChild, pDC);
	}
}

void CBinTree::postTraverse(CNode* T, HDC pDC)
{
	if (T)
	{
		postTraverse(T->m_pLeftChild, pDC);
		postTraverse(T->m_pRightChild, pDC);
		T->DrawNode(pDC);
	}
}

// draw the tree (inorder)
void CBinTree::DrawTree(HDC pDC)
{
	//pDC->SetBkMode(TRANSPARENT);

	if (m_pRootNode)
		m_pRootNode->DrawNode(pDC);
}
