//
// RigidbodyManager.cpp
// 

#include "RigidbodyManager.h"

Rigidbody* RigidbodyManager::AddRigidbody(ColliderType type, SimpleMath::Vector3 position) {
	static int id = 0;
	
	PhysicsCollider* collider = m_colliderFactory.CreateCollider(type);
	Rigidbody* rb = new Rigidbody(id,type, collider,position);
	m_rigidbodyMap[id] = rb;
	id++;

	return rb;
}

void RigidbodyManager::CheckCollision() {

	//d•¡‚È‚µ‘“–‚½‚è
	for (auto it1 = m_rigidbodyMap.begin(); it1 != m_rigidbodyMap.end(); ++it1) {
		auto it2 = it1;
		++it2; 

		for (; it2 != m_rigidbodyMap.end(); ++it2) {

			Rigidbody* rb1 = it1->second;
			Rigidbody* rb2 = it2->second;

			ContactInfo info = m_detector.DetectCollision(rb1, rb2);
			
			if (info.hasValue) {/* hasValue -> Õ“Ë‚ ‚è */
				rb1->ImpactResponse();
				rb2->ImpactResponse();
			}

		}
	}

}

void RigidbodyManager::UpdateAll() {

	for (auto& pair : m_rigidbodyMap) {

		if (pair.second != nullptr) {
			pair.second->Update();
		}
	}

	CheckCollision();

}