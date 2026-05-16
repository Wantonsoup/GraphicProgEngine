////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_Input = 0;
	m_D3D = 0;
	m_Timer = 0;
	m_ShaderManager = 0;
	m_Light = 0;
	m_Position = 0;
	m_Camera = 0;
	m_Skybox = 0;
	m_pbrDebugMode = 0;
	m_ShadowMap = 0;
	m_LightModel = 0;
	m_SpherePos = XMFLOAT3(0.0f, 5.0f, 0.0f);

	m_Fps = 0;
	m_Frustum = 0;
	m_hwnd = 0;
	m_previousFps = -1;
	m_renderCount = 0;
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	m_hwnd = hwnd;

	bool result;

	// Create the input object.
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the shader manager object.
	m_ShaderManager = new ShaderManagerClass;
	if (!m_ShaderManager)
	{
		return false;
	}

	// Initialize the shader manager object.
	result = m_ShaderManager->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shader manager object.", L"Error", MB_OK);
		return false;
	}

	// Create the timer object.
	m_Timer = new TimerClass;
	if (!m_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	// Create the position object.
	m_Position = new PositionClass;
	if (!m_Position)
	{
		return false;
	}

	// Set the initial position and rotation of the viewer.
	m_Position->SetPosition(0.0f, 0.0f, -10.0f);
	m_Position->SetRotation(0.0f, 0.0f, 0.0f);

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Create the light object.
	
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	
	m_Light->SetAmbientColor(0.35f, 0.35f, 0.35f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(64.0f);
	/*
	m_Light->SetDirection(0.0f, 0.0f, 1.0f); 
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);
	*/
	
	// Create and initialize the FPS counter.
	m_Fps = new FpsClass;
	if (!m_Fps)
	{
		return false;
	}

	m_Fps->Initialize();
	m_previousFps = -1;


	// Create the frustum class object.
	m_Frustum = new FrustumClass;
	if (!m_Frustum)
	{
		return false;
	}


	// Create the skybox object.
	m_Skybox = new CubeSkyBox;
	if(!m_Skybox)
	{
		return false;
	}

	// Initialize the skybox object.
	result = m_Skybox->Initialize(m_D3D->GetDevice(), L"../Engine/data/skymap.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the skybox.", L"Error", MB_OK);
		return false;
	}

	// Create the shadow map object.
	m_ShadowMap = new RenderTextureClass;
	if(!m_ShadowMap)
	{
		return false;
	}

	//Initialize the shadow map object.
	result = m_ShadowMap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, 1);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the shadow map render texture.", L"Error", MB_OK);
		return false;
	}

	// Create the light model object (for visualizing the light position in the scene).

	m_LightModel = new ModelClass;
	result = m_LightModel->Initialize(m_D3D->GetDevice(),
		"../Engine/data/sphere.txt", L"../Engine/data/pbr/tree/roughness.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize light model.", L"Error", MB_OK);
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Define all scene models (stores as data rather then copy & paste blocks) //
	//////////////////////////////////////////////////////////////////////////////
	struct ModelDef
	{
		char* modelFile;
		WCHAR* textureFile;
		ShaderType shader;
		XMFLOAT3   position;
		XMFLOAT3   scale;
		float      rotationSpeed;
		XMFLOAT3   rotationAxis;
		bool useFrustumCulling;
		
	};

	ModelDef modelDefs[] =
	{
		// Model 1: spinning cube (marble texture)
		{ "../Engine/data/cube.txt",   L"../Engine/data/marble.dds",
		ShaderType::Shadow,
		{ -3.5f, 1.0f, 0.0f }, // position
		{ 1.0f, 1.0f, 1.0f }, // scale
		0.0f,				// rotation speed
		{ 1.0f, 0.0f, 0.0f }, // rotation axis
		true
		},

		// Model 2: ninja head (metal texture)
		{ "../Engine/data/new-ninjaHead.txt", L"../Engine/data/metal.dds",
		ShaderType::Shadow,
		{ 0.0f, 1.0f, 0.0f },    // position
		{ 0.03f, 0.03f, 0.03f }, // scale
		0.0f,					 // rotation speed
		{ 0.0f, 1.0f, 0.0f },	 // rotation axis
		true
		}, 

		// model 5: land
		{ "../Engine/data/cube.txt", L"../Engine/data/pbr/grass/albedo.dds",
		ShaderType::Shadow,
		{ 0.0f, -6.0f, 0.0f },
		{ 20.0f, 1.0f, 20.0f },
		0.0f,
		{ 0.0f, 1.0f, 0.0f },
		true,
		},
		
		// model 5: land
		{ "../Engine/data/cube.txt", L"../Engine/data/pbr/grass/albedo.dds",
		ShaderType::Shadow,
		{ 0.0f, -7.0f, 0.0f },
		{ 200.0f, 1.0f, 150.0f },
		0.0f,
		{ 0.0f, 1.0f, 0.0f },
		true,
		},
		
		

		
	};

	// loop to reduce duplication (easier to add more models in the future)
	for (auto& def : modelDefs) 
	{
		ModelClass* model = new ModelClass;

	
		if (!model)
		{
			return false;
		}

		result = model->Initialize(m_D3D->GetDevice(), def.modelFile, def.textureFile);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize a model object.", L"Error", MB_OK);
			return false;
		}

		// store models and properties in a list (rendering each frame)
		SceneModel sm;
		sm.model = model;
		sm.shaderType = def.shader;
		sm.position = def.position;
		sm.scale = def.scale;
		sm.rotationSpeed = def.rotationSpeed;
		sm.rotationAxis = def.rotationAxis;
		sm.useFrustumCulling = def.useFrustumCulling;
		m_SceneModels.push_back(sm);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Define all PBR scene models (stores as data rather then copy & paste blocks) //
	/////////////////////////////////////////////////////////////////////////////////
	
	const int TREE_COUNT = 10; // number of trees to render
	const float TREE_AREA_SIZE = 20.0f; // area size for random tree placement
	
	struct PBRModelDef
	{
		char* modelFile;

		WCHAR* albedoTexture;
		WCHAR* normalTexture;
		WCHAR* metallicTexture;
		WCHAR* roughnessTexture;
		WCHAR* aoTexture;

		XMFLOAT3 position;
		XMFLOAT3 scale;
		float rotationSpeed;
		XMFLOAT3 rotationAxis;

		bool castsShadow;
		bool useFrustumCulling;

		bool useInstancing;
		int instanceCount;
		XMFLOAT3 instanceAreaCenter;
		XMFLOAT2 instanceAreaSize;

	};

	PBRModelDef pbrModelDefs[] =
	{
		// PBR Model 1: tree group 1
		{ "../Engine/data/tree.obj",
		L"../Engine/data/pbr/grass/albedo.dds",
		L"../Engine/data/pbr/grass/normal.dds",
		L"../Engine/data/pbr/grass/metallic.dds",
		L"../Engine/data/pbr/grass/roughness.dds",
		L"../Engine/data/pbr/grass/ao.dds",

		{ -80.0f, -6.0f, 60.0f }, // position
		{ 1.0f, 1.0f, 1.0f }, // scale
		0.0f,				  // rotation speed
		{ 0.0f, 1.0f, 0.0f }, // rotation axis
		true,				  // castsShadow = true
		false,				  // useFrustumCulling = true
		true,				  // useInstancing = true
		300,
		{ 0.0f, 0.0f, 0.0f }, // instanceAreaCenter
		{ 80.0f, 80.0f }        // instanceAreaSize: x width, z depth
		},

		// PBR Model 1: tree group 2
		{ "../Engine/data/tree.obj",
		L"../Engine/data/pbr/grass/albedo.dds",
		L"../Engine/data/pbr/grass/normal.dds",
		L"../Engine/data/pbr/grass/metallic.dds",
		L"../Engine/data/pbr/grass/roughness.dds",
		L"../Engine/data/pbr/grass/ao.dds",

		{ 0.0f, -6.0f, 0.0f }, // position
		{ 1.0f, 1.0f, 1.0f }, // scale
		0.0f,				  // rotation speed
		{ 0.0f, 1.0f, 0.0f }, // rotation axis
		true,				  // castsShadow = true
		false,				  // useFrustumCulling = true
		true,				  // useInstancing = true
		600,
		{ 0.0f, 0.0f, 0.0f }, // instanceAreaCenter
		{ 200.0f, 200.0f }        // instanceAreaSize: x width, z depth
		},
		

		// PBR Model 2: stone
		{ "../Engine/data/stones/stone_1.obj",
		L"../Engine/data/pbr/stone/albedo.dds",
		L"../Engine/data/pbr/stone/normal.dds",
		L"../Engine/data/pbr/stone/metallic.dds",
		L"../Engine/data/pbr/stone/roughness.dds",
		L"../Engine/data/pbr/stone/ao.dds",

		{ 11.0f, 0.0f, 0.0f }, // position
		{ 1.0f, 1.0f, 1.0f }, // scale
		0.0f,				  // rotation speed
		{ 0.0f, 1.0f, 0.0f }, // rotation axis
		true,				  // castsShadow = true
		true,				  // useFrustumCulling = true
		false,				  // useInstancing = true
		1,                     // instanceCount
		{ 0.0f, 0.0f, 0.0f },   // instanceAreaCenter
		{ 0.0f, 0.0f }          // instanceAreaSize
		},

		// PBR Model 3: mountain
		{ "../Engine/data/land.obj",
		L"../Engine/data/pbr/land/albedo.dds",
		L"../Engine/data/pbr/land/normal.dds",
		L"../Engine/data/pbr/land/metallic.dds",
		L"../Engine/data/pbr/land/roughness.dds",
		L"../Engine/data/pbr/land/ao.dds",

		{ -725.0f, -10.0f, 900.0f }, 
		{ 15.0f, 15.0f, 15.0f },
		0.0f,
		{ 0.0f, 0.0f, 0.0f }, // rotation axis
		true,				  // castsShadow = true
		false,				  // useFrustumCulling = true
		false,				  // useInstancing = true
		1,                     // instanceCount
		{ 0.0f, 0.0f, 0.0f },   // instanceAreaCenter
		{ 0.0f, 0.0f }          // instanceAreaSize
		},

		// PBR Model 4: cabin
		{ "../Engine/data/cabin.obj",
		L"../Engine/data/pbr/wood/albedo.dds",
		L"../Engine/data/pbr/wood/normal.dds",
		L"../Engine/data/pbr/wood/metallic.dds",
		L"../Engine/data/pbr/wood/roughness.dds",
		L"../Engine/data/pbr/wood/ao.dds",

		{ -70.0f, -6.0f, 90.0f }, // position
		{ 2.0f, 2.0f, 2.0f }, // scale
		0.0f,				  // rotation speed
		{ 0.0f, 1.0f, 0.0f }, // rotation axis
		true,				  // castsShadow = true
		false,				  // useFrustumCulling = true
		false,				  // useInstancing = true
		1,                     // instanceCount
		{ 0.0f, 0.0f, 0.0f },   // instanceAreaCenter
		{ 0.0f, 0.0f }          // instanceAreaSize
		},
		

		// PBR Model 5: test cube
		{ "../Engine/data/cube.txt",
		L"../Engine/data/pbr/tree/albedo.dds",
		L"../Engine/data/pbr/tree/normal.dds",
		L"../Engine/data/pbr/tree/metallic.dds",
		L"../Engine/data/pbr/tree/roughness.dds",
		L"../Engine/data/pbr/tree/ao.dds",

		{ -50.0f, 3.0f, 70.0f }, // position
		{ 1.0f, 1.0f, 1.0f }, // scale
		0.0f,				  // rotation speed
		{ 0.0f, 1.0f, 0.0f }, // rotation axis
		true,				  // castsShadow = true
		false,				  // useFrustumCulling = true
		false,				  // useInstancing = true
		1,                     // instanceCount
		{ 0.0f, 0.0f, 0.0f },   // instanceAreaCenter
		{ 0.0f, 0.0f }          // instanceAreaSize
		},
		
		
	};

	// need loop for each mataerial 
	for (auto& def : pbrModelDefs)
	{
		PBRModelClass* pbrModel = new PBRModelClass;
		if (!pbrModel)
		{
			return false;
		}
		if (def.useInstancing)
		{
			result = pbrModel->InitializeInstanced(
				m_D3D->GetDevice(),
				def.modelFile,
				def.albedoTexture,
				def.normalTexture,
				def.metallicTexture,
				def.roughnessTexture,
				def.aoTexture,
				def.instanceCount,
				def.instanceAreaCenter,
				def.instanceAreaSize
			);
		}
		else
		{
			result = pbrModel->Initialize(
				m_D3D->GetDevice(),
				def.modelFile,
				def.albedoTexture,
				def.normalTexture,
				def.metallicTexture,
				def.roughnessTexture,
				def.aoTexture
			);
		}

		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize a PBR model object.", L"Error", MB_OK);
			return false;
		}

		ScenePBRModel spm;
		spm.model = pbrModel;
		spm.position = def.position;
		spm.scale = def.scale;
		spm.rotationSpeed = def.rotationSpeed;
		spm.rotationAxis = def.rotationAxis;
		spm.castsShadow = def.castsShadow;
		spm.useFrustumCulling = def.useFrustumCulling;
		spm.useInstancing = def.useInstancing;
		m_ScenePBRModels.push_back(spm);
	}
	

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the input object.
	if (m_D3D && m_D3D->GetDeviceContext())
	{
		m_D3D->GetDeviceContext()->ClearState();
		m_D3D->GetDeviceContext()->Flush();
	}

	// Release all scene models. (reduced the dupes by using loop)
	for (auto& sm : m_SceneModels)
	{
		if (sm.model)
		{
			sm.model->Shutdown();
			delete sm.model;
			sm.model = 0;
		}
	}
	m_SceneModels.clear();

	// Release all bump models.
	for (auto& sbm : m_SceneBumpModels)
	{
		if (sbm.model)
		{
			sbm.model->Shutdown();
			delete sbm.model;
			sbm.model = 0;
		}
	}
	m_SceneBumpModels.clear();

	// Release all PBR models.
	for (auto& spm : m_ScenePBRModels)
	{
		if (spm.model)
		{
			spm.model->Shutdown();
			delete spm.model;
			spm.model = 0;
		}
	}
	m_ScenePBRModels.clear();

	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the position object.
	if (m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	// Release the shader manager object.
	if (m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}

	// Release the timer object.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// Release the frustum object.
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	// Release the FPS object.
	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	//Release the skybox object.
	if (m_Skybox)
	{
		m_Skybox->Shutdown();
		delete m_Skybox;
		m_Skybox = 0;
	}

	// Release the shadow map object.
	if(m_ShadowMap)
	{
		m_ShadowMap->Shutdown();
		delete m_ShadowMap;
		m_ShadowMap = 0;
	}
	// Release the light model object.
	if (m_LightModel) 
	{
		m_LightModel->Shutdown(); 
		delete m_LightModel; 
		m_LightModel = 0; 
	}
	// Release the D3D object.
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	// Update the system stats.
	m_Timer->Frame();
	float frameTime = m_Timer->GetTime();


	// Read the user input.
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->IsEscapePressed() == true)
	{
		return false;
	}


	// PBR debug: press 1-6 to switch debug visualization mode.
	// 1 = Full PBR, 2 = Albedo, 3 = Normal, 
	// F4 = Metallic, 5 = Roughness, 6 = AO
	if (m_Input->IsKeyPressed(DIK_1))
		m_pbrDebugMode = 0;
	else if (m_Input->IsKeyPressed(DIK_2))
		m_pbrDebugMode = 1;
	else if (m_Input->IsKeyPressed(DIK_3))
		m_pbrDebugMode = 2;
	else if (m_Input->IsKeyPressed(DIK_4))
		m_pbrDebugMode = 3;
	else if (m_Input->IsKeyPressed(DIK_5))
		m_pbrDebugMode = 4;
	else if (m_Input->IsKeyPressed(DIK_6))
		m_pbrDebugMode = 5;


	// Do the frame input processing.
	result = HandleMovementInput(m_Timer->GetTime());
	if (!result)
	{
		return false;
	}

	
	// Move the light position each frame to make the shadows visible.
	// Update the position of the light each frame.
	// Move sphere/light position however you like
	static float angle = 0.0f;
	angle += frameTime * 0.0005f;
	m_SpherePos.x = sinf(angle) * 60.0f;
	m_SpherePos.y = 20.0f;
	m_SpherePos.z = cosf(angle) * 60.0f;

	// Light follows the sphere exactly
	m_Light->SetPosition(m_SpherePos.x, m_SpherePos.y, m_SpherePos.z);
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light->GenerateViewMatrix();
	m_Light->GenerateOrthographicMatrix(-30.0f, 30.0f, SCREEN_NEAR, SCREEN_DEPTH);

	// Pass 1: depth-only render from light POV
	result = RenderDepthToTexture();
	if (!result) 
	{ 
		return false; 
	}

	// Pass 2: full scene render with shadows
	result = Render();
	if (!result) 
	{ 
		return false; 
	}

	// Read the FPS and update the window caption.
	result = UpdateFPS();
	if (!result)
	{
		return false;
	}


	return true;
}

