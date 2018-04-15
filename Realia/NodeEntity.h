#pragma once

#include "Node.h"

//可编辑结点-普通输入结点
class CEditNode : public CNode
{
public:
	CEditNode();
	CEditNode(UINT iNodeType, POINT point);
	CEditNode(UINT iNodeType, int iFontSize, POINT point);
	virtual ~CEditNode();


};

//运算符结点-如+-*/等不可编辑，也无需手绘的结点
class COperatorNode : public CNode
{
public:
	COperatorNode();
	COperatorNode(UINT iNodeType, POINT point);
	virtual ~COperatorNode();


};

//以下为手绘运算符结点
//分式结点
class CFractionalNode : public CNode
{
public:
	CFractionalNode();
	CFractionalNode(UINT iNodeType, int iFontSize, POINT point);
	~CFractionalNode();
};
