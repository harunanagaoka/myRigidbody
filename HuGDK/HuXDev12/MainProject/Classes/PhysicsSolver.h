//
//PhysicsSolver.h
//d—Í‚ÆÕ“ËŒã‚Ì“®‚«‚ğtransform‚É”½‰f‚·‚é

#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "Transform.h"
#include "Collision\BoxCollider.h"

class PhysicsSolver{
public:
	void Update(Transform& transform);

	void ComputeGravity(float dt);

	void UseGravity(bool use) { m_isGravity = use; }

	void SetStatic(bool isStatic) { m_isStatic = isStatic; }


private:

	SimpleMath::Vector3 m_velocity = SimpleMath::Vector3::Zero;

	bool m_isStatic = false;

	//d—Í
	bool m_isGravity = true;
	float m_gravityScale = 1.0;
	float m_mass = 1.0;
	const float m_gravityAccelConst = 9.8;

	
	
};


