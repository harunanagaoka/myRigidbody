//
// TestStage.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "..\Camera.h"
#include "..\Object\ObjectManager.h"
#include "..\Object\ObjectRegistry.h"
#include "..\UI\UIManager.h"

class TestStage {
public:
	TestStage(Camera& camera) : m_objManager(camera){};

	void Initialize();

	void Update();

	void Render();


private:
	ObjectManager m_objManager;
	UIManager m_UIManager;
};

//デバッグルーム的な
//UI,Object生成担当
//ゆくゆくはステージメイカーみたいな名前付けてなんやかんやしたいなあ
