//
// TestStage.cpp
//

#include "TestStage.h"

void TestStage::Initialize() {
	m_objManager.CreateObject(ObjectType::Player);
	m_objManager.CreateObject(ObjectType::Enemy);
	m_UIManager.CreateUI();
};

void TestStage::Update() {
	m_objManager.UpdateAll();
};

void TestStage::Render() {
	m_objManager.RenderAll();
	m_UIManager.RenderAll();
};