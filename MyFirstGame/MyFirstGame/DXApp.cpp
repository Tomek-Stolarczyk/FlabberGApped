#include "DXApp.h"
#include <assert.h>

namespace
{
	DXApp* g_pApp = nullptr;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (g_pApp)
		return g_pApp->MsgProc(hwnd, msg, wParam, lParam);
	else
		return DefWindowProc(hwnd, msg, wParam, lParam);
}

DXApp::DXApp(HINSTANCE hInstance)
{
	m_hAppInstance = hInstance;
	m_hAppWnd = NULL;
	m_ClientHeight = 600;
	m_ClientWidth = 800;
	m_AppTitle = "My First Game";
	m_WndStyle = WS_OVERLAPPEDWINDOW;

	g_pApp = this;

	m_pDevice = nullptr;
	m_pImmediateContext = nullptr;
	m_pSwapChain = nullptr;
	m_pRenderTargetView = nullptr;
}


DXApp::~DXApp()
{
	if (m_pImmediateContext)
		m_pImmediateContext->ClearState();

	Memory::SafeRelease(m_pRenderTargetView);
	Memory::SafeRelease(m_pSwapChain);
	Memory::SafeRelease(m_pImmediateContext);
	Memory::SafeRelease(m_pDevice);

}

int DXApp::Run()
{
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update(0.0f);
			Render(0.0f);
		}
	}
	return static_cast<int>(msg.wParam);

}

bool DXApp::Init()
{
	if (!InitWindow())
		return false;
	if (!InitDirect3D())
		return false;
	return true;
}

bool DXApp::InitWindow()
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof WNDCLASSEX);
	wcex.cbClsExtra = 0;
	wcex.cbSize = sizeof WNDCLASSEX;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hInstance = m_hAppInstance;
	wcex.lpfnWndProc = MainWndProc;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszClassName = "DXAPPWINDOWCLASS";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		OutputDebugString("Failed to create window class");
		return false;
	}

	RECT r = { 0, 0, m_ClientWidth, m_ClientHeight };
	AdjustWindowRect(&r, m_WndStyle, FALSE);
	UINT width = r.right - r.left;
	UINT height = r.bottom - r.top;

	UINT x = GetSystemMetrics(SM_CXSCREEN)/2 - width/2;
	UINT y = GetSystemMetrics(SM_CYSCREEN)/2 - height/2;

	m_hAppWnd = CreateWindow("DXAPPWINDOWCLASS", m_AppTitle.c_str(),
							m_WndStyle, x, y, width, height, NULL, NULL, 
							m_hAppInstance, NULL);

	if (!m_hAppWnd)
	{
		OutputDebugString("Failed to create window");
		return false;
	}

	ShowWindow(m_hAppWnd, SW_SHOW);

	return true;

}

bool DXApp::InitDirect3D()
{
	//Create device

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //_DEBUG

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] = 
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof DXGI_SWAP_CHAIN_DESC);
	swapDesc.BufferCount = 1; // back buffer + front buffer
	swapDesc.BufferDesc.Height = m_ClientHeight;
	swapDesc.BufferDesc.Width = m_ClientWidth;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hAppWnd;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Windowed = true;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr;
	for (unsigned int i = 0; i < numDriverTypes; i++)
	{
		hr = D3D11CreateDeviceAndSwapChain(NULL,driverTypes[i],NULL,
						createDeviceFlags,featureLevels,numFeatureLevels,
						D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice,
						&m_FeatureLevel, &m_pImmediateContext);
		if (SUCCEEDED(hr))
		{
			m_DriverType = driverTypes[i];
			break;
		}
	}

	if (FAILED(hr))
	{
		OutputDebugString("Failed to create Device");
		return false;
	}

	// Create Render Target View

	ID3D11Texture2D* m_pBackBufferTex;
	m_pSwapChain->GetBuffer(NULL, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pBackBufferTex));
	m_pDevice->CreateRenderTargetView(m_pBackBufferTex, nullptr, &m_pRenderTargetView);

	// Bind Render Target View
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);

	// Viewport Creation
	m_ViewPort.Width = static_cast<float>(m_ClientWidth);
	m_ViewPort.Height = static_cast<float>(m_ClientHeight);
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.MinDepth = 0.0f;

	//Bind Viewport
	m_pImmediateContext->RSSetViewports(1, &m_ViewPort);
	return true;
}


LRESULT DXApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}



