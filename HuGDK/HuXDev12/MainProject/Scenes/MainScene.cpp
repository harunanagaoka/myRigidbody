//
// MainScene.cpp
//

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "SceneFactory.h"
#include <d3dcompiler.h>
#include <random> 
#pragma comment(lib, "d3dcompiler.lib")


#ifdef _DEBUG
#pragma warning(disable : 4189)
#endif

using namespace SimpleMath;

// Initialize member variables.
MainScene::MainScene()
{

}

// Start is called after the scene is created.
void MainScene::Start()
{
	LoadAssets();
	Initialize();
}

void MainScene::LoadAssets()
{
	CreateDeviceDependentResources();
	CreateResources();


}

// Allocate memory the Direct3D and Direct2D resources.
// These are the resources that depend on the device.
void MainScene::CreateDeviceDependentResources()
{

	auto&& device      = DXTK->Device;
	auto&& commandList = DXTK->CommandList;

	m_shader.LoadShaders();
	m_pipeline.BuildRootSignature();
	m_pipeline.BuildPipelineState(m_shader.GetVS(), m_shader.GetPS());
	m_camera.Initialize();
	m_effectManager.Initialize();

	shape_ = MyGeometory::Primitive::CreateCubePrimitive(5, device);

}

void MainScene::CreateResources()
{

}

// Initialize a variable and audio resources.
void MainScene::Initialize()
{

	m_camera.Get().SetViewLookAt(Vector3(0.0f, 0.0f, -20), Vector3::Zero, Vector3::UnitY);
	//�J�����̐ݒu�i�J������u�����W,�ڕW�̍��W�i�����j,�J������y�������j

	m_camera.Get().SetPerspectiveFieldOfView(
		Mathf::PI / 4.0f,(float)DXTK->SwapChain.Width / (float)DXTK->SwapChain.Height,
		0.1f, 10000.0f);

	//m_objManager.AddObject(1, SimpleMath::Vector3(0, 0, 0), false);
	
}

// Releasing resources required for termination.
void MainScene::Terminate()
{
	// TODO: Add a sound instance reset.
	DXTK->Audio.Engine->Suspend();


	DXTK->Audio.ResetEngine();
	DXTK->WaitForGpu();

	// TODO: Add your Termination logic here.

}

// Direct3D resource cleanup.
void MainScene::OnDeviceLost()
{
	shape_.reset();
}

// Restart any looped sounds here
void MainScene::OnRestartSound()
{

}
static std::random_device rd;                         // �V�[�h�i�n�[�h�E�F�A�x�[�X�����j
static std::mt19937 gen(rd());                        // �����Z���k�E�c�C�X�^�i�����G���W���j

// Updates the scene.
NextScene MainScene::Update(const float deltaTime)
{

	if (InputSystem.Mouse.was.leftButton == InputSystem.Mouse.was.PRESSED)
	{
		std::uniform_real_distribution<float> distX(-5.0f, 10.0f);
		std::uniform_real_distribution<float> distY(5.0f, 10.0f);
		float x = distX(gen);
		float y = distY(gen);
		m_objManager.AddObject(1,SimpleMath::Vector3(x,y,0),true);
	}

	m_objManager.UpdateAll();

	return NextScene::Continue;
}

// Draws the scene.
void MainScene::Render()
{
	DXTK->BeginScene();
	DXTK->ClearRenderTarget(Colors::CornflowerBlue);

	auto&& device      = DXTK->Device;
	auto&& commandList = DXTK->CommandList;

	//�J������`��ɔ��f
	DirectXTK::BasicEffect& effect = m_effectManager.Get();
	DirectXTK::Camera& camera = m_camera.Get();

	effect->SetView(camera.GetViewMatrix());
	effect->SetProjection(camera.GetProjectionMatrix());
	m_objManager.DrawAll(effect.get());

	DXTK->EndScene();

	
}

