//
// MainScene.h
//

#pragma once

#include "Scene.h"
#include "Classes/CreateResources/ShaderManager.h"
#include "Classes/CreateResources/PipelineManager.h"
#include "Classes/CreateResources/CameraManager.h"
#include "Classes/CreateResources/EffectManager.h"
#include "Classes/OriginalGeometric.h"
#include "Classes/RigidbodyOld.h"
#include "Classes/ObjManager.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;
using namespace DirectX;
using namespace MyGeometory;

class MainScene final : public Scene {
public:
	MainScene();
	virtual ~MainScene() { Terminate(); }

	MainScene(MainScene&&) = default;
	MainScene& operator= (MainScene&&) = default;

	MainScene(MainScene const&) = delete;
	MainScene& operator= (MainScene const&) = delete;

	// These are the functions you will implement.
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
	ShaderManager m_shader;
	PipelineManager m_pipeline;
	CameraManager m_camera;
	EffectManager m_effectManager;

	ObjManager m_objManager;

	std::shared_ptr<Primitive> shape_;

	float frameTimer = 0.0f;
	int frameCount = 0;
	float fps = 0.0f;
};