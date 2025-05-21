//
//PhysicsSolver.h
//�d�͂ƏՓˌ�̓�����transform�ɔ��f����

#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "Transform.h"
#include "Collision\BoxCollider.h"

class PhysicsSolver{
public:
	void Update();

	void ComputeGravity(float dt);

	const SimpleMath::Matrix& Draw() { return m_transform.GetWorldMatrix(); }

private:
	Transform m_transform;

	SimpleMath::Vector3 m_velocity = SimpleMath::Vector3::Zero;

	//�d��
	bool m_isGravity = true;
	float m_gravityScale = 1.0;
	float m_mass = 0.5;
	const float m_gravityAccelConst = 9.8;

	
	
};


