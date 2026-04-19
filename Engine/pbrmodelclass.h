#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: pbrmodelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PBRMODELCLASS_H_
#define _PBRMODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11_1.h>
#include <directXMath.h>
using namespace DirectX;

#include <fstream>
using namespace std;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: PBRModelClass
////////////////////////////////////////////////////////////////////////////////
class PBRModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	PBRModelClass();
	PBRModelClass(const PBRModelClass&);
	~PBRModelClass();

	bool Initialize(ID3D11Device*, char*, WCHAR*, WCHAR*, WCHAR*, WCHAR*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetAlbedoTexture();
	ID3D11ShaderResourceView* GetNormalMapTexture();
	ID3D11ShaderResourceView* GetMetallicTexture();
	ID3D11ShaderResourceView* GetRoughnessTexture();
	ID3D11ShaderResourceView* GetAOTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*, WCHAR*, WCHAR*);
	void ReleaseTextures();

	bool LoadModel(char*);
	void ReleaseModel();

	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ModelType* m_model;
	TextureClass* m_AlbedoTexture;
	TextureClass* m_NormalMapTexture;
	TextureClass* m_MetallicTexture;
	TextureClass* m_RoughnessTexture;
	TextureClass* m_AOTexture;
};

#endif
