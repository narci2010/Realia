
#include "BinTree.h"
#include "NodeEntity.h"
#include <stack>
using namespace std;

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
CNode* CBinTree::CreateTree(UINT iType, POINT pt)
{
	m_pRootNode = new CEditNode(NT_STANDARD, pt);
	UpdateNodeRectByRectInc(m_pRootNode, m_pRootNode, 5);
	m_pSelectNode = m_pRootNode;
	return m_pRootNode;
}

CNode* CBinTree::CreateNode(UINT iType, int iCharPos)
{
	switch (iType)
	{
	case NT_PLUS:
	case NT_MINUS:
	case NT_MULTIPLY:
	case NT_DIVIDE:
	case NT_EQUATION:
		CreateOperatorNode(iType, iCharPos);
		break;
	case NT_FRACTION:
		CreateFractionalNode(iType, iCharPos);
		break;
	default:
		break;
	}
	
	return NULL;
}

CNode* CBinTree::CreateOperatorNode(UINT iType, int iCharPos)
{
	//拆分字符串
	TCHAR strText[MAX_PATH], strLeftText[MAX_PATH], strRightText[MAX_PATH];
	::ZeroMemory(&strText, sizeof(strText));
	::ZeroMemory(&strLeftText, sizeof(strLeftText));
	::ZeroMemory(&strRightText, sizeof(strRightText));
	lstrcpy(strText, m_pSelectNode->GetText());
	lstrcpyn(strLeftText, strText, iCharPos + 1);
	lstrcpy(strRightText, strText + iCharPos);

	RECT rc = m_pSelectNode->GetRect();
	POINT pt = { rc.left, rc.top };

	//调整选中节点的宽高，并更新右边的节点
	m_pSelectNode->SetRectRightInc(rc.left - rc.right);
	m_pSelectNode->SetRectBottomInc(rc.top - rc.bottom);
	UpdateNodeRectByRectInc(m_pSelectNode, m_pRootNode, 5);

	CNode* pNode = new COperatorNode(iType, pt);

	//替换节点
	if (m_pSelectNode != m_pRootNode) {
		CNode* pParent = m_pSelectNode->GetParentNode();

		if (pParent->GetLeftChild() == m_pSelectNode)
			pParent->SetLeftChild(pNode);
		else if (pParent->GetRightChild() == m_pSelectNode)
			pParent->SetRightChild(pNode);
	}
	else {
		delete m_pRootNode;
		m_pRootNode = pNode;
	}
	m_pSelectNode = pNode;

	//创建左孩子
	CNode* lpNode = new CEditNode(NT_STANDARD, strLeftText, pt);
	pNode->m_pLeftChild = lpNode;
	pNode->m_pLeftChild->m_pParent = pNode;

	//创建右孩子
	CNode* rpNode = new CEditNode(NT_STANDARD, strRightText, pt);
	pNode->m_pRightChild = rpNode;
	pNode->m_pRightChild->m_pParent = pNode;

	UpdateNodeRectByRectInc(lpNode, m_pRootNode, 5);
	UpdateNodeRectByRectInc(pNode, m_pRootNode, 5);
	UpdateNodeRectByRectInc(rpNode, m_pRootNode, 5);

	//节点和孩子节点隔开一个像素
	lpNode->SetRectRightInc(1);
	lpNode->SetRectBottomInc(0);
	UpdateNodeRectByRectInc(lpNode, m_pRootNode, 1);
	pNode->SetRectRightInc(1);
	pNode->SetRectBottomInc(0);
	UpdateNodeRectByRectInc(pNode, m_pRootNode, 1);

	//m_pSelectNode = pNode->m_pLeftChild;

	return pNode;
}