bool GraphicsClass::HandleMovementInput(float frameTime)
{
	bool keyDown;
	float posX, posY, posZ, rotX, rotY, rotZ;

	// Set the frame time for calculating the updated position.
	m_Position->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = m_Input->IsLeftPressed();
	m_Position->MoveLeft(keyDown);

	keyDown = m_Input->IsRightPressed();
	m_Position->MoveRight(keyDown);

	keyDown = m_Input->IsUpPressed();
	m_Position->MoveForward(keyDown);

	keyDown = m_Input->IsDownPressed();
	m_Position->MoveBackward(keyDown);

	keyDown = m_Input->IsAPressed();
	m_Position->MoveUpward(keyDown);

	keyDown = m_Input->IsZPressed();
	m_Position->MoveDownward(keyDown);

	keyDown = m_Input->IsPgUpPressed();
	m_Position->LookUpward(keyDown);

	keyDown = m_Input->IsPgDownPressed();
	m_Position->LookDownward(keyDown);

	// Handle mouse rotations.
	m_Position->MouseRotate(m_Input->GetMouseXDelta(), m_Input->GetMouseYDelta());

	// Get the view point position/rotation.
	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);
	m_Input->ResetCursorPos();

	// Set the position of the camera.
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	return true;
}

bool GraphicsClass::UpdateFPS()
{
	static float elapsedTime = 0.0f;
	static int frameCount = 0;

	wchar_t fpsString[128];

	float frameTime = m_Timer->GetTime();

	elapsedTime += frameTime;
	frameCount++;

	if (elapsedTime >= 1000.0f)
	{
		int fps = frameCount;

		swprintf_s(
			fpsString,
			128,
			L"Engine - FPS: %d | Rendered: %d",
			fps,
			m_renderCount
		);

		SetWindowText(m_hwnd, fpsString);

		frameCount = 0;
		elapsedTime = 0.0f;
	}

	return true;

}

