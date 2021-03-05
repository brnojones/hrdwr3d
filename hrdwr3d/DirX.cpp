
#include "DirX.h"
//
//struct VertexPos
//{
//	XMFLOAT3 pos;
//};

DirX::DirX()
{
	
}

DirX::~DirX()
{
	UnloadContent();

	if (_backBufferTarget) _backBufferTarget->Release();
	if (_swapChain) _swapChain->Release();
	if (_context) _context->Release();
	if (_device) _device->Release();
}

bool DirX::Initialise(HWND* hwnd, int windowWidth, int windowHeight)
{

	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = windowWidth;
	swapChainDesc.BufferDesc.Height = windowHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = *hwnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	
	unsigned int creationFlags = 0;

#ifdef _DEBUG
	//creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr;
	unsigned int driver = 0;

	hr = D3D11CreateDeviceAndSwapChain(
		0,
		driverType,
		0,
		creationFlags,
		0,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&_swapChain,
		&_device,
		&featureLevel,
		&_context);

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBoxA(NULL, "D3D11 not supported on this device", "ERROR", MB_OK | MB_ICONERROR);
		return false;
	}

	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed to create the Direct3D device", "ERROR", MB_OK | MB_ICONERROR);
		return false;
	}

	ID3D11Texture2D* backBufferTexture;

	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture);

	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed to get the swap chain back buffer", "ERROR", MB_OK | MB_ICONERROR);
		return false;
	}

	hr = _device->CreateRenderTargetView(backBufferTexture, 0, &_backBufferTarget);

	if (backBufferTexture)
	{
		backBufferTexture->Release();
	}

	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Failed to create the render target view", "ERROR", MB_OK | MB_ICONERROR);
		return false;
	}
	_context->OMSetRenderTargets(1, &_backBufferTarget, 0);

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(windowWidth);
	viewport.Height = static_cast<float>(windowHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	_context->RSSetViewports(1, &viewport);

	return LoadContent();
}

bool DirX::CompileShader(std::string filePath, std::string entry, std::string shaderModel, ID3DBlob** buffer)
{
	DWORD shaderFlags = 0;// D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob* errorBuffer = 0;
	HRESULT result;

	result = D3DX11CompileFromFile(filePath.c_str(), 0, 0, entry.c_str(), shaderModel.c_str(), shaderFlags, 0, 0, buffer, &errorBuffer, 0);

	if (FAILED(result))
	{
		if (errorBuffer != 0)
		{
			OutputDebugStringA((char*)errorBuffer->GetBufferPointer());
			errorBuffer->Release();
		}
		return false;
	}

	if (errorBuffer != 0)
		errorBuffer->Release();

	return true;
}

bool DirX::InitPipeline()
{
	ID3DBlob* vsBuffer = 0;
	ID3DBlob* psBuffer = 0;

	//	Compile vertex and pixel shader
	bool vsResult = CompileShader("shader.shader", "VS_Main", "vs_4_0", &vsBuffer);
	bool psResult = CompileShader("shader.shader", "VS_Main", "vs_4_0", &vsBuffer);

	if (vsResult == false)
	{
		MessageBox(0, "Error loading vertex shader!", "Compile Error", MB_OK);
		return false;
	}

	if (psResult == false)
	{
		MessageBox(0, "Error loading pixel shader!", "Compile Error", MB_OK);
		return false;
	}

	//	Shaders into objects
	HRESULT d3dVsResult = _device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, &_vertexShader);
	HRESULT d3dPsResult = _device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, &_pixelShader);

	if (FAILED(d3dVsResult))
	{
		if (vsBuffer)
			vsBuffer->Release();
		return false;
	}
	if (FAILED(d3dPsResult))
	{
		if (psBuffer)
			psBuffer->Release();
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOUR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	unsigned int totalLayoutElements = ARRAYSIZE(inputLayout);

	HRESULT d3dLayResult = _device->CreateInputLayout(
		inputLayout,
		totalLayoutElements,
		vsBuffer->GetBufferPointer(),
		vsBuffer->GetBufferSize(),
		&_inputLayout);
	
	if (FAILED(d3dLayResult))
	{
		return false;
	}
}

