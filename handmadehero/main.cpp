//main.cpp
//entry point file
//last update - 10/12/2014
#include <Windows.h>

static bool running;
static BITMAPINFO bmInfo;
static void* bitmapMem;
static int bitmapWidth;
static int bitmapHeight;

void Render(unsigned int xOffset,unsigned int yOffset){
	int Pitch = bitmapWidth * 4;
	unsigned int* pixel = 0;
	unsigned char* row = (unsigned char*)bitmapMem;
	for (unsigned int Y = 0; Y < bitmapHeight; ++Y){
		pixel = (unsigned int*)row;
		for (unsigned int X = 0; X < bitmapWidth; ++X){
			unsigned char r, g, b;
			b = xOffset + X;
			g = Y;
			r = 0;

			*pixel = ((r << 16) | (g << 8) | b);
			++pixel;
		}
		row += Pitch;
	}
}

void ResizeDIBSection(int width, int height){

	int videoMemoryRequired = 4 * width * height;

	if (bitmapMem){
		VirtualFree(bitmapMem, videoMemoryRequired, MEM_RELEASE);
	}

	bitmapWidth  = width;
	bitmapHeight = height;
	
	bmInfo.bmiHeader.biSize			= sizeof(bmInfo.bmiHeader);
	bmInfo.bmiHeader.biWidth		= bitmapWidth;
	bmInfo.bmiHeader.biHeight		= -bitmapHeight;
	bmInfo.bmiHeader.biPlanes		= 1;
	bmInfo.bmiHeader.biBitCount		= 32;
	bmInfo.bmiHeader.biCompression  = BI_RGB;
	bmInfo.bmiHeader.biSizeImage	= 0;
	bmInfo.bmiHeader.biXPelsPerMeter = 0;
	bmInfo.bmiHeader.biYPelsPerMeter = 0;
	bmInfo.bmiHeader.biClrUsed		 = 0;
	bmInfo.bmiHeader.biClrImportant  = 0;

	
	bitmapMem = VirtualAlloc(0,videoMemoryRequired,MEM_COMMIT,PAGE_READWRITE);

}

void UpdateWindow(HDC context, int x,int y,int width,int height){
	StretchDIBits(
		context,
		0,0,bitmapWidth,bitmapHeight,
		0,0,width,height,
		bitmapMem,
		&bmInfo,
		DIB_RGB_COLORS,
		SRCCOPY);
}

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
		{
			RECT resizeRect;
			GetClientRect(window, &resizeRect);
			int height = resizeRect.bottom - resizeRect.top;
			int width = resizeRect.right - resizeRect.left;
			ResizeDIBSection(width, height);
			OutputDebugString(L"WM_SIZE\n");
		}
				break;

		case WM_DESTROY:
			running = false;
			OutputDebugString(L"WM_DESTROY\n");
			break;

		case WM_CLOSE:
			running = false;
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
			UpdateWindow(devContext,left,top,width,height);
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
			running = true;
			while(running){

				
				while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)){

					if (message.message == WM_QUIT){
						running = false;
					}

					TranslateMessage(&message);
					DispatchMessage(&message);
				}

				static int XOffset = 0;
				Render(XOffset++, 0);
				RECT resizeRect;
				GetClientRect(winHandle, &resizeRect);
				int height = resizeRect.bottom - resizeRect.top;
				int width = resizeRect.right - resizeRect.left;
				HDC context = GetDC(winHandle);
				UpdateWindow(context, 0, 0, width, height);
				ReleaseDC(winHandle, context);
			}
		}
	}
	return 0;
}