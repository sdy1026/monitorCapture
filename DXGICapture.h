#pragma once

#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>

class DXGICapture
{
public:
	DXGICapture();
	~DXGICapture();

	void monitorCapture();
private:

	void getDXGIDevice();

	void getDXGIAdapter();

	void getDXGIOutput();
	
	void getDXGIOutput1();

	void getOutput1Duplication();

	void getOutputDesc();

	void savepic();
	void savebmp(void* pdata, char* bmp_file, int width, int height);
private:
	ID3D11Device* p_d3dDevice = NULL;
	IDXGIDevice* p_dxgiDevice = NULL;
	ID3D11DeviceContext* p_d3dDeviceContext = NULL;
	IDXGIAdapter* p_dxgiAdapter = NULL;
	IDXGIOutput* p_dxgiOutput = NULL;
	IDXGIOutput1* p_dxgiOutput1 = NULL;
	IDXGIOutputDuplication* p_dxgiOutputDup = NULL;
	ID3D11Texture2D* desktopImg = nullptr;
};