bool DirX::InitGraphics()
{
//	Vertex vertices[] =
//	{
//		/*XMFLOAT3(0.0f, 0.5f, 1.0f),
//		XMFLOAT3(0.5f, -0.5f, 1.0f),
//		XMFLOAT3(-0.5f, -0.5f, 1.0f)*/
//	};
	
	
	int x = 0;
}

bool DirX::LoadContent()
{
	InitPipeline();
	InitGraphics();

	/*ID3DBlob* vsBuffer = 0;

	bool compileResult = CompileShader("shader.fx", "VS_Main", "vs_4_0", &vsBuffer);

	if (compileResult == false)
	{
		MessageBox(0, "Error loading vertex shader!", "Compile Error", MB_OK);
		return false;
	}

	HRESULT d3dResult;
	d3dResult = _device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, &_vertexShader);

	if (FAILED(d3dResult))
	{
		if (vsBuffer)
			vsBuffer->Release();
		return false;
	}
	
	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{	
		{ 
			"POSITION", 
			0, 
			DXGI_FORMAT_R32G32B32_FLOAT,	
			0, 
			0, 
			D3D11_INPUT_PER_VERTEX_DATA, 
			0 
		}
	};
	
	unsigned int totalLayoutElements = ARRAYSIZE(inputLayout);
	
	d3dResult = _device->CreateInputLayout(
		inputLayout, 
		totalLayoutElements, 
		vsBuffer->GetBufferPointer(),
		vsBuffer->GetBufferSize(), 
		&_inputLayout);

	vsBuffer->Release();

	if (FAILED(d3dResult))
	{
		return false;
	}

	ID3DBlob* psBuffer = 0;
	
	compileResult = CompileShader("shader.fx", "PS_Main", "ps_4_0", &psBuffer);
	
	if (compileResult == false)
	{
		MessageBox(0, "Error loading pixel shader!", "Compile Error", MB_OK);
		return false;
	}

	d3dResult = _device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, &_pixelShader);

	psBuffer->Release();

	if (FAILED(d3dResult))
	{
		return false;
	}*/
	
	VertexPos vertices[] =
	{
		XMFLOAT3(0.0f, 0.5f, 1.0f),
		XMFLOAT3(0.5f, -0.5f, 1.0f),
		XMFLOAT3(-0.5f, -0.5f, 1.0f)
	};

	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(vertexDesc));

	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(VertexPos) * 3;
	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(resourceData));
	resourceData.pSysMem = vertices;

	d3dResult = _device->CreateBuffer(&vertexDesc, &resourceData, &_vertexBuffer);
	
	if (FAILED(d3dResult))
	{
		return false;
	}

	return true;
}

void DirX::UnloadContent()
{
	if (_vertexShader) _vertexShader->Release();
	if (_pixelShader) _pixelShader->Release();
	if (_inputLayout) _inputLayout->Release();
	if (_vertexBuffer) _vertexBuffer->Release();

	if (_swapChain) _swapChain->Release();
	if (_backBufferTarget) _backBufferTarget->Release();
	if (_device) _device->Release();
	if (_context) _context->Release();
}

void DirX::Update(float dt)
{
	/*for (int i = 0; i < 3; i++)
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		
		bool goUp = true;
		if (r > 0.5f)
		{
			goUp = false;
		}

		float f = _bufferColour[i];
		f += goUp ? 0.01f : -0.01f;
		f += r;
		_bufferColour[i] = f;

		while (_bufferColour[i] > 1.0f)
		{
			f = _color[i];
			f -= 1.0f;
			_color[i] = f;
		}
		while (_bufferColour[i] < 0.0f)
		{
			f = _bufferColour[i];
			f += 1.0f;
			_bufferColour[i] = f;
		}
	}*/
}

void DirX::Render()
{
	if (_context == 0)
	{
		return;
	}

	_context->ClearRenderTargetView(_backBufferTarget, _bufferColour);

	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	_context->IASetInputLayout(_inputLayout);
	_context->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_context->VSSetShader(_vertexShader, 0, 0);
	_context->PSSetShader(_pixelShader, 0, 0);
	_context->Draw(3, 0);

	_swapChain->Present(0, 0);
}
