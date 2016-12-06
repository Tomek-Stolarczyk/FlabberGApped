#include "DXApp.h"
#include <Common.h>
#include <Models.h>

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

    Pyramid myNewDragon;

	DirectX::XMMATRIX mWorld1;
	DirectX::XMMATRIX mWorld2;
	bool state[10];
	float rot = 0.0f;
	float rotSpeed = 0.0f;

	//DX Resources
	int r_CCWCullMode;
	int r_CWCullMode;

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

	
	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	r_CCWCullMode = DXApp::CreateRasterizerState(&cmdesc);

	if (r_CCWCullMode < 0)
		return false;

	cmdesc.FrontCounterClockwise = false;
	r_CWCullMode = DXApp::CreateRasterizerState(&cmdesc);

	if (r_CWCullMode < 0)
		return false;


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

	DirectX::XMMATRIX translationMatrix;
	DirectX::XMMATRIX rotMatrix;
	DirectX::XMVECTOR rotAxis;

	mWorld1 = DirectX::XMMatrixIdentity();
	translationMatrix = DirectX::XMMatrixIdentity();
	rotMatrix = DirectX::XMMatrixIdentity();
	rotAxis = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f ));
	rotMatrix = DirectX::XMMatrixRotationAxis(rotAxis, rot);
	translationMatrix = DirectX::XMMatrixTranslation(-2.0f, 0.0f, 0.0f);
	mWorld1 = translationMatrix * rotMatrix;

	mWorld2 = DirectX::XMMatrixIdentity();
	translationMatrix = DirectX::XMMatrixIdentity();
	rotMatrix = DirectX::XMMatrixIdentity();
	rotAxis = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f));
	rotMatrix = DirectX::XMMatrixRotationAxis(rotAxis, rot);
	translationMatrix = DirectX::XMMatrixTranslation(2.0f, 0.0f, 0.0f);
	mWorld2 = translationMatrix * rotMatrix;


	//DirectX::XMVECTOR cubePos = DirectX::XMVectorZero();

	//cubePos = DirectX::XMVector3TransformCoord(cubePos, cube1World);


}

void TestApp::Render(float dt)
{

	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::Blue);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	float blendFactor[] = { 0.75f, 0.75f, 0.75f, 1.0f };

	m_pImmediateContext->OMSetBlendState(m_pTransparencyBlend, blendFactor, 0xffffffff);

    Pyramid myNewPyramid;

	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UpdateBufferByMap(m_pVertexBuffer, reinterpret_cast<BYTE*>(myNewPyramid.GetVertices()), myNewPyramid.SizeOfVertexBuffer());

	UpdateBufferByMap(m_pIndexBuffer, reinterpret_cast<BYTE*>(myNewPyramid.GetIndecies()), myNewPyramid.SizeOfIndexBuffer());

	// Vertex Shader
    
    m_mWVP = mWorld1 * m_mCamView * m_mCamProjection;
    m_cbPerObj.WVP = DirectX::XMMatrixTranspose(m_mWVP);
    m_pImmediateContext->UpdateSubresource(m_pPerObjectConstantBuffer, 0, NULL, &m_cbPerObj, 0, 0);
    m_pImmediateContext->VSSetConstantBuffers(0,1, &m_pPerObjectConstantBuffer);

	// Pixel Shader
	
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTextureResourceView);
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pTextureSamplerState);

	// Draw

	m_pImmediateContext->DrawIndexed(6*3, 0, 0);
    
	//m_mWVP = mWorld2 * m_mCamView * m_mCamProjection;
	//m_cbPerObj.WVP = DirectX::XMMatrixTranspose(m_mWVP);
	//m_pImmediateContext->UpdateSubresource(m_pPerObjectConstantBuffer, 0, NULL, &m_cbPerObj, 0, 0);
	//m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pPerObjectConstantBuffer);

	//m_pImmediateContext->DrawIndexed(6*3, 0, 0);

	m_pSwapChain->Present(0, 0);
}


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	TestApp tApp(hInstance);

	if (!tApp.Init())
		return 1;
	return tApp.Run();
}