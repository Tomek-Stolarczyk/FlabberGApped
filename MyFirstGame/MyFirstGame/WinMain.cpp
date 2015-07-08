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
    float zoom = -0.8f;
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
	return true;
}

void TestApp::ProcessKey(UINT keyPress)
{
    if (keyPress == 0x00780000) //Scroll Up
    {
        SetCamera(zoom += 0.1f);
    }
    else if (keyPress == 0xff880000) //Scroll Down
    {
        SetCamera(zoom -= 0.1f);
    }
    else if (keyPress >= 0x30 && keyPress <= 0x39) // Numbers
    {
        keyPress -= 0x30;
        if (keyPress == 0x0)
        {
            for (int i = 0; i < sizeof(state); i++)
                state[i] = 0;
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


    return;
}

void TestApp::Update(float dt)
{

}

void TestApp::Render(float dt)
{
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::Blue);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	VERTEX Pyramid[] = {
		{ { 0.0f, 0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } },  //0 Tip
		{ {-0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f, 0.0f } },  //1 Back Left
		{ {-0.5f, -0.5f, 0.5f }, { 0.5f, 0.5f, 0.0f, 0.0f } }, //2 Front Left
		{ {0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f, 0.0f } },  //3 Back Right
		{ {0.5f, -0.5f, 0.5f }, { 0.0f, 0.5f, 0.5f, 0.0f } },   //4 Front Right

		//Square
		{ { -0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } },  //5 Top Left
		{ { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } },  //6 Top right
		{ { 0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } },  //7 Bottom right
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } },  //8 bottom left
	};

	DWORD Indices[] = {
        0, 4, 2, // Back
        2, 4, 1, // Bottom
        4, 3, 1, // Bottom
        2, 1, 0, // Left Side
		3, 4, 0, // Right Side
        3, 1, 0, // Front


		//Square
		5, 6, 7,  
		5, 7, 8,
	};

	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D11_MAPPED_SUBRESOURCE mapSub;

	ZeroMemory(&mapSub, sizeof(mapSub));
	m_pImmediateContext->Map(m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapSub);
	memcpy(mapSub.pData, Pyramid, sizeof(Pyramid));
	m_pImmediateContext->Unmap(m_pVertexBuffer, NULL);

	ZeroMemory(&mapSub, sizeof(mapSub));
	m_pImmediateContext->Map(m_pIndexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapSub);
	memcpy(mapSub.pData, Indices, sizeof(Indices));
	m_pImmediateContext->Unmap(m_pIndexBuffer, NULL);

    m_mWorld = DirectX::XMMatrixIdentity();
    m_mWVP = m_mWorld * m_mCamView * m_mCamProjection;
    m_cbPerObj.WVP = DirectX::XMMatrixTranspose(m_mWVP);
    m_pImmediateContext->UpdateSubresource(m_pPerObjectConstantBuffer, 0, NULL, &m_cbPerObj, 0, 0);
    m_pImmediateContext->VSSetConstantBuffers(0,1, &m_pPerObjectConstantBuffer);


    for (int i = 0; i < sizeof(state); i++)
    {
        if (state[i])
            m_pImmediateContext->DrawIndexed(3, 3 * i, 0);
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