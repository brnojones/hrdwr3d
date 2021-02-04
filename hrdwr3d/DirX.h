
#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include <iostream>
#include <string>
#include <xnamath.h>

class DirX
{
public:
	DirX();
	~DirX();

	bool Initialise(HWND* hwnd, int windowWidth, int windowHeight);

	bool LoadContent();
	void UnloadContent();

	void Update(float dt);
	void Render();

	bool CompileShader(std::string filePath, std::string entry, std::string shaderModel, ID3DBlob** buffer);

private:
	ID3D11Device* _device;
	ID3D11DeviceContext* _context;
	IDXGISwapChain* _swapChain;
	ID3D11RenderTargetView* _backBufferTarget;

	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _inputLayout;

	ID3D11Buffer* _vertexBuffer;

	float _bufferColour[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
};
