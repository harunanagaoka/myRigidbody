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

//�f�o�b�O���[���I��
//UI,Object�����S��
//�䂭�䂭�̓X�e�[�W���C�J�[�݂����Ȗ��O�t���ĂȂ�₩��₵�����Ȃ�
