////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

// Inlucdes //
#include <vector>


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


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

////////////////////////////////////////////////////////////////////////////////
// Describes how a model is placed and rendered in the scene.
////////////////////////////////////////////////////////////////////////////////
enum class ShaderType // which shader to use when rendering a model
{
	Texture,
	Light
};

struct SceneModel 
{
	ModelClass* model;
	ShaderType  shaderType;
	XMFLOAT3    position;
	XMFLOAT3    scale;
	float       rotationSpeed;  // radians per ms multiplier
	XMFLOAT3    rotationAxis;   // which axis to rotate around (1,0,0) (0,1,0) (0,0,1)
};

struct SceneBumpModel 
{
	BumpModelClass* model;
	XMFLOAT3        position;
	XMFLOAT3        scale;
	float            rotationSpeed;
	XMFLOAT3        rotationAxis;
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
	bool Render();

private:
	InputClass* m_Input;
	D3DClass* m_D3D;
	TimerClass* m_Timer;
	ShaderManagerClass* m_ShaderManager;
	PositionClass* m_Position;
	CameraClass* m_Camera;
	LightClass* m_Light;
	
	// scene model
	std::vector<SceneModel> m_SceneModels;
	std::vector<SceneBumpModel> m_SceneBumpModels;
};

#endif