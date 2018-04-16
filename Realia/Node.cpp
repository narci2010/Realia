
#include "Node.h"

CNode::CNode()
{
	m_pParent = NULL;
	m_pLeftChild = NULL;
	m_pRightChild = NULL;

	m_bEditMode = true;

	m_iRectLeftInc = 0;
	m_iRectRightInc = 0;
	m_iRectTopInc = 0;
	m_iRectBottomInc = 0;

	m_iNodeType = 0;
	::ZeroMemory(&m_strText, sizeof(m_strText));
	::ZeroMemory(&m_strName, sizeof(m_strName));

	m_crColor = 0xFF000000;// ::GetSysColor(COLOR_WINDOWTEXT);
	m_iFontSize = 20;

	m_lf.lfHeight = -m_iFontSize;
	m_lf.lfWidth = 0;
	m_lf.lfEscapement = 0;
	m_lf.lfOrientation = 0;
	m_lf.lfWeight = FW_NORMAL;
	m_lf.lfItalic = FALSE;
	m_lf.lfUnderline = FALSE;
	m_lf.lfStrikeOut = FALSE;
	m_lf.lfCharSet = ANSI_CHARSET;//GB2312_CHARSET;
	m_lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_lf.lfQuality = DEFAULT_QUALITY;
	m_lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	lstrcpy(m_lf.lfFaceName, kFontFamily);

	::ZeroMemory(&m_rcNode, sizeof(m_rcNode));
}

CNode::~CNode()
{
	if (m_pLeftChild)
		delete m_pLeftChild;
	if (m_pRightChild)
		delete m_pRightChild;
}

//CNode& CNode::operator=(const CNode& other)
//{
//	if (this != &other) {
//		delete m_pParent;
//		m_pParent = other.m_pParent;
//		lstrcpy(m_strText, other.m_strText);
//	}
//	return *this;
//}

UINT CNode::GetNodeType()
{
	return m_iNodeType;
}

CNode* CNode::GetParentNode()
{
	return m_pParent;
}

void CNode::SetParentNode(CNode* pNode)
{
	m_pParent = pNode;
}

void CNode::SetLeftChild(CNode* pNode)
{
	m_pLeftChild = pNode;
	if (m_pLeftChild)
	{
		m_pLeftChild->m_pParent = this;
		//m_pLeftChild->m_nSubLevel = m_nSubLevel + m_nSubLevelLeftInc; // inherit the SubLevel
		//m_pLeftChild->FormatNode(GetFormat());
	}
}

CNode* CNode::GetLeftChild()
{
	return m_pLeftChild;
}

void CNode::SetRightChild(CNode* pNode)
{
	m_pRightChild = pNode;
	if (m_pRightChild)
	{
		m_pRightChild->m_pParent = this;
		//m_pRightChild->m_nSubLevel = m_nSubLevel + m_nSubLevelRightInc; // inherit the SubLevel
		//m_pRightChild->FormatNode(GetFormat());
	}
}

CNode* CNode::GetRightChild()
{
	return m_pRightChild;
}

void CNode::SetText(LPCTSTR strText)
{
	lstrcpy(m_strText, strText);
}

LPCTSTR CNode::GetText()
{
	return m_strText;
}

void CNode::SetEditMode(bool bEditMode)
{
	m_bEditMode = bEditMode;
}

bool CNode::GetEditMode()
{
	return m_bEditMode;
}

void CNode::SetColor(COLORREF crColor)
{
	m_crColor = crColor;
}

COLORREF CNode::GetColor()
{
	return  m_crColor;
}

void CNode::SetRectRightInc(int iRectRightInc)
{
	m_iRectRightInc = iRectRightInc;
}

void CNode::SetRectBottomInc(int iRectBottomInc)
{
	m_iRectBottomInc = iRectBottomInc;
}

void CNode::SetFaceName(LPCTSTR strFaceName)
{
	lstrcpy(m_lf.lfFaceName, strFaceName);
}

LPCTSTR CNode::GetFaceName()
{
	return m_lf.lfFaceName;
}

LOGFONT* CNode::GetLogFont()
{
	return &m_lf;
}

void CNode::SetFontSize(int iFontSize)
{
	m_iFontSize = iFontSize;
}

BOOL CNode::IsNodeLeaf()
{
	if (GetLeftChild() || GetRightChild())
		return FALSE;
	else
		return TRUE;
}

RECT CNode::GetRect()
{
	return m_rcNode;
}

LPCTSTR CNode::GetOperatorTextByNodeType(UINT iNodeType)
{
	TCHAR* strOperator;
	switch (iNodeType)
	{
	case NT_PLUS:
		strOperator = _T("+");
		break;
	case NT_MINUS:
		strOperator = _T("-");
		break;
	case NT_MULTIPLY:
		strOperator = _T("*");
		break;
	case NT_DIVIDE:
		strOperator = _T("/");
		break;
	case NT_EQUATION:
		strOperator = _T("=");
		break;
	default:
		strOperator = _T("");
		break;
	}
	return strOperator;
}

void CNode::UpdateRect(POINT point)
{
	SIZE size = GetTextExtent(lstrlen(m_strText) != 0 ? m_strText : _T("123"), m_lf.lfFaceName, m_iFontSize);
	m_rcNode.left = point.x;
	m_rcNode.top = point.y;
	m_rcNode.right = point.x;
	m_rcNode.bottom = point.y;
	m_iRectRightInc = lstrlen(m_strText) != 0 ? size.cx : 1;
	m_iRectBottomInc = size.cy;
}

// draw the node pre-order 
// first myself
// second the left child 
// third the right child
void CNode::DrawNode(Graphics* pGraphics)
{
	//Font font(pGraphics->GetHDC(), &m_lf);//用LOGFONT指定字体时需要HDC，因为此处用gdi+画在内存位图上的，找不到HDC的
	Font font(kFontFamily, m_iFontSize, FontStyleRegular, UnitPixel);//默认的UnitPoint与RichEdit显示的字体大小不一致
	SolidBrush brush(m_crColor);
	
	pGraphics->SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);//消除锯齿，使用网格匹配
	//pGraphics->SetTextRenderingHint(TextRenderingHintAntiAlias);//消除锯齿，不使用网格匹配 
	pGraphics->DrawString(m_strText, lstrlen(m_strText), &font, PointF(m_rcNode.left - 3, m_rcNode.top), &brush);

	Pen pen(m_crColor, 1.5f);
	pen.SetDashStyle(DashStyleDot);
	pGraphics->DrawRectangle(&pen, m_rcNode.left, m_rcNode.top, m_rcNode.right - m_rcNode.left, m_rcNode.bottom - m_rcNode.top);
}
