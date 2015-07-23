#include "DXApp.h"

class TestApp : public DXApp
{
public:
	TestApp(HINSTANCE hInstance);
	~TestApp();

	bool Init();
	void Update(float dt);
	void Render(float dt);
    void ProcessKey(UINT keyPress);

private:
	bool state[10];
	float rot = 0.0f;
	float rotSpeed = 0.0f;
};

TestApp::TestApp(HINSTANCE hInstance) : DXApp(hInstance)
{

}

TestApp::~TestApp()
{

}

bool TestApp::Init()
{
	if (!DXApp::Init())
		return false;
	
	LoadTexture(L"..\\..\\Resources\\Wood.DDS");
	for (int i = 0; i < sizeof(state); i++)
		state[i] = true;
	return true;
}

void TestApp::ProcessKey(UINT keyPress)
{
    if (keyPress == 0x00780000) //Scroll Up
    {
        SetCamera(m_Zoom += 0.1f);
    }
    else if (keyPress == 0xff880000) //Scroll Down
    {
        SetCamera(m_Zoom -= 0.1f);
    }
    else if (keyPress >= 0x30 && keyPress <= 0x39) // Numbers
    {
        keyPress -= 0x30;
        if (keyPress == 0x0)
        {
            for (int i = 0; i < sizeof(state) - 4; i++)
                state[i] = true;
        }
        else
        {
            state[keyPress-1] = !state[keyPress-1];
        }
    }
	else if (keyPress >= 0x25 && keyPress <= 0x28) //Arrow Keys
	{
		if (keyPress == 0x25) //Left
			;
		else if (keyPress == 0x26) //Up
			;
		else if (keyPress == 0x27) //Right
			;
		else if (keyPress == 0x28) //Down
			;
	}
	else if (keyPress == 0x21) // Page Up
		rotSpeed += 0.0001f;
	else if (keyPress == 0x22) // Page Down
		rotSpeed -= 0.0001f;
    return;
}

void TestApp::Update(float dt)
{
	rot += rotSpeed;
	if (rot > DirectX::XM_2PI)
		rot = 0.0f;

	m_mWorld = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMVECTOR rotAxis = { 0.0f, 1.0f, 0.0f, 0.0f };

	rotMatrix = DirectX::XMMatrixRotationAxis(rotAxis, rot);
	//translationMatrix = DirectX::XMMatrixTranslation(0.0f, 5.0f, 0.0f);

	m_mWorld = translationMatrix * rotMatrix;

}

void TestApp::Render(float dt)
{

	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::Blue);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	VERTEX Pyramid[] = {
		{ {  0.0f,  0.5f,  0.0f }, {0.5f, 0.0f}},  //0 Tip
		{ { -0.5f, -0.5f, -0.5f }, {1.0f, 1.0f}},  //1 Front Right
		{ { -0.5f, -0.5f,  0.5f }, {1.0f, 1.0f}}, //2 Back Right
		{ {  0.5f, -0.5f, -0.5f }, {1.0f, 0.0f}},  //3 Front Left
		{ {  0.5f, -0.5f,  0.5f }, {1.0f, 0.0f}},   //4 Back Left

		//Square
		{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f} },  //5 Top Left
		{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f} },  //6 Top right
		{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f} },  //7 Bottom right
		{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f} },  //8 bottom left
	};

	DWORD Indices[] = {
        2, 4, 0, // Back
        1, 4, 2, // Bottom
        1, 3, 4, // Bottom
        0, 1, 2, // Left Side
		0, 4, 3, // Right Side
        3, 1, 0, // Front

		//5, 7, 8,
		//5, 6, 7


	};
	
	// IA

	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	MapBuffer(m_pVertexBuffer, reinterpret_cast<BYTE*>(Pyramid), sizeof(Pyramid));

	MapBuffer(m_pIndexBuffer, reinterpret_cast<BYTE*>(Indices), sizeof(Indices));

	// Vertex Shader
    
    m_mWVP = m_mWorld * m_mCamView * m_mCamProjection;
    m_cbPerObj.WVP = DirectX::XMMatrixTranspose(m_mWVP);
    m_pImmediateContext->UpdateSubresource(m_pPerObjectConstantBuffer, 0, NULL, &m_cbPerObj, 0, 0);
    m_pImmediateContext->VSSetConstantBuffers(0,1, &m_pPerObjectConstantBuffer);

	// Pixel Shader
	
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTextureResourceView);
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pTextureSamplerState);

	// Rasterizer

	//m_pImmediateContext->RSSetState(m_pWireFrameRasterizer);

	// Draw

    for (int i = 0; i < sizeof(state); i++)
    {
		if (state[i])
		{
			m_pImmediateContext->DrawIndexed(3, 3 * i, 0);
		}
    }
        
	m_pSwapChain->Present(0, 0);
}


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	TestApp tApp(hInstance);

	if (!tApp.Init())
		return 1;
	return tApp.Run();
}