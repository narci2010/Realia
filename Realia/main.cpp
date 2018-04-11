#include "stdafx.h"
#include "resource.h"
#include "RealiaWnd.h"
#include "FormulaWnd.h"

//全局变量
HINSTANCE hInst;								// 当前实例
CRealiaWnd m_CRealiaWnd;
CFormulaWnd* pFormulaWnd = NULL;

// 必须要进行前导声明
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 程序入口点
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//GDI+开启  
	GdiplusStartupInput StartupInput;
	ULONG_PTR m_gdiplusToken;
	//StartupInput.GdiplusVersion = 1;
	if (GdiplusStartup(&m_gdiplusToken, &StartupInput, NULL))MessageBox(0, TEXT("GdiPlus开启失败"), TEXT("错误"), MB_ICONERROR);

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr)) return false;
	Hr = ::OleInitialize(NULL);
	if (FAILED(Hr)) return false;

	HINSTANCE hInstRich = ::LoadLibrary(_T("Riched20.dll"));

	// 类名
	TCHAR* szWindowClass = _T("Realia");

	hInst = hInstance; // 将实例句柄存储在全局变量中

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
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_REALIA);
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

	::FreeLibrary(hInstRich);

	::OleUninitialize();
	::CoUninitialize();

	//GDI+关闭  
	GdiplusShutdown(m_gdiplusToken);//可以把这个写在消息循环后面，程序退出就销毁，或者在不需要GDI+时调用，比如GDI+窗口的WM_DESTROY消息里调用

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
		case IDM_FORMULA: {
			//if (pFormulaWnd == NULL) {
				pFormulaWnd = CFormulaWnd::Instance();
			//}

			CPaintManagerUI::SetInstance(hInst);
			CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

			pFormulaWnd->Create(hWnd, _T("公式编辑器"), WS_OVERLAPPEDWINDOW, 0, 0, 0, 800, 600);

			pFormulaWnd->SetParentWnd(hWnd);

			// 显示窗口
			pFormulaWnd->CenterWindow();
			ShowWindow(*pFormulaWnd, SW_SHOW);
			//CPaintManagerUI::MessageLoop();

			// 更新窗口
			//UpdateWindow(*pFormulaWnd);			
		}
			break;
		case IDM_DEL:
			m_CRealiaWnd.DeleteRealia();
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

		assert(SaveDC(hdc) != 0);
		m_CRealiaWnd.OnPaint(hdc);
		RestoreDC(hdc, -1);

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
