//
// MainScene.h
//

#pragma once

#include "Scene.h"
#include "Classes/CreateResources/ShaderManager.h"
#include "Classes/CreateResources/PipelineManager.h"
#include "Classes/CreateResources/CameraManager.h"
#include "Classes/CreateResources/EffectManager.h"
#include "Classes/Simulation/SimulationObject.h"
#include "Classes/RigidbodyManager.h"
#include "Classes/Core/Rigidbody.h"
#include "Classes/Resistry/PhysicsTypes.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;
using namespace DirectX;

class MainScene final : public Scene {
public:
	MainScene();
	virtual ~MainScene() { Terminate(); }

	MainScene(MainScene&&) = default;
	MainScene& operator= (MainScene&&) = default;

	MainScene(MainScene const&) = delete;
	MainScene& operator= (MainScene const&) = delete;

	void Start() override;

	void CreateDeviceDependentResources() override;
	void CreateResources() override;

	void LoadAssets() override;

	void Initialize() override;
	void Terminate() override;

	void OnDeviceLost() override;
	void OnRestartSound() override;

	NextScene Update(const float deltaTime) override;
	void Render() override;

private:
	// ï`âÊä÷òA
	ShaderManager m_shader;
	PipelineManager m_pipeline;
	CameraManager m_camera;
	EffectManager m_effectManager;

	// ï®óùä÷òA
	RigidbodyManager m_rigidbodyManager;
	SimulationObject m_stage;
	SimulationObject m_cube;

};