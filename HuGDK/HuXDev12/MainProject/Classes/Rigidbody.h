//
// Rigidbody.h
//

#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "Transform.h"
#include "Collision/Collider/PhysicsCollider.h"
#include "PhysicsSolver.h"


class Rigidbody {
public:
	Rigidbody(int id,ColliderType type, PhysicsCollider* collider,SimpleMath::Vector3 position):m_id(id) , m_collider(collider) {
		//m_collider = new TetrahedronCollider(type);
	
		m_transform.SetPosition(position);
	}

	void Update();
	void ImpactResponse();

	void UseGravity(bool use) { m_solver.UseGravity(use); }
	void SetStatic(bool isStatic) { m_solver.SetStatic(isStatic); }

	const DirectX::SimpleMath::Matrix GetWorldMatrix() const { return m_transform.GetWorldMatrix(); }
	PhysicsCollider* GetCollider() const { return m_collider; }

private:
	Transform m_transform;
	PhysicsCollider* m_collider;
	PhysicsSolver m_solver;

	int m_id = -1;
};