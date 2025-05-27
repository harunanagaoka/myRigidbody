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

	m_obj.SetShape();
	m_objB.SetShape();

}

void MainScene::CreateResources()
{

}

// Initialize a variable and audio resources.
void MainScene::Initialize()
{

	m_camera.Get().SetViewLookAt(Vector3(0.0f, 0.0f, -20), Vector3::Zero, Vector3::UnitY);
	//カメラの設置（カメラを置く座標,目標の座標（向き）,カメラのy軸方向）

	m_camera.Get().SetPerspectiveFieldOfView(
		Mathf::PI / 4.0f,(float)DXTK->SwapChain.Width / (float)DXTK->SwapChain.Height,
		0.1f, 10000.0f);

	Rigidbody* rb = m_rigidbodyManager.AddRigidbody(ColliderType::Tetrahedron, SimpleMath::Vector3::Zero);
	rb->SetStatic(true);
	m_obj.SetRigidbody(rb);

	rb = m_rigidbodyManager.AddRigidbody(ColliderType::Tetrahedron, SimpleMath::Vector3(0, 5, 0));
	m_objB.SetRigidbody(rb);
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

}

// Restart any looped sounds here
void MainScene::OnRestartSound()
{

}
//static std::random_device rd;                         // シード（ハードウェアベース乱数）
//static std::mt19937 gen(rd());                        // メルセンヌ・ツイスタ（乱数エンジン）

// Updates the scene.
NextScene MainScene::Update(const float deltaTime)
{
	m_rigidbodyManager.UpdateAll();

	return NextScene::Continue;
}

// Draws the scene.
void MainScene::Render()
{
	DXTK->BeginScene();
	DXTK->ClearRenderTarget(Colors::CornflowerBlue);

	auto&& device      = DXTK->Device;
	auto&& commandList = DXTK->CommandList;

	//カメラを描画に反映
	DirectXTK::BasicEffect& effect = m_effectManager.Get();
	DirectXTK::Camera& camera = m_camera.Get();

	effect->SetView(camera.GetViewMatrix());
	effect->SetProjection(camera.GetProjectionMatrix());

	m_obj.Render(effect);
	m_objB.Render(effect);

	DXTK->EndScene();

	
}

