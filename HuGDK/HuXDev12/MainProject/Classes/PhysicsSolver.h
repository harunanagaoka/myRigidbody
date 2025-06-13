//
//PhysicsSolver.h
//�d�͂ƏՓˌ�̓�����transform�ɔ��f����

#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "Transform.h"
//#include "Collision\BoxCollider.h"

class PhysicsSolver{
public:
	void Update(Transform& transform);

	SimpleMath::Vector3 ComputeGravity(const SimpleMath::Vector3& position, SimpleMath::Vector3& velocity, const float gravityScale);

private:

	SimpleMath::Vector3 m_velocity = SimpleMath::Vector3::Zero;//���s���x
	SimpleMath::Vector3 m_angularvelocity = SimpleMath::Vector3::Zero;//�p���x

	const float m_gravityAccelConst = 9.8;
};


