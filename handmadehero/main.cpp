//main.cpp
//entry point file
//last update - 10/12/2014
#include <Windows.h>

static bool running;

struct offScreenBuffer{
	
	BITMAPINFO bmInfo;
	void* bitmapMem;
	int width;
	int height;
	int bytesPerPixel;
};

static offScreenBuffer buff;


void Render(offScreenBuffer* buff,unsigned int xOffset,unsigned int yOffset){
	int Pitch = buff->width * buff->bytesPerPixel;
	unsigned int* pixel = 0;
	unsigned char* row = (unsigned char*)buff->bitmapMem;
	int height = buff->height;
	int width = buff->width;
	for (unsigned int Y = 0; Y < height; ++Y){
		pixel = (unsigned int*)row;
		for (unsigned int X = 0; X < width; ++X){
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

void ResizeDIBSection(offScreenBuffer* buff,int width, int height){

	int videoMemoryRequired = 4 *width * height;

	if (buff->bitmapMem){
		VirtualFree(buff->bitmapMem, videoMemoryRequired, MEM_RELEASE);
	}

	buff->bytesPerPixel = 4;
	buff->width  = width;
	buff->height = height;
	
	buff->bmInfo.bmiHeader.biSize = sizeof(buff->bmInfo.bmiHeader);
	buff->bmInfo.bmiHeader.biWidth = buff->width;
	buff->bmInfo.bmiHeader.biHeight = -buff->height;
	buff->bmInfo.bmiHeader.biPlanes = 1;
	buff->bmInfo.bmiHeader.biBitCount = 32;
	buff->bmInfo.bmiHeader.biCompression = BI_RGB;
	buff->bmInfo.bmiHeader.biSizeImage = 0;
	buff->bmInfo.bmiHeader.biXPelsPerMeter = 0;
	buff->bmInfo.bmiHeader.biYPelsPerMeter = 0;
	buff->bmInfo.bmiHeader.biClrUsed = 0;
	buff->bmInfo.bmiHeader.biClrImportant = 0;

	
	buff->bitmapMem = VirtualAlloc(0, videoMemoryRequired, MEM_COMMIT, PAGE_READWRITE);

}

void UpdateWindow(offScreenBuffer* buff,HDC context, int x, int y, int width, int height){
	StretchDIBits(
		context,
		0, 0, width, height,
		0,0,buff->width,buff->height,
		buff->bitmapMem,
		&(buff->bmInfo),
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
			UpdateWindow(&buff,devContext,left,top,width,height);
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

	ResizeDIBSection(&buff, 1028, 768);

	windowClass.style		  = CS_HREDRAW | CS_VREDRAW;
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
				Render(&buff,XOffset++, 0);
				RECT resizeRect;
				GetClientRect(winHandle, &resizeRect);
				int height = resizeRect.bottom - resizeRect.top;
				int width = resizeRect.right - resizeRect.left;
				HDC context = GetDC(winHandle);
				UpdateWindow(&buff,context, 0, 0, width, height);
				ReleaseDC(winHandle, context);
			}
		}
	}
	return 0;
}