////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

// Inlucdes //
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "d3dclass.h"
#include "timerclass.h"
#include "shadermanagerclass.h"
#include "positionclass.h"
#include "cameraclass.h"
#include "lightclass.h"
#include "modelclass.h"
#include "bumpmodelclass.h"
#include "pbrmodelclass.h"
#include "cubeskybox.h"
#include "rendertextureclass.h";

#include "fpsclass.h"
#include "frustumclass.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f; // increase near plane to reduce shadow acne


// Shadow map depth bias — tweak if you see shadow acne or peter-panning.
const float shadow_bias = 0.00001f;


////////////////////////////////////////////////////////////////////////////////
// Describes how a model is placed and rendered in the scene.
////////////////////////////////////////////////////////////////////////////////
enum class ShaderType // which shader to use when rendering a model
{
	Texture,
	Light,
	Shadow,
};

struct SceneModel 
{
	ModelClass* model;
	ShaderType  shaderType;
	XMFLOAT3    position;
	XMFLOAT3    scale;
	float       rotationSpeed;  // radians per ms multiplier
	XMFLOAT3    rotationAxis;   // which axis to rotate around (1,0,0) (0,1,0) (0,0,1)
	bool		useFrustumCulling;
	
};

struct SceneBumpModel 
{
	BumpModelClass* model;
	XMFLOAT3        position;
	XMFLOAT3        scale;
	float            rotationSpeed;
	XMFLOAT3        rotationAxis;
};

struct ScenePBRModel
{
	PBRModelClass* model;
	XMFLOAT3       position;
	XMFLOAT3       scale;
	float          rotationSpeed;
	XMFLOAT3       rotationAxis;
	bool		   castsShadow;
	bool	       useFrustumCulling;
	bool		   useInstancing;
};


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

private:
	//bool Render(float);
	//Xu
	bool HandleMovementInput(float);
	bool UpdateFPS();

	bool RenderDepthToTexture(); // pass 1: built shadow map
	bool Render(); // pass 2: render scene

	
private:
	InputClass* m_Input;
	D3DClass* m_D3D;
	TimerClass* m_Timer;
	ShaderManagerClass* m_ShaderManager;
	PositionClass* m_Position;
	CameraClass* m_Camera;
	LightClass* m_Light;
	CubeSkyBox* m_Skybox;
	ModelClass* m_LightModel;
	// FPS & Frustum
	FpsClass* m_Fps;
	FrustumClass* m_Frustum;

	HWND m_hwnd;

	int m_previousFps;
	int m_renderCount;
	// Shadow map render target
	RenderTextureClass* m_ShadowMap;

	// scene model
	std::vector<SceneModel> m_SceneModels;
	std::vector<SceneBumpModel> m_SceneBumpModels;
	std::vector<ScenePBRModel> m_ScenePBRModels;

	// PBR debug visualization mode (0=Full PBR, 1=Albedo, 2=Normal, 3=Metallic, 4=Roughness, 5=AO)
	int m_pbrDebugMode;

	// Sphere position
	XMFLOAT3 m_SpherePos;

	// bool 
};

#endif