
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
CNode* CBinTree::CreateTree(UINT iType)
{
	m_pRootNode = new CNode;
	m_pRootNode->m_iNodeType = iType;
	if (iType == NT_EQUATION) {
		lstrcpy(m_pRootNode->m_strText, _T("="));
	}
	else if (iType == NT_PLUS) {
		lstrcpy(m_pRootNode->m_strText, _T("+"));
	}
	return m_pRootNode;
}

CNode* CBinTree::CreateNode(UINT iType, POINT pt)
{
	if (m_pRootNode == NULL) {
		m_pRootNode = new COperatorNode(iType, { pt.x + 1, pt.y });
		m_pRootNode->m_iNodeType = iType;

		//创建左孩子
		CNode* lpNode = new CEditNode(NT_STANDARD, pt);
		m_pRootNode->m_pLeftChild = lpNode;
		m_pRootNode->m_pLeftChild->m_pParent = m_pRootNode;

		RECT rcRoot = m_pRootNode->GetRect();
		//创建右孩子
		CNode* rpNode = new CEditNode(NT_STANDARD, { rcRoot.right + 1, rcRoot.top });
		m_pRootNode->m_pRightChild = rpNode;
		m_pRootNode->m_pRightChild->m_pParent = m_pRootNode;

		UpdateNodeRectByRectInc(lpNode, m_pRootNode);
		UpdateNodeRectByRectInc(rpNode, m_pRootNode);

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

	if (minDistance == 10000)//不需要更新
		*iUpdateStatus = -1;
	else {
		*iUpdateStatus = 1;
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
	UpdateNodeRectByRectInc(m_pSelectNode, m_pRootNode);

	::ZeroMemory(&m_pSelectNode->m_strText, sizeof(m_pSelectNode->m_strText));

	lstrcpy(m_pSelectNode->m_strText, strText);

	return m_pSelectNode->m_rcNode;
}

void CBinTree::UpdateNodeRectByRectInc(CNode* pNode1, CNode* pNode2)
{
	if (pNode2) {
		if (pNode2->m_rcNode.left > pNode1->m_rcNode.right && pNode2 != pNode1) {
			pNode2->m_rcNode.left += pNode1->m_iRectRightInc;
			pNode2->m_rcNode.right += pNode1->m_iRectRightInc;
		}
		UpdateNodeRectByRectInc(pNode1, pNode2->m_pLeftChild);
		UpdateNodeRectByRectInc(pNode1, pNode2->m_pRightChild);
	}
	if (pNode2 == m_pRootNode) {
		pNode1->m_rcNode.right += pNode1->m_iRectRightInc;
		pNode1->m_rcNode.bottom += pNode1->m_iRectBottomInc;
		pNode1->m_iRectRightInc = 0;
		pNode1->m_iRectBottomInc = 0;
	}
}
