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
	Rigidbody(int id,ColliderType type,SimpleMath::Vector3 position):m_id(id) {
		m_collider = new PhysicsCollider(type);
		m_transform.SetPosition(position);
	}

	void Update();
	void ImpactResponse();

	const DirectX::SimpleMath::Matrix GetWorldMatrix() const { return m_transform.GetWorldMatrix(); }

private:
	Transform m_transform;
	PhysicsCollider* m_collider;
	PhysicsSolver m_solver;

	int m_id = -1;
};