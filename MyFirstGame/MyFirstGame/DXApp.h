#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "DXUtil.h"

#define ARRAYLEN(arr) sizeof(arr)/sizeof(arr[0])

typedef struct 
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
} VERTEX, *pVertex;

struct cbPerObject
{
    DirectX::XMMATRIX  WVP;
};

class DXApp
{
public:
	DXApp(HINSTANCE hInstance);
	virtual ~DXApp();


	int Run();

	virtual bool Init();
	virtual void Update(float dt) = 0;
	virtual void Render(float dt) = 0;
    virtual void ProcessKey(UINT keyPress) = 0;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void SetCamera(float zoom);
	bool LoadTexture(const wchar_t *TexPath);
	bool MapBuffer(ID3D11Buffer* buffer, BYTE* data, size_t maxBytes);
	int CreateRasterizerState(D3D11_RASTERIZER_DESC*);

protected:

	//Win32 attributes
	HWND			m_hAppWnd;
	HINSTANCE		m_hAppInstance;
	UINT			m_ClientWidth;
	UINT			m_ClientHeight;
	std::string		m_AppTitle;
	DWORD			m_WndStyle;

	//DX attributes

	ID3D11Device*				m_pDevice;
	ID3D11DeviceContext*		m_pImmediateContext;
	IDXGISwapChain*				m_pSwapChain;
	ID3D11RenderTargetView*		m_pRenderTargetView;
	D3D_DRIVER_TYPE				m_DriverType;
	D3D_FEATURE_LEVEL			m_FeatureLevel;
	D3D11_VIEWPORT				m_ViewPort;
	ID3D11VertexShader*			m_pVS;
	ID3D11PixelShader*			m_pPS;
	ID3D11DepthStencilView*		m_pDepthStencilView;

	// DX Camera

	DirectX::XMMATRIX			m_mWVP;
	DirectX::XMMATRIX			m_mWorld;
	DirectX::XMMATRIX			m_mCamView;
	DirectX::XMMATRIX			m_mCamProjection;

	DirectX::XMVECTOR			m_vCamPosition;
	DirectX::XMVECTOR			m_vCamTarget;
	DirectX::XMVECTOR			m_vCamUp;

	float						m_Zoom = -2.0f;

	// DX resources

	ID3D11Buffer*               m_pVertexBuffer;
	ID3D11Buffer*				m_pIndexBuffer;
	ID3D11Buffer*				m_pPerObjectConstantBuffer;
	ID3D11Texture2D*			m_pDepthStencilBuffer;
	ID3D11InputLayout*			m_pLayout;

	ID3D11ShaderResourceView*	m_pTextureResourceView;
	ID3D11Resource*				m_pTextureResource;
	ID3D11SamplerState*			m_pTextureSamplerState;

	ID3D11BlendState*			m_pTransparencyBlend;

    // Raw DX Resources

    cbPerObject                 m_cbPerObj;
	ID3D11RasterizerState*		m_pRasterizerStates[10];

protected:

	//Init Win32 window
	bool InitWindow();

	//Init D3D11

	bool InitDirect3D();

private:

	bool BuildPipeline();
	bool CompilePS();
	bool CompileVS();
	bool BuildVertexBuffer(int);
	bool BuildIndexBuffer(int);
	bool BuildDepthStencilView();
    bool CreateConstantBuffer();
	bool CreateBlendState();
};

