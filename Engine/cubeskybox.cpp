////////////////////////////////////////////////////////////////////////////////
// Filename: CubeSkyBox.cpp
////////////////////////////////////////////////////////////////////////////////
#include "CubeSkyBox.h"
#include <vector>


CubeSkyBox::CubeSkyBox()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}


CubeSkyBox::CubeSkyBox(const CubeSkyBox& other)
{
}


CubeSkyBox::~CubeSkyBox()
{
}


bool CubeSkyBox::Initialize(ID3D11Device* device, const wchar_t* textureFilename)
{
	bool result;

	// Initialize the vertex and index buffers.
	result = LoadAndInitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}


void CubeSkyBox::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}


void CubeSkyBox::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int CubeSkyBox::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* CubeSkyBox::GetTexture()
{
	return m_Texture->GetTexture();
}


bool CubeSkyBox::LoadAndInitializeBuffers(ID3D11Device* device)
{
	CubeSkyboxVertexType* skyboxVertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	//NOTE Hard coded values, gross...just doing it for now

	// Create the vertex array.
	skyboxVertices = new CubeSkyboxVertexType[8]
	{
		{ DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f) },  // 0 - Top Left Front
		{ DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f) },  // 1 - Top Right Front
		{ DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f) },  // 2 - Top Right Back
		{ DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f) },  // 3 - Top Left Back
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f) },  // 4 - Bottom Left Front
		{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f) },  // 5 - Bottom Right Front
		{ DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f) },  // 6 - Bottom Right Back
		{ DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f) },  // 7 - Bottom Left Back
	};

	indices = new unsigned long[36]
		{
			// Front face
			0, 1, 2, 0, 2, 3,
			// Back face
			4, 6, 5, 4, 7, 6,
			// Left face
			4, 5, 1, 4, 1, 0,
			// Right face
			3, 2, 6, 3, 6, 7,
			// Top face
			7, 4, 0, 7, 0, 3,
			// Bottom face
			5, 6, 2, 5, 2, 1
		};


	const int vertexCount = 8;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(CubeSkyboxVertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = skyboxVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	m_indexCount = 36;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] skyboxVertices;
	skyboxVertices = 0;
	delete[] indices;
	indices = 0;

	return true;
}


void CubeSkyBox::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void CubeSkyBox::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(CubeSkyboxVertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool CubeSkyBox::LoadTexture(ID3D11Device* device, const wchar_t* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}


void CubeSkyBox::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}