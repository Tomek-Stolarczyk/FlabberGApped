#include "DXApp.h"

class TestApp : public DXApp
{
public:
	TestApp(HINSTANCE hInstance);
	~TestApp();

	bool Init();
	void Update(float dt);
	void Render(float dt);
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

void TestApp::Update(float dt)
{

}

void TestApp::Render(float dt)
{

}


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	TestApp tApp(hInstance);

	if (!tApp.Init())
		return 1;
	return tApp.Run();
}