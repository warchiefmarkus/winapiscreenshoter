#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <wininet.h>
#include <stdio.h>
#include <fstream>

using namespace std;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);

void screenshot(char* filename) {
	HDC hdc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	if (hdc == NULL) {
		printf("Couldn't create device context\n");
		return;
	}

	DWORD dwWidth, dwHeight, dwBPP, dwNumColors;
	//dwWidth  = GetDeviceCaps(hdc, HORZRES);
	//dwHeight = GetDeviceCaps(hdc, VERTRES);
	dwWidth = 1280;
	dwHeight = 1024;
	dwBPP = GetDeviceCaps(hdc, BITSPIXEL);
	if (dwBPP <= 8) {
		dwNumColors = GetDeviceCaps(hdc, NUMCOLORS);
		dwNumColors = 256;
	}
	else {
		dwNumColors = 0;
	}

	// Create compatible DC.
	HDC hdc2 = CreateCompatibleDC(hdc);
	if (hdc2 == NULL) {
		DeleteDC(hdc);
		printf("Couldn't create compatible device context\n");
		return;
	}

	// Create bitmap.
	LPVOID pBits;
	HBITMAP bitmap;
	BITMAPINFO bmInfo;

	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = dwWidth;
	bmInfo.bmiHeader.biHeight = dwHeight;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = (WORD)dwBPP;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	bmInfo.bmiHeader.biSizeImage = 0;
	bmInfo.bmiHeader.biXPelsPerMeter = 0;
	bmInfo.bmiHeader.biYPelsPerMeter = 0;
	bmInfo.bmiHeader.biClrUsed = dwNumColors;
	bmInfo.bmiHeader.biClrImportant = dwNumColors;

	bitmap = CreateDIBSection(hdc, &bmInfo, DIB_PAL_COLORS, &pBits, NULL, 0);
	if (bitmap == NULL) {
		DeleteDC(hdc);
		DeleteDC(hdc2);
		printf("Couldn't create compatible bitmap\n");
		return;
	}

	HGDIOBJ gdiobj = SelectObject(hdc2, (HGDIOBJ)bitmap);
	if ((gdiobj == NULL) || (gdiobj == (void*)(LONG_PTR)GDI_ERROR)) {
		DeleteDC(hdc);
		DeleteDC(hdc2);
		printf("Couldn't select bitmap\n");
		return;
	}
	if (!BitBlt(hdc2, 0, 0, dwWidth, dwHeight, hdc, 0, 0, SRCCOPY)) {
		DeleteDC(hdc);
		DeleteDC(hdc2);
		printf("Could not copy bitmap\n");
		return;
	}

	RGBQUAD colors[256];
	if (dwNumColors != 0)
		dwNumColors = GetDIBColorTable(hdc2, 0, dwNumColors, colors);

	// Fill in bitmap structures.
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bitmapinfoheader;

	bmfh.bfType = 0x04D42;
	bmfh.bfSize = ((dwWidth * dwHeight * dwBPP) / 8) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (dwNumColors * sizeof(RGBQUAD));
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (dwNumColors * sizeof(RGBQUAD));
	bitmapinfoheader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapinfoheader.biWidth = dwWidth;
	bitmapinfoheader.biHeight = dwHeight;
	bitmapinfoheader.biPlanes = 1;
	bitmapinfoheader.biBitCount = (WORD)dwBPP;
	bitmapinfoheader.biCompression = BI_RGB;
	bitmapinfoheader.biSizeImage = 0;
	bitmapinfoheader.biXPelsPerMeter = 0;
	bitmapinfoheader.biYPelsPerMeter = 0;
	bitmapinfoheader.biClrUsed = dwNumColors;
	bitmapinfoheader.biClrImportant = 0;

	ofstream file;
	file.open("image.bmp", ios::binary | ios::trunc | ios::out);
	file.write((char*)& bmfh, sizeof(BITMAPFILEHEADER));
	file.write((char*)& bitmapinfoheader, sizeof(BITMAPINFOHEADER));

	if (dwNumColors != 0)
		file.write((char*)colors, sizeof(RGBQUAD) * dwNumColors);
	file.write((char*)pBits, (dwWidth * dwHeight * dwBPP) / 8);

	DeleteObject(bitmap);
	DeleteDC(hdc2);
	DeleteDC(hdc);
}

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE         hPrevInstance,
	LPSTR             lpCmdLine,
	int               nCmdShow)
{

	LPCTSTR lpzClass = TEXT("My Window Class");
	if (!RegMyWindowClass(hInstance, lpzClass))
		return 1;


	RECT screen_rect;
	GetWindowRect(GetDesktopWindow(), &screen_rect); 
	int x = screen_rect.right / 2 - 150;
	int y = screen_rect.bottom / 2 - 75;


	HWND hWnd = CreateWindow(lpzClass, TEXT("WINAPI SCREENSHOTER"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, x, y, 560, 100, NULL, NULL,
		hInstance, NULL);

	// ���� ���� �� �������, ��������� ����� ����� 0
	if (!hWnd) return 2;

	CreateWindow("button", "Screenshot", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		370, 10, 160, 30, hWnd, (HMENU)10000, hInstance, NULL);

	CreateWindowEx(WS_EX_CLIENTEDGE, "edit", "", WS_CHILD | WS_VISIBLE | ES_RIGHT,
		10, 10, 350, 30, hWnd, (HMENU)10001, hInstance, NULL);


	MSG msg = { 0 };   
	int iGetOk = 0;   
	while ((iGetOk = GetMessage(&msg, NULL, 0, 0)) != 0) 
	{
		if (iGetOk == -1) return 3;  
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam; 

}

LRESULT CALLBACK WndProc(
	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// ������� � ��������� ���������
	switch (message)
	{
	case WM_LBUTTONUP:
		// ������� �� ���������
		//MessageBox(hWnd, TEXT("�� ��������!"), TEXT("�������"), 0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);  // ������� �� ���������
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == 10000)
		{
			HWND filedit = GetDlgItem(hWnd, 10001); 		
			char txt[1024];			
			GetWindowText(filedit, txt, sizeof(txt));	
			screenshot(txt);
			//MessageBox(hWnd, txt, "������ ������", 0);
		}
		break;
	default:
		// ��� ��������� �� ������������ ���� ���������� ���� Windows
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

ATOM RegMyWindowClass(HINSTANCE hInst, LPCTSTR lpzClassName)
{
	WNDCLASS wcWindowClass = { 0 };
	// ����� �-��� ��������� ���������
	wcWindowClass.lpfnWndProc = (WNDPROC)WndProc;
	// ����� ����
	wcWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	// ���������� ���������� ����������
	wcWindowClass.hInstance = hInst;
	// �������� ������
	wcWindowClass.lpszClassName = lpzClassName;
	// �������� �������
	wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	// �������� ����� ����
	wcWindowClass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	return RegisterClass(&wcWindowClass); // ����������� ������
}
