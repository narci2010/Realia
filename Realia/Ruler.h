#pragma once

#include "stdafx.h"
#include "Realia.h"

class CRuler : public CRealia
{
public:
	CRuler();
	CRuler(UINT iHeight);

	virtual int HitTest(POINT pt) const;
	
public:
	virtual ~CRuler();

protected:
	int m_iHeight;

	virtual void AdjustRgn(int nHandle, POINT ptBegin, POINT ptEnd);
	virtual void DrawInside(HDC dc, POINT pt1, POINT pt2) const;

private:
	void DrawRuler(HDC dc, POINT pt1, POINT pt2, const int iHeight = 50) const;
};
