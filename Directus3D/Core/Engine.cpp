/*
Copyright(c) 2016 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES =========================
#include "Engine.h"
#include "Socket.h"
#include "Scene.h"
#include "Timer.h"
#include "../IO/Log.h"
#include "../Pools/GameObjectPool.h"
#include "../Scripting/ScriptEngine.h"
#include "../Graphics/Renderer.h"
#include "../Loading/ModelLoader.h"
#include "../Input/Input.h"
//====================================

Engine::Engine(HINSTANCE instance, HWND windowHandle, HWND drawPaneHandle)
{
	m_engineSocket = nullptr;
	m_scriptEngine = nullptr;
	m_renderer = nullptr;
	m_imageLoader = nullptr;
	m_modelLoader = nullptr;
	m_scene = nullptr;
	m_input = nullptr;
	m_timer = nullptr;
	m_physicsEngine = nullptr;
	m_meshPool = nullptr;
	m_materialPool = nullptr;
	m_texturePool = nullptr;

	Initialize(instance, windowHandle, drawPaneHandle);
}

Engine::~Engine()
{
	Shutdown();
}

void Engine::Initialize(HINSTANCE instance, HWND windowHandle, HWND drawPaneHandle)
{
	/*------------------------------------------------------------------------------
								[COMPONENT INITIALIZATION]
	------------------------------------------------------------------------------*/
	// 1 - DEBUG LOG
	Log::Initialize();

	// 2 - D3D11
	m_D3D11Device = new D3D11Device();
	m_D3D11Device->Initialize(drawPaneHandle);

	// 3 - TIMER
	m_timer = new Timer();
	m_timer->Initialize();

	// 4 - INPUT
	m_input = new Input();
	m_input->Initialize(instance, windowHandle);

	// 5 - PHYSICS ENGINE
	m_physicsEngine = new PhysicsEngine();
	m_physicsEngine->Initialize();

	// 6 - SCRIPT ENGINE
	m_scriptEngine = new ScriptEngine(m_timer, m_input);
	m_scriptEngine->Initialize();

	// 7 - SHADER POOL
	m_shaderPool = new ShaderPool(m_D3D11Device);

	// 8 - MESH POOL
	m_meshPool = new MeshPool();

	// 9 - IMAGE LOADER
	m_imageLoader = new ImageLoader(m_D3D11Device);

	// 10 - TEXTURE POOL
	m_texturePool = new TexturePool(m_imageLoader);

	// 11 - MODEL LOADER
	m_modelLoader = new ModelLoader();
	m_modelLoader->Initialize(m_meshPool, m_texturePool, m_shaderPool);

	// 12 - MATERIAL POOL
	m_materialPool = new MaterialPool(m_texturePool, m_shaderPool);

	// 13 - RENDERER
	m_renderer = new Renderer();
	m_scene = new Scene(m_texturePool, m_materialPool, m_meshPool, m_scriptEngine, m_physicsEngine, m_modelLoader, m_renderer);
	m_renderer->Initialize(true, m_D3D11Device, m_imageLoader, m_timer, m_physicsEngine, m_scene);

	// 14 - GAMEOBJECT POOL
	GameObjectPool::GetInstance().Initialize(m_D3D11Device, m_scene, m_meshPool, m_materialPool, m_texturePool, m_shaderPool, m_physicsEngine, m_scriptEngine);

	// 15 - SCENE	
	m_scene->Initialize();

	// 16 - ENGINE SOCKET
	m_engineSocket = new Socket(m_scene, m_renderer, m_timer, m_modelLoader, m_physicsEngine, m_imageLoader);
}

void Engine::Shutdown()
{
	/*------------------------------------------------------------------------------
								[COMPONENT SHUTDOWN]
	------------------------------------------------------------------------------*/
	// 16 - ENGINE INTERFACE
	delete m_engineSocket;

	// 15 - SCENE
	delete m_scene;

	// 14 - GAMEOBJECT POOL
	GameObjectPool::GetInstance().Release();

	// 13 - RENDERER
	delete m_renderer;

	// 12 - MATERIAL POOL
	delete m_materialPool;

	// 11 - MODEL LOADER
	delete m_modelLoader;

	// 10 - TEXTURE POOL
	delete m_texturePool;

	// 9 - IMAGE LOADER
	delete m_imageLoader;

	// 8 - MESH POOL
	delete m_meshPool;

	// 7 - SHADER POOL
	delete m_shaderPool;

	// 6 - SCRIPT ENGINE
	delete m_scriptEngine;

	// 5 - PHYSICS ENGINE
	delete m_physicsEngine;

	// 4 - INPUT
	delete m_input;

	// 3 - TIMER
	delete m_timer;

	//2 - D3D11
	delete m_D3D11Device;

	// 1- DEBUG LOG
	Log::Release();
}

void Engine::Run()
{
	// update time
	m_timer->Update();

	// update input
	m_input->Update();

	// update physics
	m_physicsEngine->Update();

	// update gaemeobjects
	GameObjectPool::GetInstance().Update();

	// update scene
	m_scene->Update();

	// render
	m_renderer->Render();
}

Socket* Engine::GetSocket()
{
	return m_engineSocket;
}