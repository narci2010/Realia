
#include "Node.h"

CNode::CNode()
{
	::ZeroMemory(&m_strName, sizeof(m_strName));
	m_pParent = NULL;
	m_pLeftChild = NULL;
	m_pRightChild = NULL;
	::ZeroMemory(&m_rcNode, sizeof(m_rcNode));

	//m_dwEditMode = NE_ALLOWDELETE;

	m_nSubLevel = 0;
	m_nSubLevelRightInc = 0;
	m_nSubLevelLeftInc = 0;

	m_iNodeType = 0;
	m_crColor = ::GetSysColor(COLOR_WINDOWTEXT);

	//m_lf.lfHeight = GetFormat()->GetFontHeight(FH_STANDARD) * 10;//100;
	//m_lf.lfWidth = 0;
	//m_lf.lfEscapement = 0;
	//m_lf.lfOrientation = 0;
	//m_lf.lfWeight = FW_NORMAL;
	//m_lf.lfItalic = TRUE;
	//m_lf.lfUnderline = FALSE;
	//m_lf.lfStrikeOut = FALSE;
	//m_lf.lfCharSet = ANSI_CHARSET;
	//m_lf.lfOutPrecision = OUT_CHARACTER_PRECIS;
	//m_lf.lfClipPrecision = CLIP_CHARACTER_PRECIS;
	//m_lf.lfQuality = ANTIALIASED_QUALITY;
	//m_lf.lfPitchAndFamily = FF_DECORATIVE;
	//strcpy(m_lf.lfFaceName, "Times New Roman");

	//for (int i = 0; i<LT_COUNT; i++)
	//	m_strCodeFormat[i] = _T("%s %s %s");
}

CNode::~CNode()
{
	if (m_pLeftChild)
		delete m_pLeftChild;
	if (m_pRightChild)
		delete m_pRightChild;
}

// draw the node pre-order 
// first myself
// second the left child 
// third the right child
void CNode::DrawNode(HDC pDC)
{
	HFONT hFont = CreateFont(
		-20,                   // nHeight  
		0,                         // nWidth  
		0,                         // nEscapement  
		0,                         // nOrientation  
		FW_NORMAL,                 // nWeight  
		FALSE,                     // bItalic  
		FALSE,                     // bUnderline  
		0,                         // cStrikeOut  
		ANSI_CHARSET,              // nCharSet  
		OUT_DEFAULT_PRECIS,        // nOutPrecision  
		CLIP_DEFAULT_PRECIS,       // nClipPrecision  
		DEFAULT_QUALITY,           // nQuality  
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily  
		_T("Î¢ÈíÑÅºÚ"));           // lpszFacename 
	SetTextColor(pDC, RGB(0, 0, 128));
	SelectObject(pDC, hFont);
	SetBkMode(pDC, TRANSPARENT);

	DrawText(pDC, m_strText, lstrlen(m_strText), &m_rcNode, DT_LEFT);

	::DeleteObject(hFont);
}
