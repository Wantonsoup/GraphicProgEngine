////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <DirectXMath.h> 

using namespace DirectX;



////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////
class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);
	void SetPosition(float, float, float);
	void SetLookAt(float, float, float);

	XMFLOAT4  GetAmbientColor();
	XMFLOAT4  GetDiffuseColor();
	XMFLOAT3  GetDirection();
	XMFLOAT4  GetSpecularColor();
	XMFLOAT3 GetPosition();
	float GetSpecularPower();
	
	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);
	void GenerateOrthographicMatrix(float, float, float, float);
	void GetViewMatrix(XMMATRIX&);
	void GetProjectionMatrix(XMMATRIX&);
	
private:
	XMFLOAT4  m_ambientColor;
	XMFLOAT4  m_diffuseColor;
	XMFLOAT3  m_direction;
	XMFLOAT3 m_position;
	XMFLOAT4  m_specularColor;
	float m_specularPower;

	XMFLOAT3 m_lookAt;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;
};

#endif