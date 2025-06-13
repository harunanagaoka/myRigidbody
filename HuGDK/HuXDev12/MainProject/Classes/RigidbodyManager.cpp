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

			Rigidbody* rbA = it1->second;
			Rigidbody* rbB = it2->second;

			ContactInfo info = m_detector.DetectCollision(rbA, rbB);
			
			if (info.hasValue) {/* hasValue -> Õ“Ë‚ ‚è */
				rbA->ImpactResponse(rbB, info, true);
				rbB->ImpactResponse(rbA,info,false);
			}

		}
	}

}

void RigidbodyManager::UpdateAll() {
			PhysicsUpdate();
			//pair.second->Update();
}

void RigidbodyManager::PhysicsUpdate() {
	const int Step = 4;
	const float maxDeltaTime = 0.033f;
	float dt = DXTK->Time.deltaTime / Step;
	dt = dt = std::min(dt, maxDeltaTime);

	for (int i = 0; i < Step; ++i) {
		for (auto& pair : m_rigidbodyMap) {
			if (pair.second != nullptr) {
				pair.second->Update(dt);
			}
		}
		CheckCollision(); // © ‘S„‘Ì‚ª“®‚¢‚½‚ ‚Æ‚ÉˆêŠ‡”»’èI
	}


}