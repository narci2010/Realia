#pragma once

#include "Node.h"

//�ɱ༭���-��ͨ������
class CEditNode : public CNode
{
public:
	CEditNode();
	CEditNode(UINT iNodeType, POINT point);
	CEditNode(UINT iNodeType, int iFontSize, POINT point);
	virtual ~CEditNode();


};

//��������-��+-*/�Ȳ��ɱ༭��Ҳ�����ֻ�Ľ��
class COperatorNode : public CNode
{
public:
	COperatorNode();
	COperatorNode(UINT iNodeType, POINT point);
	virtual ~COperatorNode();


};

//����Ϊ�ֻ���������
//��ʽ���
class CFractionalNode : public CNode
{
public:
	CFractionalNode();
	CFractionalNode(UINT iNodeType, int iFontSize, POINT point);
	~CFractionalNode();
};