CNode* CBinTree::CreateFractionalNode(UINT iType, int iCharPos)
{
	//拆分字符串
	TCHAR strText[MAX_PATH], strLeftText[MAX_PATH], strRightText[MAX_PATH];
	::ZeroMemory(&strText, sizeof(strText));
	::ZeroMemory(&strLeftText, sizeof(strLeftText));
	::ZeroMemory(&strRightText, sizeof(strRightText));
	lstrcpy(strText, m_pSelectNode->GetText());
	lstrcpyn(strLeftText, strText, iCharPos + 1);
	lstrcpy(strRightText, strText + iCharPos);

	RECT rc = m_pSelectNode->GetRect();
	POINT pt = { rc.left, rc.top };

	//调整选中节点的宽高，并更新右边的节点
	m_pSelectNode->SetRectRightInc(rc.left - rc.right);
	m_pSelectNode->SetRectBottomInc(rc.top - rc.bottom);
	UpdateNodeRectByRectInc(m_pSelectNode, m_pRootNode, 5);

	CNode* pNoneNode1 = new CNoneNode(pt);
	CNode* pNoneNode2 = new CNoneNode(pt);

	//替换节点
	if (m_pSelectNode != m_pRootNode) {
		CNode* pParent = m_pSelectNode->GetParentNode();

		if (pParent->GetLeftChild() == m_pSelectNode)
			pParent->SetLeftChild(pNoneNode1);
		else if (pParent->GetRightChild() == m_pSelectNode)
			pParent->SetRightChild(pNoneNode1);
	}
	else {
		delete m_pRootNode;
		m_pRootNode = pNoneNode1;
	}
	m_pSelectNode = pNoneNode1;

	//创建左孩子
	CNode* lpNode = new CEditNode(NT_STANDARD, strLeftText, pt);
	pNoneNode2->m_pLeftChild = lpNode;
	pNoneNode2->m_pLeftChild->m_pParent = pNoneNode2;

	//创建右孩子
	CNode* rpNode = new CEditNode(NT_STANDARD, strRightText, pt);
	pNoneNode1->m_pRightChild = rpNode;
	pNoneNode1->m_pRightChild->m_pParent = pNoneNode1;

	pNoneNode1->m_pLeftChild = pNoneNode2;
	pNoneNode2->m_pParent = pNoneNode1;

	UpdateNodeRectByRectInc(lpNode, m_pRootNode, 5);
	UpdateNodeRectByRectInc(rpNode, m_pRootNode, 5);

	//节点和孩子节点隔开一个像素
	lpNode->SetRectRightInc(3);
	lpNode->SetRectBottomInc(0);
	UpdateNodeRectByRectInc(lpNode, m_pRootNode, 1);
	pt = { lpNode->m_rcNode.right + 1, lpNode->m_rcNode.top };
	pNoneNode1->m_rcNode = { pt.x, pt.y, pt.x, pt.y };
	pNoneNode2->m_rcNode = { pt.x, pt.y, pt.x, pt.y };

	//创建分式结点并加到pNoneNode2的右孩子上
	m_pSelectNode = pNoneNode2;
	CNode* pNode = new CFractionalNode(20, pt);
	pNoneNode2->m_pRightChild = pNode;
	pNode->m_pParent = pNoneNode2;

	//创建分子结点
	lpNode = new CEditNode(NT_STANDARD, pt);
	pNode->m_pLeftChild = lpNode;
	pNode->m_pLeftChild->m_pParent = pNode;
	lpNode->m_rcNode.right = lpNode->m_rcNode.left + lpNode->m_iRectRightInc;
	lpNode->m_rcNode.bottom = lpNode->m_rcNode.top + lpNode->m_iRectBottomInc;
	lpNode->SetRectRightInc(0);
	lpNode->SetRectBottomInc(0);

	//修正分式的矩形区域
	pNode->m_rcNode.left = lpNode->m_rcNode.left;
	pNode->m_rcNode.right = lpNode->m_rcNode.right;
	pNode->m_rcNode.top = lpNode->m_rcNode.bottom + 1;
	pNode->m_rcNode.bottom = pNode->m_rcNode.top + pNode->m_iFontSize / 10 + 1;
	pt = { pNode->m_rcNode.left, pNode->m_rcNode.bottom + 1 };

	//创建分母结点
	rpNode = new CEditNode(NT_STANDARD, pt);
	pNode->m_pRightChild = rpNode;
	pNode->m_pRightChild->m_pParent = pNode;
	rpNode->m_rcNode.right = rpNode->m_rcNode.left + rpNode->m_iRectRightInc;
	rpNode->m_rcNode.bottom = rpNode->m_rcNode.top + rpNode->m_iRectBottomInc;
	rpNode->SetRectRightInc(0);
	rpNode->SetRectBottomInc(0);

	int lHeight = rpNode->m_rcNode.bottom - lpNode->m_rcNode.top;
	int lHeight2 = lpNode->m_rcNode.bottom - lpNode->m_rcNode.top;
	pNode->SetRectBottomInc((lHeight - lHeight2) / 2);
	UpdateNodeRectByRectInc(pNode, m_pRootNode, 2);

	return pNode;
}

void CBinTree::ReplaceNode(CNode* pDestNode, CNode* pSrcNode)
{
	if (pDestNode != m_pRootNode) {
		CNode* pParent = pDestNode->GetParentNode();

		if (pParent->GetLeftChild() == pDestNode)
			pParent->SetLeftChild(pSrcNode);
		else if (pParent->GetRightChild() == pDestNode)
			pParent->SetRightChild(pSrcNode);
	}

	if (m_pSelectNode == pDestNode)
		m_pSelectNode = pSrcNode;

	delete pDestNode;
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
	lstrcpy(m_strName, strName);
}

// select a node with a given selectiontype 
// NS_NODE (only the node)
// NS_SUBTREE (complete subtree)
void CBinTree::SelectNode(CNode* pNode, DWORD dwSelectType)
{
	m_pSelectNode = pNode;
	m_dwSelectType = dwSelectType;
}

// return the currently selected node
CNode* CBinTree::GetSelectedNode()
{
	return m_pSelectNode;
}

