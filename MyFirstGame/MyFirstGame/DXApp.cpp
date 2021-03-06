#include "DXApp.h"
#include <ErrorMessage.h>
#include <Models.h>

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

DXApp::DXApp(HINSTANCE hInstance):
    m_hAppInstance(hInstance),
    m_hAppWnd(nullptr),
    m_ClientHeight(600),
    m_ClientWidth(800),
    m_AppTitle("My First Game"),
    m_WndStyle(WS_OVERLAPPEDWINDOW),

    m_pDevice(nullptr),
    m_pImmediateContext(nullptr),
    m_pSwapChain(nullptr),
    m_pRenderTargetView(nullptr)
{
	g_pApp = this;

}

DXApp::~DXApp()
{
	if (m_pImmediateContext)
		m_pImmediateContext->ClearState();

	Memory::SafeRelease(m_pPerObjectConstantBuffer);

	Memory::SafeRelease(m_pDepthStencilView);
	Memory::SafeRelease(m_pDepthStencilBuffer);

	for (int i = 0; i < ARRAYLEN(m_pRasterizerStates); i++)
		Memory::SafeRelease(m_pRasterizerStates[i]);

	Memory::SafeRelease(m_pTransparencyBlend);

	Memory::SafeRelease(m_pRenderTargetView);
	m_pSwapChain->SetFullscreenState(FALSE, NULL);

	Memory::SafeRelease(m_pIndexBuffer);
	Memory::SafeRelease(m_pVertexBuffer);

	Memory::SafeRelease(m_pLayout);
	Memory::SafeRelease(m_pVS);
	Memory::SafeRelease(m_pPS);

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

	return (0 == ShowWindow(m_hAppWnd, SW_SHOW));
}

bool DXApp::BuildPipeline()
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
		hr = D3D11CreateDeviceAndSwapChain(NULL, driverTypes[i], NULL,
			createDeviceFlags, featureLevels, numFeatureLevels,
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
        globalErrorMessage.ThrowError("Failed to create Device");
		return false;
	}

	// Create Render Target View

	ID3D11Texture2D* m_pBackBufferTex;
	hr = m_pSwapChain->GetBuffer(NULL, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pBackBufferTex));
	
	if (FAILED(hr))
	{
		OutputDebugString("Failed to fetch back buffer");
		return false;
	}

	hr = m_pDevice->CreateRenderTargetView(m_pBackBufferTex, nullptr, &m_pRenderTargetView);

	if (FAILED(hr))
	{
		OutputDebugString("Failed to create render target");
		return false;
	}

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

bool DXApp::CompileVS()
{
	//Compile Shaders
	ID3DBlob *VS, *Err;
	HRESULT hr;

	hr = D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "main", "vs_4_0", D3DCOMPILE_DEBUG, D3DCOMPILE_PREFER_FLOW_CONTROL, &VS, &Err);
	if FAILED(hr)
	{
		if (Err)
			OutputDebugString(static_cast<char *>(Err->GetBufferPointer()));
		Memory::SafeRelease(Err);
		Memory::SafeRelease(VS);
		return false;
	}

	//Bind Shaders to frame
	m_pDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVS);

	m_pImmediateContext->VSSetShader(m_pVS, NULL, NULL);

	//Configure the vertex descriptions
	D3D11_INPUT_ELEMENT_DESC elementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = m_pDevice->CreateInputLayout(elementDesc, sizeof(elementDesc) / sizeof(elementDesc[0]), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pLayout);

	m_pImmediateContext->IASetInputLayout(m_pLayout);

	return SUCCEEDED(hr);
}

bool DXApp::CompilePS()
{
	//Compile Shaders
	ID3DBlob *PS, *Err;
	HRESULT hr;
	hr = D3DCompileFromFile(L"PixelShader.hlsl", NULL, NULL, "main", "ps_4_0", D3DCOMPILE_DEBUG, D3DCOMPILE_PREFER_FLOW_CONTROL | D3DCOMPILE_SKIP_OPTIMIZATION, &PS, &Err);
	if FAILED(hr)
	{
		if (Err)
			OutputDebugString(static_cast<char *>(Err->GetBufferPointer()));
		Memory::SafeRelease(Err);
		Memory::SafeRelease(PS);
		return false;
	}

	//Bind Shaders to frame
	hr = m_pDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPS);

	m_pImmediateContext->PSSetShader(m_pPS, NULL, NULL);

	return SUCCEEDED(hr);
}

bool DXApp::BuildVertexBuffer(int numOfVertices)
{
	HRESULT hr;

	//Initiate Vertex Buffer
	D3D11_BUFFER_DESC bufDesc;
	ZeroMemory(&bufDesc, sizeof(bufDesc));

	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.ByteWidth = sizeof(VERTEX)* numOfVertices;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = m_pDevice->CreateBuffer(&bufDesc, NULL, &m_pVertexBuffer);

	// Bind Vertex buffer
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;

	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	return SUCCEEDED(hr);
}

