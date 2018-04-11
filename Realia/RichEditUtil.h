#pragma once

#include "stdafx.h"

static DWORD CALLBACK streamInCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
BOOL ReadFileByStreamIn(CRichEditUI* hRichEdit, LPCTSTR pszFileName);
BOOL InsertObject(CRichEditUI* hRichEdit, LPCTSTR pszFileName);
