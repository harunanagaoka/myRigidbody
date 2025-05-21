//
// SimulationObject.h
//�����ڂƃ��W�b�h�{�f�B�̓����N���X

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "..\Rigidbody.h"

class SimulationObject  {
public:
	SimulationObject() : m_rigidbody(nullptr) {
	}

	~SimulationObject() = default;

	void SetShape(){
		m_shape = GeometricPrimitive::CreateTetrahedron();//�e�X�g�p�l�ʑ�
	}

	void SetRigidbody(Rigidbody* rb) { m_rigidbody = rb; }

	void Render(DirectXTK::BasicEffect& effect);

private:
	DirectXTK::GeometricPrimitive m_shape;
	Rigidbody* m_rigidbody;//delete�͊Ǘ��N���X�ňꊇ�ōs��
};




/*
//enum class ObjectType {��ޑ����Ă�����g��
//	Tetrahedron
//
//};*/