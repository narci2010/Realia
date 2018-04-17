
#include "NodeEntity.h"

CNoneNode::CNoneNode()
{
	m_bEditMode = false;
	m_iNodeType = NT_NONE;
	//lstrcpy(m_strText, _T("NoneNode"));
}

CNoneNode::CNoneNode(POINT point)
{
	m_bEditMode = false;
	m_iNodeType = NT_NONE;
	//lstrcpy(m_strText, _T("NoneNode"));
	m_rcNode = { point.x, point.y, point.x, point.y };
}

CEditNode::CEditNode()
{
	m_bEditMode = true;
}

CEditNode::CEditNode(UINT iNodeType, POINT point)
{
	m_bEditMode = true;
	m_iNodeType = iNodeType;
	UpdateRect(point);
}

CEditNode::CEditNode(UINT iNodeType, LPCTSTR strText, POINT point)
{
	m_bEditMode = true;
	m_iNodeType = iNodeType;
	lstrcpy(m_strText, strText);
	UpdateRect(point);
}

CEditNode::CEditNode(UINT iNodeType, int iFontSize, POINT point)
{
	m_bEditMode = true;
	m_iNodeType = iNodeType;
	m_iFontSize = iFontSize;
	UpdateRect(point);
}

CEditNode::~CEditNode()
{

}


COperatorNode::COperatorNode()
{
	m_bEditMode = false;
}

COperatorNode::COperatorNode(UINT iNodeType, POINT point)
{
	m_bEditMode = false;
	m_iNodeType = iNodeType;
	lstrcpy(m_strText, GetOperatorTextByNodeType(iNodeType));
	UpdateRect(point);
}

COperatorNode::~COperatorNode()
{

}


CFractionalNode::CFractionalNode()
{
	m_bEditMode = false;
	m_iNodeType = NT_FRACTION;
}

CFractionalNode::CFractionalNode(int iFontSize, POINT point)
{
	m_bEditMode = false;
	m_iNodeType = NT_FRACTION;
	m_iFontSize = iFontSize;
	UpdateRect(point);
}

CFractionalNode::~CFractionalNode()
{

}

void CFractionalNode::DrawNode(Graphics* pGraphics)
{
	Pen pen(m_crColor, REAL(m_iFontSize / 10));
	pGraphics->DrawLine(&pen, m_rcNode.left, m_rcNode.top, m_rcNode.right, m_rcNode.top);
}
