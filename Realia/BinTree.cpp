
#include "BinTree.h"
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
		lpNode->m_rcNode.bottom = pt.y + 20;
		m_pRootNode->m_pLeftChild = lpNode;
		m_pRootNode->m_pLeftChild->m_pParent = m_pRootNode;

		//创建右孩子
		CNode* rpNode = new CNode;
		rpNode->m_iNodeType = NT_STANDARD;
		rpNode->m_rcNode.left = pt.x + 22;
		rpNode->m_rcNode.top = pt.y;
		rpNode->m_rcNode.right = pt.x + 22;
		rpNode->m_rcNode.bottom = pt.y + 20;
		m_pRootNode->m_pRightChild = rpNode;
		m_pRootNode->m_pRightChild->m_pParent = m_pRootNode;

		m_pSelectNode = m_pRootNode->m_pLeftChild;
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
		preTraverse(m_pRootNode, pDC);
		//m_pRootNode->DrawNode(pDC);
}

void CBinTree::GetEditInputPos(POINT pt, int* iUpdateStatus, LPRECT lprc, LPCTSTR strText)
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
	}

	if (minDistance == 10000)//不需要更新
		*iUpdateStatus = -1;
	else {
		*iUpdateStatus = 1;
		*lprc = nearestNode->m_rcNode;
		m_pSelectNode = nearestNode;
		strText = nearestNode->m_strText;
	}
}

RECT CBinTree::UpdateSelectNode(LPCTSTR strText)
{
	::ZeroMemory(&m_pSelectNode->m_strText, sizeof(m_pSelectNode->m_strText));

#   ifdef _UNICODE
	wmemcpy(m_pSelectNode->m_strText, strText, lstrlen(strText));
#   else
	memcpy(m_pSelectNode->m_strText, strText, lstrlen(strText));
#   endif

	//需要根据字体大小重新计算节点宽度
	m_pSelectNode->m_rcNode.right = m_pSelectNode->m_rcNode.left + lstrlen(strText) * 12;

	//如果是左节点宽度改变，对应父节点和兄弟节点的位置都要更新

	return m_pSelectNode->m_rcNode;
}
