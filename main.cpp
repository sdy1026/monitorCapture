
#include "monitorCapture.h"
#include "DXGICapture.h"
#include "CELLTimestamp.hpp"

int main()
{
	int i = 0;

	GDIMonitor gdi;
	

	DXGICapture dxgi;

	CELLTimestamp time;
	time.update();
	while (++i < 100)
	{
		gdi.StartCapture();
	}
	std::cout << "gdi stop capture use time is :" << time.getElapsedTimeInMilliSec() << std::endl;
	i = 0;

	time.update();
	while (++i < 10000)
	{
		dxgi.monitorCapture();
	}
	std::cout << "dxgi stop capture use time is :" << time.getElapsedTimeInMilliSec() << std::endl;

	system("pause");

	return 0;
}
