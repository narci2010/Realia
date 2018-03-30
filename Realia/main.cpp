#include "stdafx.h"
#include "resource.h"
#include "RealiaWnd.h"

//全局变量
CRealiaWnd m_CRealiaWnd;

// 必须要进行前导声明
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 程序入口点
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// 类名
	TCHAR* szWindowClass = _T("Realia");

	// 设计窗口类
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);;
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = NULL;

	// 注册窗口类
	RegisterClassEx(&wcex);

	// 创建窗口
	HWND hWnd = CreateWindow(
		szWindowClass,		//类名，要和刚才注册的一致
		szWindowClass,  	//窗口标题文字
		WS_OVERLAPPEDWINDOW,//窗口外观样式
		CW_USEDEFAULT,		//窗口相对于父级的X坐标
		CW_USEDEFAULT,		//窗口相对于父级的Y坐标
		CW_USEDEFAULT,		//窗口的宽度
		CW_USEDEFAULT,		//窗口的高度
		NULL,				//没有父窗口，为NULL
		NULL,				//没有菜单，为NULL
		hInstance,			//当前应用程序的实例句柄
		NULL);				//没有附加数据，为NULL
	if (!hWnd)
	{
		return FALSE;
	}

	//加载菜单
	HMENU hMenu;
	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
	SetMenu(hWnd, hMenu);

	m_CRealiaWnd.InitWindow(hWnd);

	// 显示窗口
	ShowWindow(hWnd, SW_SHOW);

	// 更新窗口
	UpdateWindow(hWnd);

	// 消息循环
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	POINT point;
	UINT nHitTest;
	switch (uMsg)
	{
	case WM_COMMAND:
		int wmId, wmEvent;
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_RULER:
			m_CRealiaWnd.SetRealiaType(1);
			break;
		case IDM_ACUTE:
			m_CRealiaWnd.SetRealiaType(2);
			break;
		case IDM_ISOSCELES:
			m_CRealiaWnd.SetRealiaType(3);
			break;
		case IDM_PROTRACTOR:
			m_CRealiaWnd.SetRealiaType(4);
			break;
		case IDM_GONIOMETER:
			m_CRealiaWnd.SetRealiaType(5);
			break;
		case IDM_COMPASS:
			m_CRealiaWnd.SetRealiaType(6);
			break;
		case IDCLOSE:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...

		m_CRealiaWnd.OnPaint(hdc);

		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		m_CRealiaWnd.OnLButtonDown(point);
		break;
	case WM_LBUTTONUP:
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		m_CRealiaWnd.OnLButtonUp(point);
		break;
	case WM_MOUSEMOVE:
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		m_CRealiaWnd.OnMouseMove(point);
		break;
	case WM_SETCURSOR:
		nHitTest = LOWORD(lParam);
		m_CRealiaWnd.OnSetCursor(hWnd, nHitTest);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
			return 0;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
