// monitorCapture.cpp: 定义应用程序的入口点。
//

#include "monitorCapture.h"

class WindowCaptureWinData
{
public:
	HWND hwnd;

	HDC hdc;
	HBITMAP bmp, old_bmp;
	BYTE* bits;
#if 0
	CURSORINFO ci;
	bool cursor_captured = false;
#endif

	int32_t width = {};
	int32_t height = {};

};


GDIMonitor::GDIMonitor()
{
	m_data = std::make_unique<WindowCaptureWinData>();
	m_data->hwnd = GetDesktopWindow();
	resetDC();
}

GDIMonitor::~GDIMonitor()
{
	releaseObject();
}

void GDIMonitor::StartCapture()
{
	
	captureMonitor();
}

void GDIMonitor::releaseObject()
{
	if (m_data->hdc)
	{
		//恢复兼容 设备上下文 环境
		SelectObject(m_data->hdc, m_data->old_bmp);
		//删除兼容dc
		DeleteDC(m_data->hdc);
		//删除位图数据
		DeleteObject(m_data->bmp);
	}
}

void GDIMonitor::resetDC()
{
	releaseObject();

	HDC screenDC = GetDC(GetDesktopWindow());

	

	//bitmap info 位图信息
	BITMAPINFO bi = { 0 };
	BITMAPINFOHEADER* bih = &bi.bmiHeader;
	bih->biSize = sizeof(BITMAPINFOHEADER);
	bih->biBitCount = 32;
	bih->biWidth = GetDeviceCaps(screenDC, DESKTOPHORZRES);
	bih->biHeight = GetDeviceCaps(screenDC, DESKTOPVERTRES);
	bih->biPlanes = 1;
	ReleaseDC(nullptr, screenDC);
	m_data->width = bih->biWidth;
	m_data->height = bih->biHeight;

	//创建兼容dcdc dc 设备上下文 该上下文允许在windows中进行与设备无关的绘制。
	m_data->hdc = CreateCompatibleDC(NULL);
	//创建DIB位图
	m_data->bmp = CreateDIBSection(m_data->hdc, &bi,
		DIB_RGB_COLORS, (void**)&m_data->bits,
		NULL, 0);

	//selectobject 将obj对象指定到设备的上下文，新的对象替换原有对象 返回的将是被替换的对象
	m_data->old_bmp = (HBITMAP)SelectObject(m_data->hdc, m_data->bmp);
}

void GDIMonitor::captureMonitor()
{
	HDC hdc_target;

#if 0
	memset(&m_data->ci, 0, sizeof(CURSORINFO));
	m_data->ci.cbSize = sizeof(CURSORINFO);
	m_data->cursor_captured = GetCursorInfo(&m_data->ci);
#endif

	hdc_target = GetDC(m_data->hwnd);

	//目标设备dc 目标区域左上角x坐标， 目标区域左上角y坐标 ，源区域宽度，源区域高度，源设备环境句柄，原区域左上角x坐标，源区域左上角y坐标
	//指定操作方式 srccopy 直接拷贝。
	BOOL bRet = BitBlt(m_data->hdc, 0, 0, m_data->width, m_data->height,
		hdc_target, 0, 0, SRCCOPY);

	ReleaseDC(NULL, hdc_target);

#if 1
	savebmp(m_data->bits, "D:\\test.bmp", m_data->width, m_data->height);
#endif
}


void GDIMonitor::savebmp(BYTE* pdata, char* bmp_file, int width, int height)
{
	HANDLE hFile = CreateFileA(bmp_file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL)
	{
		return;
	}
	// 已写入字节数
	DWORD bytesWritten = 0;
	// 位图大小
	int bmpSize = width * height * 4;

	// 文件头
	BITMAPFILEHEADER bmpHeader;
	// 文件总大小 = 文件头 + 位图信息头 + 位图数据
	bmpHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpSize;
	// 固定
	bmpHeader.bfType = 0x4D42; //字符bm的十六进制编码
	// 数据偏移，即位图数据所在位置
	bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	// 保留为0
	bmpHeader.bfReserved1 = 0;
	// 保留为0
	bmpHeader.bfReserved2 = 0;
	// 写文件头
	WriteFile(hFile, (LPSTR)&bmpHeader, sizeof(bmpHeader), &bytesWritten, NULL);

	// 位图信息头
	BITMAPINFOHEADER bmiHeader;
	// 位图信息头大小
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	// 位图像素宽度
	bmiHeader.biWidth = width;
	// 位图像素高度
	bmiHeader.biHeight = height;
	// 必须为1
	bmiHeader.biPlanes = 1;
	// 像素所占位数
	bmiHeader.biBitCount = 32; //rgba 8+8+8+8
	// 0表示不压缩
	bmiHeader.biCompression = 0;
	// 位图数据大小
	bmiHeader.biSizeImage = bmpSize;
	// 水平分辨率(像素/米)
	bmiHeader.biXPelsPerMeter = 0;
	// 垂直分辨率(像素/米)
	bmiHeader.biYPelsPerMeter = 0;
	// 使用的颜色，0为使用全部颜色
	bmiHeader.biClrUsed = 0;
	// 重要的颜色数，0为所有颜色都重要
	bmiHeader.biClrImportant = 0;

	// 写位图信息头
	WriteFile(hFile, (LPSTR)&bmiHeader, sizeof(bmiHeader), &bytesWritten, NULL);
	// 写位图数据
	WriteFile(hFile, pdata, bmpSize, &bytesWritten, NULL);
	CloseHandle(hFile);
}