bool GraphicsClass::RenderDepthToTexture()
{
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	bool result;

	static float rotation = 0.0f;
	rotation += (float)XM_PI * 0.0005f * m_Timer->GetTime();

	// Unbind shadow map from previous frame before using it as a render target again.
	ID3D11ShaderResourceView* nullSRV[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	m_D3D->GetDeviceContext()->PSSetShaderResources(0, 6, nullSRV);

	// Redirect rendering to the shadow map texture.
	m_ShadowMap->SetRenderTarget(m_D3D->GetDeviceContext());
	m_ShadowMap->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);


	// Render every Shadow type scene model into the depth map.
	for (auto& sm : m_SceneModels)
	{
		if (sm.shaderType != ShaderType::Shadow) { continue; }

		XMMATRIX scaleMatrix = XMMatrixScaling(sm.scale.x, sm.scale.y, sm.scale.z);
		XMMATRIX rotMatrix = XMMatrixIdentity();
		if (sm.rotationSpeed != 0.0f)
		{
			float angle = rotation * sm.rotationSpeed;
			rotMatrix = XMMatrixRotationAxis(XMLoadFloat3(&sm.rotationAxis), angle);
		}
		XMMATRIX transMatrix = XMMatrixTranslation(sm.position.x, sm.position.y, sm.position.z);
		XMMATRIX worldMatrix = XMMatrixMultiply(XMMatrixMultiply(scaleMatrix, rotMatrix), transMatrix);

		sm.model->Render(m_D3D->GetDeviceContext());
		result = m_ShaderManager->RenderDepthShader(m_D3D->GetDeviceContext(),
			sm.model->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		if (!result) { return false; }
	}

	for (auto& spm : m_ScenePBRModels)
	{
		if (!spm.castsShadow)
		{
			continue;
		}

		XMMATRIX scaleMatrix = XMMatrixScaling(spm.scale.x, spm.scale.y, spm.scale.z);

		XMMATRIX rotMatrix = XMMatrixIdentity();
		if (spm.rotationSpeed != 0.0f)
		{
			float angle = rotation * spm.rotationSpeed;
			rotMatrix = XMMatrixRotationAxis(XMLoadFloat3(&spm.rotationAxis), angle);
		}

		XMMATRIX transMatrix = XMMatrixTranslation(spm.position.x, spm.position.y, spm.position.z);
		XMMATRIX worldMatrix = XMMatrixMultiply(XMMatrixMultiply(scaleMatrix, rotMatrix), transMatrix);

		spm.model->Render(m_D3D->GetDeviceContext());

		if (spm.useInstancing)
		{
			result = m_ShaderManager->RenderDepthShaderInstanced(
				m_D3D->GetDeviceContext(),
				spm.model->GetIndexCount(),
				spm.model->GetInstanceCount(),
				worldMatrix,
				lightViewMatrix,
				lightProjectionMatrix
			);
		}
		else
		{
			result = m_ShaderManager->RenderDepthShader(
				m_D3D->GetDeviceContext(),
				spm.model->GetIndexCount(),
				worldMatrix,
				lightViewMatrix,
				lightProjectionMatrix
			);
		}

		if (!result)
		{
			return false;
		}
	}

	// Restore the back buffer as the render target.
	ID3D11RenderTargetView* backBuffer = m_D3D->GetRenderTargetView();
	ID3D11DepthStencilView* dsv = m_D3D->GetDepthStencilView();
	m_D3D->SetBackBufferRenderTarget();
	m_D3D->ResetViewport();
	return true;
}

bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	bool result;

	worldMatrix = XMMatrixMultiply(
		XMMatrixScaling(0.3f, 0.3f, 0.3f),
		XMMatrixTranslation(m_SpherePos.x, m_SpherePos.y, m_SpherePos.z)
	);

	static float rotation = 0.0f;

	// Update the rotation variable each frame.
	rotation += (float)XM_PI * 0.0005f * m_Timer->GetTime();

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the view and projection matrices.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Get the light's view and projection matrices.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	// Get Frustum for culling
	m_Frustum->ConstructFrustum(viewMatrix, projectionMatrix, SCREEN_DEPTH);

	m_renderCount = 0;


	//disable depth & render skybox
	m_D3D->DisableDepth();

	XMMATRIX skyboxViewMatrix = viewMatrix;
	skyboxViewMatrix.r[3] = DirectX::XMVectorSet(0, 0, 0, 1); // Remove translation, we don't want to translate the cube based on the view matrix

	

	// Render the skybox
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Skybox->Render(m_D3D->GetDeviceContext());
	result = m_ShaderManager->RenderCubeMapShader(m_D3D->GetDeviceContext(), m_Skybox->GetIndexCount(), worldMatrix, skyboxViewMatrix, projectionMatrix, m_Skybox->GetTexture());

	if (!result)
	{
		return false;
	}

	m_D3D->EnableDepth();

	// Render all models using the shader enum. 
	for (auto& sm : m_SceneModels)
	{
		// Cull models outside the frustum (sphere culling)
		float cullRadius = max(max(sm.scale.x, sm.scale.y), sm.scale.z) * 2.0f;
		if (sm.useFrustumCulling)
		{
			bool visible = m_Frustum->CheckSphere(
				sm.position.x,
				sm.position.y,
				sm.position.z,
				cullRadius
			);

			if (!visible)
			{
				continue;
			}
		}

		// Build the world matrix: Scale -> Rotate -> Translate.
		XMMATRIX scaleMatrix = XMMatrixScaling(sm.scale.x, sm.scale.y, sm.scale.z);
		XMMATRIX rotMatrix = XMMatrixIdentity(); 
		if (sm.rotationSpeed != 0.0f)
		{
			float angle = rotation * sm.rotationSpeed;
			rotMatrix = XMMatrixRotationAxis(XMLoadFloat3(&sm.rotationAxis), angle);
		}

		XMMATRIX transMatrix = XMMatrixTranslation(sm.position.x, sm.position.y, sm.position.z);
		worldMatrix = XMMatrixMultiply(XMMatrixMultiply(scaleMatrix, rotMatrix), transMatrix);

		// Bind model buffers.
		sm.model->Render(m_D3D->GetDeviceContext());

		// Dispatch to the correct shader.
		switch (sm.shaderType)
		{
			case ShaderType::Texture:
				result = m_ShaderManager->RenderTextureShader(m_D3D->GetDeviceContext(), sm.model->GetIndexCount(), sm.model->GetInstanceCount(),
						worldMatrix, viewMatrix, projectionMatrix, sm.model->GetTexture());
				break;

			case ShaderType::Light:
				result = m_ShaderManager->RenderLightShader(m_D3D->GetDeviceContext(), sm.model->GetIndexCount(), sm.model->GetInstanceCount(),
					worldMatrix, viewMatrix, projectionMatrix, sm.model->GetTexture(),
					m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
					m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
				break;
			case ShaderType::Shadow:
				result = m_ShaderManager->RenderShadowShader(m_D3D->GetDeviceContext(),
					sm.model->GetIndexCount(),
					worldMatrix, viewMatrix, projectionMatrix,
					lightViewMatrix, lightProjectionMatrix,
					sm.model->GetTexture(), m_ShadowMap->GetShaderResourceView(),
					m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
					m_Light->GetPosition(), shadow_bias);
				break;
			default:
				break;


		}
		if (!result) 
		{ 
			return false; 
		}

		m_renderCount++;
	}

	

	// Render all bump-mapped models.
	/*
	for (auto& sbm : m_SceneBumpModels)
	{
		XMMATRIX scaleMatrix = XMMatrixScaling(sbm.scale.x, sbm.scale.y, sbm.scale.z);

		XMMATRIX rotMatrix = XMMatrixIdentity();
		if (sbm.rotationSpeed != 0.0f)
		{
			float angle = rotation * sbm.rotationSpeed;
			rotMatrix = XMMatrixRotationAxis(XMLoadFloat3(&sbm.rotationAxis), angle);
		}

		XMMATRIX transMatrix = XMMatrixTranslation(sbm.position.x, sbm.position.y, sbm.position.z);
		worldMatrix = XMMatrixMultiply(XMMatrixMultiply(scaleMatrix, rotMatrix), transMatrix);

		sbm.model->Render(m_D3D->GetDeviceContext());
		result = m_ShaderManager->RenderBumpMapShader(m_D3D->GetDeviceContext(), sbm.model->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix,
			sbm.model->GetColorTexture(), sbm.model->GetNormalMapTexture(),
			m_Light->GetDirection(), m_Light->GetDiffuseColor());

		if (!result)
		{
			return false;
		}
	}
	*/


	// Render all PBR models.
	for (auto& spm : m_ScenePBRModels)
	{
		// Cull models outside the frustum.
		float cullRadius = max(max(spm.scale.x, spm.scale.y), spm.scale.z) * 2.0f;

		if (spm.useFrustumCulling)
		{
			bool visible = m_Frustum->CheckSphere(
				spm.position.x,
				spm.position.y,
				spm.position.z,
				cullRadius
			);

			if (!visible)
			{
				continue;
			}
		}

		// Build world matrix.
		XMMATRIX scaleMatrix = XMMatrixScaling(spm.scale.x, spm.scale.y, spm.scale.z);

		XMMATRIX rotMatrix = XMMatrixIdentity();
		if (spm.rotationSpeed != 0.0f)
		{
			float angle = rotation * spm.rotationSpeed;
			rotMatrix = XMMatrixRotationAxis(XMLoadFloat3(&spm.rotationAxis), angle);
		}

		XMMATRIX transMatrix = XMMatrixTranslation(spm.position.x, spm.position.y, spm.position.z);
		worldMatrix = XMMatrixMultiply(XMMatrixMultiply(scaleMatrix, rotMatrix), transMatrix);

		// IMPORTANT: bind vertex/index/instance buffers.
		spm.model->Render(m_D3D->GetDeviceContext());

		if (spm.useInstancing)
		{
			result = m_ShaderManager->RenderPBRShaderInstanced(
				m_D3D->GetDeviceContext(),
				spm.model->GetIndexCount(),
				spm.model->GetInstanceCount(),
				worldMatrix,
				viewMatrix,
				projectionMatrix,
				spm.model->GetAlbedoTexture(),
				spm.model->GetNormalMapTexture(),
				spm.model->GetMetallicTexture(),
				spm.model->GetRoughnessTexture(),
				spm.model->GetAOTexture(),
				m_ShadowMap->GetShaderResourceView(),
				lightViewMatrix,
				lightProjectionMatrix,
				m_Light->GetDirection(),
				m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(),
				m_pbrDebugMode
			);

			if (!result)
			{
				return false;
			}

			m_renderCount += spm.model->GetInstanceCount();
		}
		else
		{
			result = m_ShaderManager->RenderPBRShader(
				m_D3D->GetDeviceContext(),
				spm.model->GetIndexCount(),
				worldMatrix,
				viewMatrix,
				projectionMatrix,
				spm.model->GetAlbedoTexture(),
				spm.model->GetNormalMapTexture(),
				spm.model->GetMetallicTexture(),
				spm.model->GetRoughnessTexture(),
				spm.model->GetAOTexture(),
				m_ShadowMap->GetShaderResourceView(),
				lightViewMatrix,
				lightProjectionMatrix,
				m_Light->GetDirection(),
				m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(),
				m_pbrDebugMode
			);

			if (!result)
			{
				return false;
			}

			m_renderCount++;
		}
	}

	// Render the light sphere.
	XMFLOAT3 lightPos = m_Light->GetPosition();

	worldMatrix = XMMatrixMultiply(
		XMMatrixScaling(2.0f, 2.0f, 2.0f),
		XMMatrixTranslation(lightPos.x, lightPos.y, lightPos.z)
	);

	m_LightModel->Render(m_D3D->GetDeviceContext());

	result = m_ShaderManager->RenderTextureShader(
		m_D3D->GetDeviceContext(),
		m_LightModel->GetIndexCount(),
		m_LightModel->GetInstanceCount(),
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		m_LightModel->GetTexture()
	);

	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
};