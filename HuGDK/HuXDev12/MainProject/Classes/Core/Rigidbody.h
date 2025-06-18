//
// Rigidbody.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "Transform.h"
#include "..\Collision\Collider\PhysicsCollider.h"
#include "..\Collision\Detection\CollisionSupport.h"

using namespace CollisionSupport;
constexpr float EPSILON = 0.01f;

class Rigidbody {
public:
	Rigidbody(int id,ColliderType type, PhysicsCollider* collider,SimpleMath::Vector3 position):m_id(id) , m_collider(collider) {

		m_transform.SetPosition(position);
	}

	~Rigidbody(){

		delete m_collider;
	}

	void Update(float deltaTime);
	void AddForce(const SimpleMath::Vector3& force);
	void ImpactResponse(const Rigidbody* hitObject,ContactInfo& info,bool isA);

	// Set
	void UseGravity(bool use) { m_isGravity = use; }
	void SetStatic(bool isStatic) { m_isStatic = isStatic; }
	const bool IsStatic() const { return m_isStatic; }

	// Get
	const DirectX::SimpleMath::Matrix GetWorldMatrix() const { return m_transform.GetWorldMatrix(); }
	PhysicsCollider* GetCollider() const { return m_collider; }
	const SimpleMath::Vector3 GetVelocity() const { return m_velocity; }
	const float GetMass() const { return m_mass; }
	const float GetFriction() const { return m_friction; }

private:
	
	void Correct(const Rigidbody* hitObject,SimpleMath::Vector3 normal, float penetrationDepth);
	void ApplyFrictionImpulse(const Rigidbody* hitObject, const ContactInfo& info, bool isA);

	Transform m_transform;
	PhysicsCollider* m_collider;

	int m_id = -1;
	float m_mass = 1.0f;
	float m_restitution = 2;//反発係数
	float m_friction = 0.2f;
	float m_gravityScale = 1.0;
	const float m_gravitationalAcceleration = 9.8f;

	bool m_isStatic = false;
	bool m_isGravity = true;
	
	SimpleMath::Vector3 m_velocity = SimpleMath::Vector3::Zero;//平行速度
	SimpleMath::Vector3 m_angularVelocity = SimpleMath::Vector3::Zero;//角速度

	const float m_angularVelocityThreshold = 0.0001f;// 角速度の閾値
	const float m_correctionBias = 1.2f;//重力方向にだけめり込み補正を少し大きくしています。
};