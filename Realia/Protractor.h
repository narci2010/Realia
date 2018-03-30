#pragma once

#include "stdafx.h"
#include "Realia.h"

class CProtractor : public CRealia
{
public:
	CProtractor();
	CProtractor(UINT iHeight);

	virtual int HitTest(POINT pt) const;

public:
	virtual ~CProtractor();

protected:
	int m_iHeight;

	virtual void AdjustRgn(int nHandle, POINT ptBegin, POINT ptEnd);
	virtual void DrawInside(HDC dc, POINT pt1, POINT pt2) const;

private:
	void DrawRuler(HDC dc, POINT pt1, POINT pt2, const int iHeight = 50) const;
};
