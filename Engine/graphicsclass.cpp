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
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
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
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(64.0f);

	// Define all scene models (stores as data rather then copy & paste blocks) //
	struct ModelDef
	{
		char* modelFile;
		WCHAR* textureFile;
		ShaderType shader;
		XMFLOAT3   position;
		XMFLOAT3   scale;
		float      rotationSpeed;
		XMFLOAT3   rotationAxis;
	};

	ModelDef modelDefs[] =
	{
		// Model 1: spinning cube (marble texture)
		{ "../Engine/data/cube.txt",          L"../Engine/data/marble.dds",
		ShaderType::Texture,
		{ -3.5f, 0.0f, 0.0f }, // position
		{ 1.0f, 1.0f, 1.0f }, // scale
		3.0f,				// rotation speed
		{ 0.0f, 0.0f, 1.0f } // rotation axis
		},

		// Model 2: ninja head (metal texture)
		{ "../Engine/data/new-ninjaHead.txt", L"../Engine/data/metal.dds",
		ShaderType::Light,
		{ 0.0f, 0.0f, 0.0f },    // position
		{ 0.03f, 0.03f, 0.03f }, // scale
		1.0f,					 // rotation speed
		{ 0.0f, 1.0f, 0.0f } }, // rotation axis

		// Model 4: iceland terrain (dessert texture)
		{ "../Engine/data/iceland.txt",       L"../Engine/data/dessert2.dds",
			  ShaderType::Texture, { 0.0f, -50.0f, 200.0f }, { 10.0f, 3.0f, 10.0f }, 0.0f, { 0.0f, 0.0f, 0.0f } },
	};

	for (auto& def : modelDefs) // loop to reduce duplication (easier to add more models in the future)
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
		m_SceneModels.push_back(sm);
	}

	// Bump-mapped models (separate pipeline) //
	BumpModelClass* bumpModel = new BumpModelClass;
	if (!bumpModel)
	{
		return false;
	}
	// Initialize the bump model object.
	result = bumpModel->Initialize(m_D3D->GetDevice(), "../Engine/data/cube.txt",
		L"../Engine/data/stone.dds", L"../Engine/data/normal.dds");

	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bump model object.", L"Error", MB_OK);
		return false;
	}

	// store bump model and properties in a list (rendering each frame)
	SceneBumpModel sbm;
	sbm.model = bumpModel;
	sbm.position = { 3.5f, 0.0f, 0.0f };
	sbm.scale = { 1.0f, 1.0f, 1.0f };
	sbm.rotationSpeed = 1.0f / 3.0f;
	sbm.rotationAxis = { 1.0f, 0.0f, 0.0f };
	m_SceneBumpModels.push_back(sbm);

	return true;
}


void GraphicsClass::Shutdown()
{
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

	// Release the D3D object.
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	// Update the system stats.
	m_Timer->Frame();

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

	// Do the frame input processing.
	result = HandleMovementInput(m_Timer->GetTime());
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = Render();
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

bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

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

	// Render all models using the texture or light shader. 
	for (auto& sm : m_SceneModels)
	{
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
		if (sm.shaderType == ShaderType::Texture)
		{
			result = m_ShaderManager->RenderTextureShader(m_D3D->GetDeviceContext(), sm.model->GetIndexCount(),
				worldMatrix, viewMatrix, projectionMatrix, sm.model->GetTexture());
		}
		else // ShaderType::Light
		{
			result = m_ShaderManager->RenderLightShader(m_D3D->GetDeviceContext(), sm.model->GetIndexCount(),
				worldMatrix, viewMatrix, projectionMatrix, sm.model->GetTexture(),
				m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
		}

		if (!result)
		{
			return false;
		}
	}

	// Render all bump-mapped models.
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

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}