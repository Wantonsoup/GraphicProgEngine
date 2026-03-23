////////////////////////////////////////////////////////////////////////////////
// Filename: cubemapshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CUBEMAPSHADERCLASS_H_
#define _CUBEMAPSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: cubemapshaderclass
////////////////////////////////////////////////////////////////////////////////
class CubeMapShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};


public:
	CubeMapShaderClass();
	CubeMapShaderClass(const CubeMapShaderClass&);
	~CubeMapShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, const XMMATRIX&, const XMMATRIX&, const XMMATRIX&, ID3D11ShaderResourceView*);

private:
	bool InitializeShader(ID3D11Device*, HWND, const wchar_t*, const wchar_t*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, const wchar_t*);

	bool SetShaderParameters(ID3D11DeviceContext*, const XMMATRIX&, const XMMATRIX&, const XMMATRIX&, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
};

#endif