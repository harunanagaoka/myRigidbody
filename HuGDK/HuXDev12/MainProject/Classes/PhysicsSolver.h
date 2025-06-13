//
//PhysicsSolver.h
//重力と衝突後の動きをtransformに反映する

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

	SimpleMath::Vector3 m_velocity = SimpleMath::Vector3::Zero;//平行速度
	SimpleMath::Vector3 m_angularvelocity = SimpleMath::Vector3::Zero;//角速度

	const float m_gravityAccelConst = 9.8;
};


