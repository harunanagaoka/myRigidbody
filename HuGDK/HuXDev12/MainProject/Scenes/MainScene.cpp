//
// MainScene.cpp
//

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "SceneFactory.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")


#ifdef _DEBUG
#pragma warning(disable : 4189)
#endif

using namespace SimpleMath;

MainScene::MainScene()
{

}

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

void MainScene::CreateDeviceDependentResources()
{

	auto&& device      = DXTK->Device;
	auto&& commandList = DXTK->CommandList;

	/*•`‰æ€”õ*/
	m_shader.LoadShaders();
	m_pipeline.BuildRootSignature();
	m_pipeline.BuildPipelineState(m_shader.GetVS(), m_shader.GetPS());
	m_camera.Initialize();
	m_effectManager.Initialize();

	m_stage.SetShape(ShapeName::Box);
	m_cube.SetShape(ShapeName::Box);

}

void MainScene::CreateResources()
{

}

// Initialize a variable and audio resources.
void MainScene::Initialize()
{
	constexpr float cameraZ = -20.0f;

	m_camera.Get().SetViewLookAt(Vector3(0.0f, 0.0f, cameraZ), Vector3::Zero, Vector3::UnitY);
	m_camera.Get().SetPerspectiveFieldOfView(
		Mathf::PI / 4.0f,(float)DXTK->SwapChain.Width / (float)DXTK->SwapChain.Height,
		0.1f, 10000.0f);

	//’n–Ê‚ÌÝ’è
	constexpr SimpleMath::Vector3 stagePosition(0.0f, -1.0f, 0.0f);
	constexpr SimpleMath::Vector3 stageSize(10.0f, 0.5f, 15.0f);

	Rigidbody* rb = m_rigidbodyManager.AddRigidbody(ColliderType::Box, stagePosition);
	rb->SetStatic(true);
	m_stage.SetRigidbody(rb);
	m_stage.SetSize(stageSize);

	//—§•û‘Ì‚ÌÝ’è
	constexpr SimpleMath::Vector3 cubePosition(0.0f, 2.0f, 0.0f);
	constexpr SimpleMath::Vector3 cubeSize(1.0f, 1.0f, 1.0f);

	rb = m_rigidbodyManager.AddRigidbody(ColliderType::Box, cubePosition);
	m_cube.SetRigidbody(rb);
	m_cube.SetSize(cubeSize);
}

void MainScene::Terminate()
{
	DXTK->Audio.Engine->Suspend();

	DXTK->Audio.ResetEngine();
	DXTK->WaitForGpu();

}

void MainScene::OnDeviceLost()
{

}

void MainScene::OnRestartSound()
{

}

NextScene MainScene::Update(const float deltaTime)
{
	//Rigidbody‚Ì“–‚½‚è”»’èA•¨—‰‰ŽZ‚ðˆêŠ‡‚Ås‚¢‚Ü‚·B
	m_rigidbodyManager.Update();

	return NextScene::Continue;
}

// Draws the scene.
void MainScene::Render()
{
	DXTK->BeginScene();
	DXTK->ClearRenderTarget(Colors::CornflowerBlue);

	auto&& device      = DXTK->Device;
	auto&& commandList = DXTK->CommandList;

	DirectXTK::BasicEffect& effect = m_effectManager.Get();
	DirectXTK::Camera& camera = m_camera.Get();

	effect->SetView(camera.GetViewMatrix());
	effect->SetProjection(camera.GetProjectionMatrix());

	//Rigidbody‚Ì‰e‹¿‚ðŽó‚¯‚½shape‚ð•`‰æ
	m_stage.Render(effect);
	m_cube.Render(effect);

	DXTK->EndScene();

}

