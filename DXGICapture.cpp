#include "DXGICapture.h"
#include <iostream>

DXGICapture::DXGICapture()
{
	getDXGIDevice();
	getDXGIAdapter();
	getDXGIOutput();
	getDXGIOutput1();
	getOutput1Duplication();
}

DXGICapture::~DXGICapture()
{

}

void DXGICapture::monitorCapture()
{

	savepic();
}

//获取dxgi设备 通过创建d3d设备对象获取
void DXGICapture::getDXGIDevice()
{
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, NULL, D3D11_SDK_VERSION, &p_d3dDevice, NULL, &p_d3dDeviceContext);
	p_d3dDevice->QueryInterface(IID_PPV_ARGS(&p_dxgiDevice));
}

//获取dxgi adapter
void DXGICapture::getDXGIAdapter()
{
	p_dxgiDevice->GetParent(IID_PPV_ARGS(&p_dxgiAdapter));
}

//获取dxgi output
void DXGICapture::getDXGIOutput()
{
	p_dxgiAdapter->EnumOutputs(0, &p_dxgiOutput);
}

//获取dxgi output1 dxgi1.2之后推出的新接口，其中包括了desktopduplication 接口
void DXGICapture::getDXGIOutput1()
{
	p_dxgiOutput->QueryInterface(IID_PPV_ARGS(&p_dxgiOutput1));
}

void DXGICapture::getOutput1Duplication()
{
	p_dxgiOutput1->DuplicateOutput(p_d3dDevice, &p_dxgiOutputDup);
}

void DXGICapture::getOutputDesc()
{
	DXGI_OUTDUPL_DESC dxgiOutputDesc;
	p_dxgiOutputDup->GetDesc(&dxgiOutputDesc);
	std::cout << " " << dxgiOutputDesc.Rotation << std::endl;
}

void DXGICapture::savepic()
{
	IDXGIResource* desktopResource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO frame_info;
next:
	HRESULT ret =  p_dxgiOutputDup->AcquireNextFrame(0, &frame_info, &desktopResource);

	if (ret == DXGI_ERROR_ACCESS_LOST)
		goto next;
	if (ret == DXGI_ERROR_WAIT_TIMEOUT)
		goto next;
	else if (FAILED(ret))
		goto next;

	ret = desktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&desktopImg));
	
	desktopResource->Release();

	D3D11_TEXTURE2D_DESC dataDesc;
	
	desktopImg->GetDesc(&dataDesc);

	ID3D11Texture2D* copyDesktop = nullptr;
	dataDesc.Usage = D3D11_USAGE_STAGING;
	dataDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	dataDesc.BindFlags = 0;
	dataDesc.MiscFlags = 0;
	dataDesc.MipLevels = 1;
	dataDesc.ArraySize = 1;
	dataDesc.SampleDesc.Count = 1;

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	p_d3dDevice->CreateTexture2D(&dataDesc, NULL, &copyDesktop);

	p_d3dDeviceContext->CopyResource(copyDesktop, desktopImg);

	p_d3dDeviceContext->Map(copyDesktop, 0, D3D11_MAP_READ, 0, &mappedResource);

#if 1
	savebmp(mappedResource.pData, "D:\\dxgitest.bmp", dataDesc.Width, dataDesc.Height);
#endif
	copyDesktop->Release();
	p_dxgiOutputDup->ReleaseFrame();
}

void DXGICapture::savebmp(void* pdata, char* bmp_file, int width, int height)
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
	bmpHeader.bfType = 0x4D42;
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
	bmiHeader.biHeight = -height;
	// 必须为1
	bmiHeader.biPlanes = 1;
	// 像素所占位数
	bmiHeader.biBitCount = 32;
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