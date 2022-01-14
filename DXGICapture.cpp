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

//��ȡdxgi�豸 ͨ������d3d�豸�����ȡ
void DXGICapture::getDXGIDevice()
{
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, NULL, D3D11_SDK_VERSION, &p_d3dDevice, NULL, &p_d3dDeviceContext);
	p_d3dDevice->QueryInterface(IID_PPV_ARGS(&p_dxgiDevice));
}

//��ȡdxgi adapter
void DXGICapture::getDXGIAdapter()
{
	p_dxgiDevice->GetParent(IID_PPV_ARGS(&p_dxgiAdapter));
}

//��ȡdxgi output
void DXGICapture::getDXGIOutput()
{
	p_dxgiAdapter->EnumOutputs(0, &p_dxgiOutput);
}

//��ȡdxgi output1 dxgi1.2֮���Ƴ����½ӿڣ����а�����desktopduplication �ӿ�
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
	// ��д���ֽ���
	DWORD bytesWritten = 0;
	// λͼ��С
	int bmpSize = width * height * 4;

	// �ļ�ͷ
	BITMAPFILEHEADER bmpHeader;
	// �ļ��ܴ�С = �ļ�ͷ + λͼ��Ϣͷ + λͼ����
	bmpHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpSize;
	// �̶�
	bmpHeader.bfType = 0x4D42;
	// ����ƫ�ƣ���λͼ��������λ��
	bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	// ����Ϊ0
	bmpHeader.bfReserved1 = 0;
	// ����Ϊ0
	bmpHeader.bfReserved2 = 0;
	// д�ļ�ͷ
	WriteFile(hFile, (LPSTR)&bmpHeader, sizeof(bmpHeader), &bytesWritten, NULL);

	// λͼ��Ϣͷ
	BITMAPINFOHEADER bmiHeader;
	// λͼ��Ϣͷ��С
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	// λͼ���ؿ��
	bmiHeader.biWidth = width;
	// λͼ���ظ߶�
	bmiHeader.biHeight = -height;
	// ����Ϊ1
	bmiHeader.biPlanes = 1;
	// ������ռλ��
	bmiHeader.biBitCount = 32;
	// 0��ʾ��ѹ��
	bmiHeader.biCompression = 0;
	// λͼ���ݴ�С
	bmiHeader.biSizeImage = bmpSize;
	// ˮƽ�ֱ���(����/��)
	bmiHeader.biXPelsPerMeter = 0;
	// ��ֱ�ֱ���(����/��)
	bmiHeader.biYPelsPerMeter = 0;
	// ʹ�õ���ɫ��0Ϊʹ��ȫ����ɫ
	bmiHeader.biClrUsed = 0;
	// ��Ҫ����ɫ����0Ϊ������ɫ����Ҫ
	bmiHeader.biClrImportant = 0;

	// дλͼ��Ϣͷ
	WriteFile(hFile, (LPSTR)&bmiHeader, sizeof(bmiHeader), &bytesWritten, NULL);
	// дλͼ����
	WriteFile(hFile, pdata, bmpSize, &bytesWritten, NULL);
	CloseHandle(hFile);
}