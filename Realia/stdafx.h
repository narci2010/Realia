// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

//#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>
#include <tchar.h>
#include <assert.h>

#include <gdiplus.h>
using namespace Gdiplus;

#pragma comment(lib, "gdiplus.lib")

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#endif


#include <UIlib.h>

using namespace DuiLib;

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_ud.lib")
#   else
#       pragma comment(lib, "DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_u.lib")
#   else
#       pragma comment(lib, "DuiLib.lib")
#   endif
#endif

//��ʽ����ȷ�ϻ�ȡ��
#define FORMULAWND_OK			(WM_USER + 1)
#define FORMULAWND_CANCEL		(WM_USER + 2)

//д���Ӳ˵����ڵĿؼ�ID
#define MENUWNDCMD_BTNNUMBER0	(WM_USER + 100)
#define MENUWNDCMD_BTNNUMBER1	(WM_USER + 101)
#define MENUWNDCMD_BTNNUMBER2	(WM_USER + 102)
#define MENUWNDCMD_BTNNUMBER3	(WM_USER + 103)
#define MENUWNDCMD_BTNNUMBER4	(WM_USER + 104)
#define MENUWNDCMD_BTNNUMBER5	(WM_USER + 105)
#define MENUWNDCMD_BTNNUMBER6	(WM_USER + 106)
#define MENUWNDCMD_BTNNUMBER7	(WM_USER + 107)
#define MENUWNDCMD_BTNNUMBER8	(WM_USER + 108)
#define MENUWNDCMD_BTNNUMBER9	(WM_USER + 109)
#define MENUWNDCMD_BTNPOINT		(WM_USER + 110)
#define MENUWNDCMD_BTNPLUS		(WM_USER + 111)
#define MENUWNDCMD_BTNMINUS		(WM_USER + 112)
#define MENUWNDCMD_BTNMULTIPLY	(WM_USER + 113)
#define MENUWNDCMD_BTNDIVIDE	(WM_USER + 114)
#define MENUWNDCMD_BTNEQUATION	(WM_USER + 115)

