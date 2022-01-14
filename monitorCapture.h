// monitorCapture.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>
#include <thread>
#ifdef WIN32
#include <windows.h>
#endif

// TODO: 在此处引用程序需要的其他标头。

class WindowCaptureWinData;

class GDIMonitor
{
public:
	GDIMonitor();
	~GDIMonitor();

	void StartCapture();

private:
	void releaseObject();
	void resetDC();
	void captureMonitor();
	void savebmp(BYTE* pdata, char* bmp_file, int width, int height);
private:
	std::unique_ptr<WindowCaptureWinData> m_data;
};