#include "Engine.h"

using namespace DirectX;
// 삼각형 정점 구조체 (위치만 예제로)
struct Vertex
{
	DirectX::XMFLOAT3 Position;
};





Engine_Core::Engine::Engine() :mHwnd(nullptr), mWidth(0), mHeight(0)
{
}

Engine_Core::Engine::~Engine()
{
}

void Engine_Core::Engine::Init(HWND hwnd, UINT width, UINT height)
{

	mHwnd = hwnd;
	InitWindowRect(width, height);

	UINT flags = 0;
	D3D_FEATURE_LEVEL featurelevel;

	HRESULT hr = D3D11CreateDevice(
		0,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		flags, 0, 0,
		D3D11_SDK_VERSION,
		&md3device,
		&featurelevel,
		&md3context);

	if (FAILED(hr)) {
		OutputDebugString(L"Faild\n");
	}
	else {
		OutputDebugString(L"Success\n");
	}
	WCHAR currentDir[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, currentDir);
	OutputDebugStringW(L"Current Directory: ");
	OutputDebugStringW(currentDir);
	OutputDebugStringW(L"\n");

	UINT m4xMsaaQuality = 0; // 초기화 중요!

	hr = md3device->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);
	
	if (SUCCEEDED(hr)) {
		OutputDebugString(L"CheckMultisampleQualityLevels success\n!");
	}


	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mWidth;
	sd.BufferDesc.Height = mHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 4;
	sd.SampleDesc.Quality = m4xMsaaQuality - 1; // 4x MSAA
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = mHwnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;
	if (m4xMsaaQuality > 0) {
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else {
		// 폴백: MSAA 아예 끔
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	IDXGIDevice* dxgiDevice = 0;
	hr = md3device->QueryInterface(__uuidof(IDXGIDevice),
		(void**)&dxgiDevice);
	IDXGIAdapter* dxgiAdapter = 0;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter),
		(void**)&dxgiAdapter);
	IDXGIFactory* dxgiFactory = 0;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory),
		(void**)&dxgiFactory);

	dxgiFactory->CreateSwapChain(md3device, &sd, &mSwapChain);

	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	ID3D11Texture2D* backBuffer;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&backBuffer));
	md3device->CreateRenderTargetView(backBuffer, 0,
		&mRenderTargetView);
	backBuffer->Release();


	D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	depthStencilDesc.Width = mWidth;
	depthStencilDesc.Height = mHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	depthStencilDesc.SampleDesc.Count = 4;
	depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	 hr = md3device->CreateTexture2D(&depthStencilDesc, nullptr, &mDepthStencilBuffer);
	if (FAILED(hr)) {
		OutputDebugString(L"Depth buffer creation failed\n");
	}
	else
	{
		OutputDebugStringA("CreateTexture2D for depth succeeded\n");
	}
	hr = md3device->CreateDepthStencilView(mDepthStencilBuffer, nullptr, &mDepthStencilView);
	if (FAILED(hr)) {
		OutputDebugString(L"DepthStencilView creation failed\n");
	}
	else {

		OutputDebugStringA("CreateDepthStencilView succeeded\n");
	}

	md3context->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);


	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	md3context->RSSetViewports(1, &viewport);

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* errorBlob = nullptr;
	// Vertex Shader 컴파일[
	hr = D3DCompileFromFile(L"..//Engine_Core//test.fxh", nullptr, nullptr, "VS", "vs_4_0",
		dwShaderFlags, 0, &vsBlob, &errorBlob);

	//hr = D3DCompileFromFile(L"..\\WakWakEngine\\VS.hlsl", nullptr, nullptr, "main", "vs_4_0", 0, 0, &vsBlob, &errorBlob);
	if (FAILED(hr)) OutputDebugString(L"Vertex shader compile failed\n");
	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		OutputDebugString(L"Vertex shader compile failed\n");
		return;
	}


	md3device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &mVS);
	if (FAILED(hr))
	{
		psBlob->Release();
		return;
	}
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	md3device->CreateInputLayout(layout, numElements,
		vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mInputLayout);

	md3context->IASetInputLayout(mInputLayout);

	// Pixel Shader 컴파일
	hr = D3DCompileFromFile(L"..\\Engine_Core\\test.fxh", nullptr, nullptr, "PS", "ps_4_0",
		dwShaderFlags, 0, &psBlob, &errorBlob);


	//hr = D3DCompileFromFile(L"..\\WakWakEngine\\PS.hlsl", nullptr, nullptr, "main", "ps_4_0", 0, 0, &psBlob, &errorBlob);
	if (FAILED(hr)) OutputDebugString(L"Pixel shader compile failed\n");
	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		OutputDebugString(L"Vertex shader compile failed\n");
		return;
	}
	md3device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &mPS);


	// 버텍스 버퍼 생성
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(Vertex) * 3;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	Vertex vertices[] =
	{
		XMFLOAT3(0.0f, 0.5f, 0.0f),
		XMFLOAT3(0.5f, -0.5f,0.0f),
		XMFLOAT3(-0.5f, -0.5f, 0.0f),
	};

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	hr = md3device->CreateBuffer(&vbd, &initData, &mVertexBuffer);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create vertex buffer\n");
	}

	// Set vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	// Set primitive topology
	md3context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Engine_Core::Engine::Run()
{
	Update();
	Render();
}

void Engine_Core::Engine::Update()
{

}

void Engine_Core::Engine::Render()
{
	// 렌더타겟 클리어 (하얀 배경)
	float clearColor[4] = { 0.098039225f, 0.098039225f, 0.439215720f, 1.f };
	
	md3context->ClearRenderTargetView(mRenderTargetView, clearColor);
	md3context->ClearDepthStencilView(
		mDepthStencilView,
		D3D11_CLEAR_DEPTH,
		1.0f,   // 깊이 클리어 값
		0);    // 스텐실 클리어 값(안 쓰면 0)


	// Render a triangle
	md3context->VSSetShader(mVS, nullptr, 0);
	md3context->PSSetShader(mPS, nullptr, 0);
	md3context->Draw(3, 0);

	// Present the information rendered to the back buffer to the front buffer (the screen)
	mSwapChain->Present(0, 0);
}

void Engine_Core::Engine::InitWindowRect(UINT width, UINT height)
{
	RECT rect = { 0, 0, width, height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	mWidth = rect.right - rect.left;
	mHeight = rect.bottom - rect.top;

	SetWindowPos(mHwnd, nullptr, 0, 0, mWidth, mHeight, 0);
	ShowWindow(mHwnd, true);
}