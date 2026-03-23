////////////////////////////////////////////////////////////////////////////////
// Filename: CubeSkyBox.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CUBESKYBOX_H_
#define _CUBESKYBOX_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11_1.h>
#include <DirectXMath.h> 
using namespace DirectX;

using namespace std;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: CubeSkyBox
////////////////////////////////////////////////////////////////////////////////
class CubeSkyBox
{
private:
	struct CubeSkyboxVertexType
	{
		XMFLOAT3  position;
	};

public:
	CubeSkyBox();
	CubeSkyBox(const CubeSkyBox&);
	~CubeSkyBox();

	bool Initialize(ID3D11Device*, const wchar_t*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();


private:
	bool LoadAndInitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, const wchar_t*);
	void ReleaseTexture();

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount = 0, m_indexCount = 0;
	TextureClass* m_Texture;
};

#endif