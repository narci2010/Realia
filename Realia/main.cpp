#include "stdafx.h"
#include "resource.h"
#include "RealiaWnd.h"

//ȫ�ֱ���
CRealiaWnd m_CRealiaWnd;

// ����Ҫ����ǰ������
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ������ڵ�
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// ����
	TCHAR* szWindowClass = _T("Realia");

	// ��ƴ�����
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

	// ע�ᴰ����
	RegisterClassEx(&wcex);

	// ��������
	HWND hWnd = CreateWindow(
		szWindowClass,		//������Ҫ�͸ղ�ע���һ��
		szWindowClass,  	//���ڱ�������
		WS_OVERLAPPEDWINDOW,//���������ʽ
		CW_USEDEFAULT,		//��������ڸ�����X����
		CW_USEDEFAULT,		//��������ڸ�����Y����
		CW_USEDEFAULT,		//���ڵĿ��
		CW_USEDEFAULT,		//���ڵĸ߶�
		NULL,				//û�и����ڣ�ΪNULL
		NULL,				//û�в˵���ΪNULL
		hInstance,			//��ǰӦ�ó����ʵ�����
		NULL);				//û�и������ݣ�ΪNULL
	if (!hWnd)
	{
		return FALSE;
	}

	//���ز˵�
	HMENU hMenu;
	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
	SetMenu(hWnd, hMenu);

	m_CRealiaWnd.InitWindow(hWnd);

	// ��ʾ����
	ShowWindow(hWnd, SW_SHOW);

	// ���´���
	UpdateWindow(hWnd);

	// ��Ϣѭ��
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
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
		// �����˵�ѡ��:
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
		// TODO: �ڴ���������ͼ����...

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