bool DXApp::BuildIndexBuffer(int numOfIndices)
{
	HRESULT hr;

	D3D11_BUFFER_DESC bufDesc;
	ZeroMemory(&bufDesc, sizeof(bufDesc));

	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.ByteWidth = sizeof(DWORD) * numOfIndices;
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = m_pDevice->CreateBuffer(&bufDesc, NULL, &m_pIndexBuffer);
	m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	return SUCCEEDED(hr);
}

bool DXApp::BuildDepthStencilView()
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));

	texDesc.Width = m_ClientWidth;
	texDesc.Height = m_ClientHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	if FAILED(m_pDevice->CreateTexture2D(&texDesc, NULL, &m_pDepthStencilBuffer))
	{
		return false;
	}
	if FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, NULL, &m_pDepthStencilView))
	{
		return false;
	}

	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	return true;
}

bool DXApp::CreateConstantBuffer()
{
    D3D11_BUFFER_DESC bufDesc;
    ZeroMemory(&bufDesc, sizeof(bufDesc));

    bufDesc.Usage = D3D11_USAGE_DEFAULT;
    bufDesc.ByteWidth = sizeof(cbPerObject);
    bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufDesc.CPUAccessFlags = 0;
    bufDesc.MiscFlags = 0;

    return SUCCEEDED(m_pDevice->CreateBuffer(&bufDesc, NULL, &m_pPerObjectConstantBuffer));
}

int DXApp::CreateRasterizerState(D3D11_RASTERIZER_DESC* RasDesc)
{
	ID3D11RasterizerState* newRasterizer;
	HRESULT hr;
	hr = m_pDevice->CreateRasterizerState(RasDesc, &newRasterizer);
	if (FAILED(hr))
		return hr;
	int i;
	for (i = 1; i < ARRAYLEN(m_pRasterizerStates); i++)
	{
		if (m_pRasterizerStates[i] == NULL)
		{
			m_pRasterizerStates[i] = newRasterizer;
			break;
		}
	}

	if (i == ARRAYLEN(m_pRasterizerStates))
		return -1;

	return i;
}

bool DXApp::LoadTexture(const wchar_t *TexPath)
{
	HRESULT hr = S_OK;
	//hr = DirectX::CreateDDSTextureFromFile(m_pDevice, TexPath, &m_pTextureResource, &m_pTextureResourceView);
	if (FAILED(hr))
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			OutputDebugString("Error: Texture file not found");
			return false;
		}
		else
		{
			OutputDebugString("Unknown Error loading Texture File");
		}
	}

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_pDevice->CreateSamplerState(&sampDesc, &m_pTextureSamplerState);

	return SUCCEEDED(hr);
}

void DXApp::SetCamera(float zoom)
{
    m_vCamPosition = DirectX::XMVectorSet(0.0f, 0.0f, zoom, 0.0f);
    m_vCamTarget = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    m_vCamUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    m_mCamView = DirectX::XMMatrixLookAtLH(m_vCamPosition, m_vCamTarget, m_vCamUp);
    float Aspect = (float)m_ClientWidth / m_ClientHeight;
    m_mCamProjection = DirectX::XMMatrixPerspectiveFovLH( DirectX::XMConvertToRadians(70.0f), Aspect, 0.3f, 100.0f);

}

bool DXApp::UpdateBufferByMap(ID3D11Buffer* buffer, BYTE* data, size_t maxBytes)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapSub;
	ZeroMemory(&mapSub, sizeof(mapSub));
	hr = m_pImmediateContext->Map(buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapSub);
	memcpy(mapSub.pData, data, maxBytes);
	m_pImmediateContext->Unmap(buffer, NULL);
	return SUCCEEDED(hr);
}

bool DXApp::InitDirect3D()
{
	if (!BuildPipeline()){
		OutputDebugString("Failed to initialize device/rendertarget");
		return false;
	}

	if (!CompileVS()){
		OutputDebugString("Failed to Compile VertexShader");
		return false;
	}

	if (!CompilePS()){
		OutputDebugString("Failed to Compile PixelShader");
		return false;
	}

	if (!BuildVertexBuffer(9)){
		OutputDebugString("Failed to build and bind Vertex Buffer");
		return false;
	}

	if (!BuildIndexBuffer(30)){
		OutputDebugString("Failed to build and bind Index Buffer");
		return false;
	}

	if (!BuildDepthStencilView()){
		OutputDebugString("Failed to build depth stencil view");
		return false;
	}

    if (!CreateConstantBuffer()){
        OutputDebugString("Failed to create constant buffer");
        return false;
    }

	if (!CreateBlendState()){
		OutputDebugString("Failed to create Blend State for Transparency");
		return false;
	}

    SetCamera(m_Zoom);

	return true;
}

LRESULT DXApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
    case WM_KEYDOWN:
    case WM_MOUSEWHEEL:
        ProcessKey(wParam);
        return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

bool DXApp::CreateBlendState()
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	return SUCCEEDED(m_pDevice->CreateBlendState(&blendDesc, &m_pTransparencyBlend));
}





