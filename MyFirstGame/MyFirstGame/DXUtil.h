#pragma once

#include <d3d11.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <DDSTextureLoader.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3DCompiler.lib")

#ifdef _DEBUG
	#pragma comment (lib, "DirectXTKD.lib")
#else
	#pragma comment (lib, "DirectXTK.lib")
#endif //_Debug

namespace Memory
{
	template <class T> void SafeDelete(T& t)
	{
		if (t)
		{
			delete t;
			t = nullptr;
		}
	}

	template <class T> void SafeDeleteArr(T& t)
	{
		if (t)
		{
			delete[] t;
			t = nullptr;
		}
	}

	template <class T> void SafeRelease(T& t)
	{
		if (t)
		{
			t->Release();
			t = nullptr;

		}
	}


}
