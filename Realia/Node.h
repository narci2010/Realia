#pragma once

#include "stdafx.h"


// Node Types
#define NT_STANDARD		0x01	// standard-node
#define NT_DIVISION		0x02	// x over y
#define NT_NTHROOT		0x03	// n-th root of anything
#define NT_ROOT			0x04	// squareroot
#define NT_POWERTO		0x05	// x powered by y
#define NT_PLACEHOLDER	0x06    // placeholder for nodes
#define NT_BRACE		0x07	// Brackes {x+y}
#define NT_PLUS			0x08	// x + y
#define NT_MINUS		0x09	// x - y
#define NT_TIMES		0x0A	// x * y
#define NT_NUMBER		0x0B	// 3,1 and so on
#define NT_EQUATION		0x0C	// equations
#define NT_PARTDERIVE	0x0D	// part. d/dt
#define NT_DERIVE		0x0E	// d/dt fkt(t)
#define NT_SUM			0x0F	// Sum from i=0;to i=10, over fkt(i)
#define NT_PROD			0x10	// Product from i=0;to i=10, over fkt(i)
#define NT_RANGE		0x11	// Upper and lower range
#define NT_INTEGRAL		0x12	// Integral from i=0;to i=10, over fkt(t) dt
#define NT_INTEGRAND	0x13	// Integrandvariable and function
#define NT_FUNC			0x14	// like sin, cos, tan
#define NT_EXTFUNC		0x15	// external functions
#define NT_USERFUNC		0x16	// like f(x)
#define NT_POISON		0x17	// like (a,b) {a,b}
#define NT_INDEX		0x18	// Subscript Node
#define NT_VECTOR		0x19	// Overline like vector...
#define NT_CONSTANT		0x1A	// Constant like pi, c...
#define NT_VARIABLE		0x1B	// variable like m(mass)...
#define NT_LIMES		0x1C	// lim(x->oo) f(x)
#define NT_OPERATOR		0x1D	// x->oo
#define NT_BINOMIAL		0x1E	// n over k
#define NT_MATRIX		0x1F	// Matrixnode
#define NT_LINE			0x20	// line in a matrix
#define NT_ELEMENT		0x21	// Elementnode in a matrix
#define NT_NABLA		0x22	// Nablaoperator
#define NT_LAPLACE		0x23	// Laplaceoperator
#define NT_CROSS		0x24	// Crossproduct of a vector
#define NT_ARROWS		0x25	// ->, =>
#define NT_INFINITY		0x26	// Infinity oo
#define NT_PLANCK		0x27	// Planck h
#define NT_LAMBDA		0x28	// Lambda
#define NT_LABEL		0x29	// Label for vars and consts

class CNode
{
public:
	CNode();
	virtual ~CNode();

	// pointer to parent
	CNode* m_pParent;
	// pointer to left child
	CNode* m_pLeftChild;
	// pointer to right child
	CNode* m_pRightChild;

	// sublevel (subscript, sub-subscript ...)
	int m_nSubLevel;
	// the increments of the sublevel for the children
	int m_nSubLevelLeftInc;
	int m_nSubLevelRightInc;

	// node type (one of the NT_* defines)
	UINT m_iNodeType;
	// the shortcut of the node (sin for sinus)
	TCHAR m_strText[MAX_PATH];
	// the name of the node (sinus for sinus)
	TCHAR m_strName[MAX_PATH];
	// color of the node (modified by the formatter class)
	COLORREF m_crColor;
	// LOGFONT node (modified by the formatter class)
	LOGFONT m_lf;

	// the destination rectangle for the ouput
	RECT m_rcNode;

	// Sets the parent node directly
	void SetParentNode(CNode* pNode);
	// Returns the parent node
	CNode* GetParentNode();

	// drawing functions
	virtual void DrawNode(HDC pDC);

};
