//
// RigidbodyManager.h
// Rigidbodyä«óùÉNÉâÉX

#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "Rigidbody.h"
#include "Collision/CollisionDetection.h"
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

	void UpdateAll();

private:
	CollisionDetection m_detector;
	unordered_map<int, Rigidbody*> m_rigidbodyMap;

};
