
#include "NodeEntity.h"

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
}

CFractionalNode::CFractionalNode(UINT iNodeType, int iFontSize, POINT point)
{
	m_bEditMode = false;
	m_iNodeType = iNodeType;
	m_iFontSize = iFontSize;
	UpdateRect(point);
}

CFractionalNode::~CFractionalNode()
{

}
