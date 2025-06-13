//
// Rigidbody.h
//

#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "Collision/CollisionSupport.h"
#include "Transform.h"
#include "Collision/Collider/PhysicsCollider.h"
#include "PhysicsSolver.h"

using namespace CollisionSupport;

class Rigidbody {
public:
	Rigidbody(int id,ColliderType type, PhysicsCollider* collider,SimpleMath::Vector3 position):m_id(id) , m_collider(collider) {
		//m_collider = new TetrahedronCollider(type);
	
		m_transform.SetPosition(position);
	}

	void Update(float deltaTime);
	void ImpactResponse(const Rigidbody* hitObject,ContactInfo& info,bool isA);
	void Correct(const Rigidbody* hitObject,SimpleMath::Vector3 normal, float penetrationDepth);
	void ApplyFrictionImpulse(const Rigidbody* hitObject, const ContactInfo& info, bool isA);
	void MoveBy(const SimpleMath::Vector3& offset) {
		m_transform.SetPosition(m_transform.GetPosition() + offset);}

	void UseGravity(bool use) { m_isGravity = use; }
	void SetStatic(bool isStatic) { m_isStatic = isStatic; }

	const bool IsStatic() const { return m_isStatic; }

	const DirectX::SimpleMath::Matrix GetWorldMatrix() const { return m_transform.GetWorldMatrix(); }
	PhysicsCollider* GetCollider() const { return m_collider; }
	const SimpleMath::Vector3 GetVelocity() const { return m_velocity; }
	const float GetMass() const { return m_mass; }
	const float GetFriction() const { return m_friction; }

private:
	Transform m_transform;
	PhysicsCollider* m_collider;
	PhysicsSolver m_solver;

	int m_id = -1;
	float m_mass = 1.0f;
	float m_restitution = 2;//îΩî≠åWêî
	float m_friction = 0.2f;

	bool m_isStatic = false;
	bool m_isGravity = true;
	float m_gravityScale = 1.0;

	SimpleMath::Vector3 m_velocity = SimpleMath::Vector3::Zero;//ïΩçsë¨ìx
	SimpleMath::Vector3 m_angularVelocity = SimpleMath::Vector3::Zero;//äpë¨ìx

};