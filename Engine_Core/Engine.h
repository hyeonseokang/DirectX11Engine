#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <string>
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")

namespace Engine_Core {
	class Engine {
	public:
		Engine();
		~Engine();

		void Init(HWND hwnd, UINT width, UINT height);
		void Run();

	private:
		void Update();
		void Render();

		HWND mHwnd;
		UINT mWidth;
		UINT mHeight;
		
	private:
		DWORD dwShaderFlags;
		ID3DBlob* errorBlob = nullptr;

		UINT m4xMsaaQuality = 0; // 초기화 중요!
		bool CreateDeviceAndSwapChaing();
		bool CreateDepthStencilBuffer();
		bool SetViewport(int width, int height);
		bool InitShader();
		bool CreateVertextShader(const std::wstring& filePath);
		bool CreatePixelShader(const std::wstring& filePath);
		bool SetInputLayout();

		ID3D11Device* md3device;
		ID3D11DeviceContext* md3context;
		IDXGISwapChain* mSwapChain;
		ID3D11RenderTargetView* mRenderTargetView;
		ID3D11Texture2D* mDepthStencilBuffer;
		ID3D11DepthStencilView* mDepthStencilView;

		ID3D11Buffer* mVertexBuffer = nullptr;
		ID3DBlob* vsBlob = nullptr;
		ID3DBlob* psBlob = nullptr;
		ID3D11VertexShader* mVS = nullptr;
		ID3D11PixelShader* mPS = nullptr;
		ID3D11InputLayout* mInputLayout = nullptr;

		void InitWindowRect(UINT width, UINT height);
	};
}