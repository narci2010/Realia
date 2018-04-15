#pragma once

#include "stdafx.h"

DWORD DistanceBetweenPointAndRect(RECT rc, POINT pt);//获取点到矩形的最短距离
int CDuiStringToInt(CDuiString str);//CDuiString -> int
SIZE GetTextExtent(LPCTSTR lpszString, LPCTSTR lpszFontFamily, int iFontSize);//获取字符串的长度和高度

