////////////////////////////////////////////////////////////////////////////////
// Filename: pbrshaderclass.h
////////////////////////////////////////////////////////////////////////////////

#ifndef _PBRSHADERCLASS_H_
#define _PBRSHADERCLASS_H_

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
// Class name: PBRShaderClass
////////////////////////////////////////////////////////////////////////////////
class PBRShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

	struct LightBufferType
	{
		XMFLOAT4 lightColor;
		XMFLOAT3 lightDirection;
		float padding;
	};

	// Debug visualization modes:
	// 0 = Full PBR (default)
	// 1 = Albedo
	// 2 = Normal Map
	// 3 = Metallic
	// 4 = Roughness
	// 5 = AO
	struct DebugBufferType
	{
		int debugMode;
		XMFLOAT3 padding;
	};

public:
	PBRShaderClass();
	PBRShaderClass(const PBRShaderClass&);
	~PBRShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, const XMMATRIX&, const XMMATRIX&, const XMMATRIX&,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		XMFLOAT3, XMFLOAT4, XMFLOAT3, int debugMode = 0);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, const XMMATRIX&, const XMMATRIX&, const XMMATRIX&,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		XMFLOAT3, XMFLOAT4, XMFLOAT3, int debugMode);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_debugBuffer;
	ID3D11SamplerState* m_sampleState;
};

#endif