void CBinTree::preTraverse(CNode* T, Graphics* pGraphics)
{
	if (T)
	{
		T->DrawNode(pGraphics);
		preTraverse(T->m_pLeftChild, pGraphics);
		preTraverse(T->m_pRightChild, pGraphics);
	}
}

void CBinTree::midTraverse(CNode* T, Graphics* pGraphics)
{
	if (T)
	{
		midTraverse(T->m_pLeftChild, pGraphics);
		T->DrawNode(pGraphics);
		midTraverse(T->m_pRightChild, pGraphics);
	}
}

void CBinTree::postTraverse(CNode* T, Graphics* pGraphics)
{
	if (T)
	{
		postTraverse(T->m_pLeftChild, pGraphics);
		postTraverse(T->m_pRightChild, pGraphics);
		T->DrawNode(pGraphics);
	}
}

// draw the tree (inorder)
void CBinTree::DrawTree(Graphics* pGraphics)
{
	//pDC->SetBkMode(TRANSPARENT);

	if (m_pRootNode)
		preTraverse(m_pRootNode, pGraphics);
		//m_pRootNode->DrawNode(pDC);
}

void CBinTree::GetEditInputPos(POINT pt, int* iUpdateStatus, LPRECT lprc, LPTSTR strText)
{
	*lprc = { 0, 0, 0, 0 };
	DWORD minDistance = 10000;
	CNode* nearestNode = NULL;
	if (m_pRootNode) {
		CNode* pNode = m_pRootNode;
		stack<CNode*> s;
		while (pNode || !s.empty()) {
			while (pNode != NULL) {
				s.push(pNode);
				pNode = pNode->m_pLeftChild;
			}
			if (!s.empty()) {
				pNode = s.top();

				if (pNode->m_iNodeType == NT_STANDARD) {
					DWORD iDistance = DistanceBetweenPointAndRect(pNode->m_rcNode, pt);
					if (iDistance == 0) {
						minDistance = iDistance;
						nearestNode = pNode;
						break;
					}
					else if (iDistance > 0) {
						if (iDistance < minDistance) {
							minDistance = iDistance;
							nearestNode = pNode;
						}
					}
				}

				s.pop();
				pNode = pNode->m_pRightChild;
			}
		}

		while (!s.empty())//清空栈
			s.pop();
	}

	if (minDistance == 10000 || m_pSelectNode == nearestNode)//不需要更新
		*iUpdateStatus = -1;
	else {
		*iUpdateStatus = 1;
	}

	if (nearestNode) {
		*lprc = nearestNode->m_rcNode;
		m_pSelectNode = nearestNode;
		lstrcpy(strText, nearestNode->m_strText);
	}
}

RECT CBinTree::UpdateSelectNode(LPCTSTR strText)
{
	SIZE sizeOld = GetTextExtent(m_pSelectNode->m_strText, m_pSelectNode->m_lf.lfFaceName, m_pSelectNode->m_iFontSize);
	SIZE size = GetTextExtent(strText, m_pSelectNode->m_lf.lfFaceName, m_pSelectNode->m_iFontSize);
	//需要根据字体大小重新计算节点宽度
	//如果是左节点宽度改变，对应父节点和兄弟节点的位置都要更新
	m_pSelectNode->m_iRectRightInc = size.cx - sizeOld.cx;
	UpdateNodeRectByRectInc(m_pSelectNode, m_pRootNode, 5);

	::ZeroMemory(&m_pSelectNode->m_strText, sizeof(m_pSelectNode->m_strText));

	lstrcpy(m_pSelectNode->m_strText, strText);

	return m_pSelectNode->m_rcNode;
}

void CBinTree::UpdateNodeRectByRectInc(CNode* pNode1, CNode* pNode2, int iMode)
{
	if (pNode2) {
		if (pNode2->m_rcNode.left >= pNode1->m_rcNode.right && pNode2 != pNode1) {
			pNode2->m_rcNode.left += pNode1->m_iRectRightInc;
			pNode2->m_rcNode.right += pNode1->m_iRectRightInc;
		}
		if (iMode & 2) {
			if (pNode2->m_iNodeType != NT_FRACTION && (pNode2->m_pParent == NULL || pNode2->m_pParent->m_iNodeType != NT_FRACTION)) {
				pNode2->m_rcNode.top += pNode1->m_iRectBottomInc;
				pNode2->m_rcNode.bottom += pNode1->m_iRectBottomInc;
			}
		}
		UpdateNodeRectByRectInc(pNode1, pNode2->m_pLeftChild, iMode);
		UpdateNodeRectByRectInc(pNode1, pNode2->m_pRightChild, iMode);
	}
	if (pNode2 == m_pRootNode) {
		if (iMode & 4) {//调整源节点的宽度和高度
			pNode1->m_rcNode.right += pNode1->m_iRectRightInc;
			pNode1->m_rcNode.bottom += pNode1->m_iRectBottomInc;
		}
		pNode1->m_iRectRightInc = 0;
		pNode1->m_iRectBottomInc = 0;
	}
}
