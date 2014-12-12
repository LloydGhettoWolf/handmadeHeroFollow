//main.cpp
//entry point file
//last update - 10/12/2014
#include <Windows.h>

LRESULT CALLBACK WindowProcCallback(
	HWND window,
	UINT message,
	WPARAM WParam,
	LPARAM LParam)
{
	LRESULT result = 0;

	switch (message){
		case WM_CREATE:
			OutputDebugString(L"WM_CREATE\n");
			break;

		case WM_SIZE:
			OutputDebugString(L"WM_SIZE\n");
			break;

		case WM_DESTROY:
			DestroyWindow(window);
			OutputDebugString(L"WM_DESTROY\n");
			break;

		case WM_CLOSE:
			DestroyWindow(window);
			OutputDebugString(L"WM_CLOSE\n");
			break;

		case WM_ACTIVATEAPP:
			OutputDebugString(L"WM_ACTIVATEAPP\n");
			break;

		case WM_PAINT:
		{
			PAINTSTRUCT paint;
			HDC devContext = BeginPaint(window, &paint);
			int height = paint.rcPaint.bottom - paint.rcPaint.top;
			int width = paint.rcPaint.right - paint.rcPaint.left;
			int top  = paint.rcPaint.top;
			int left = paint.rcPaint.left;
			PatBlt(devContext, left,top, width, height, WHITENESS);
			EndPaint(window, &paint);
			OutputDebugString(L"WM_PAINT\n");
		}
			break;

		default:
			OutputDebugString(L"DEFAULT");
			result = DefWindowProc(window,message,WParam,LParam);
			break;
	}

	return result;
}

int CALLBACK WinMain(
	  HINSTANCE instance,
	  HINSTANCE prevInstance,
	  LPSTR cmdLine,
	  int cmdShow)
{
	WNDCLASS windowClass = {};

	windowClass.style		  = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc   = WindowProcCallback;
	windowClass.hInstance	  = instance;
	windowClass.lpszClassName = L"myGameWindowClass";

	if (RegisterClass(&windowClass)){
		HWND winHandle = CreateWindowEx(
				0,
				windowClass.lpszClassName,
				L"Lloyd o tron",
				WS_OVERLAPPEDWINDOW|WS_VISIBLE,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				0,
				0,
				instance,
				0
			);
		if (winHandle){
			MSG message;
			for (;;){
				BOOL messageResult = GetMessage(&message, 0, 0, 0);
				if (messageResult > 0){
					TranslateMessage(&message);
					DispatchMessage(&message);
				}else{
					break;
				}
			}
		}else{

		}
	}
	return 0;
}