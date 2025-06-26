//
// RigidbodyManager.h
// Rigidbody�Ǘ��N���X

#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "Core/Rigidbody.h"
#include "Collision/Collider/PhysicsColliderFactory.h"
#include "Collision/Detection/CollisionDetection.h"
#include "Resistry/PhysicsTypes.h"
#include <unordered_map>

class RigidbodyManager {
public:
	RigidbodyManager(){}
	~RigidbodyManager() {
		for (auto& pair : m_rigidbodyMap) {

			if (pair.second != nullptr) {
				delete pair.second;
			}
		}
	};

	Rigidbody* AddRigidbody(ColliderType type, SimpleMath::Vector3 position);

	void CheckCollision();

	void Update();

	void PhysicsUpdate();

	void HandleDemoInput();

private:
	PhysicsColliderFactory m_colliderFactory;

	CollisionDetection m_detector;

	unordered_map<int, Rigidbody*> m_rigidbodyMap;
	
	const int m_step = 4;//1�t���[�����ŉ��񕨗����Z���s����
	const float m_maxDeltaTime = 0.033f;

	//�ȉ��f���p
	const int m_boxIDDemo = 1;
	const SimpleMath::Vector3 m_boxJumpHeightDemo = SimpleMath::Vector3(0, 5, 0);
};
