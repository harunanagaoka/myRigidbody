//
// RigidbodyManager.cpp
// 

#include "RigidbodyManager.h"

Rigidbody* RigidbodyManager::AddRigidbody(ColliderType type, SimpleMath::Vector3 position) {
	static int id = 0;
	Rigidbody* rb = new Rigidbody(id,type, position);
	m_rigidbodyMap[id] = rb;
	id++;

	return rb;
}

void RigidbodyManager::CheckCollision() {

	//重複なし総当たり
	for (auto it1 = m_rigidbodyMap.begin(); it1 != m_rigidbodyMap.end(); ++it1) {
		auto it2 = it1;
		++it2; 

		for (; it2 != m_rigidbodyMap.end(); ++it2) {

			CollisionDetection::ContactInfo info;

			Rigidbody* rb1 = it1->second;
			Rigidbody* rb2 = it2->second;

			//if(m_detector.Collision(rb1,rb2,info;))
			rb1->ImpactResponse();
			rb2->ImpactResponse();

			// ここで rb1 と rb2 のペアに対する処理を書くケロ！
			// 例: 衝突チェックや距離計算など
		}
	}

}

void RigidbodyManager::UpdateAll() {

	CheckCollision();

	for (auto& pair : m_rigidbodyMap) {

		if (pair.second != nullptr) {
			pair.second->Update();
		}
	}
}