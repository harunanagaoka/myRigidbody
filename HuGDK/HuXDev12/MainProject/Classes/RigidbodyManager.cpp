//
// RigidbodyManager.cpp
// 

#include "RigidbodyManager.h"

/// <summary>
/// 新しいRigidbodyを生成、管理リストに追加します。
/// </summary>
/// <param name="type">コライダーの形状</param>
/// <param name="position">描画、当たり判定のポジション</param>
/// <returns>生成したRigidbodyのポインタ</returns>
Rigidbody* RigidbodyManager::AddRigidbody(ColliderType type, SimpleMath::Vector3 position) {
	static int id = 0;
	
	PhysicsCollider* collider = m_colliderFactory.CreateCollider(type);

	Rigidbody* rb = new Rigidbody(id,type, collider,position);
	m_rigidbodyMap[id] = rb;
	id++;

	return rb;
}

/// <summary>
/// 全てのRigidbodyどうしの衝突検出を行います。
/// 衝突していた場合、衝突応答を行います。
/// </summary>
void RigidbodyManager::CheckCollision() {

	// 重複なし総当たり
	for (auto it1 = m_rigidbodyMap.begin(); it1 != m_rigidbodyMap.end(); ++it1) {
		auto it2 = it1;
		++it2; 

		for (; it2 != m_rigidbodyMap.end(); ++it2) {

			Rigidbody* rbA = it1->second;
			Rigidbody* rbB = it2->second;

			ContactInfo info = m_detector.DetectCollision(rbA, rbB);
			
			if (info.hasValue) {
				/* hasValue -> 衝突あり */
				rbA->ImpactResponse(rbB, info, true);
				rbB->ImpactResponse(rbA,info,false);
			}
		}
	}
}

/// <summary>
/// PhysicsUpdate、AddForceを呼び出します。
/// </summary>
void RigidbodyManager::Update() {

	PhysicsUpdate();

	HandleDemoInput();
}

/// <summary>
/// 全てのRigidbodyのUpdateと衝突判定を行います。
/// </summary>
void RigidbodyManager::PhysicsUpdate() {

	float dt = DXTK->Time.deltaTime / m_step;

	dt = std::min(dt, m_maxDeltaTime);

	for (int i = 0; i < m_step; ++i) {
		for (auto& pair : m_rigidbodyMap) {
			if (pair.second != nullptr) {

				pair.second->Update(dt); // 各剛体のUpdate()
			}
		}

		CheckCollision(); // 衝突判定
	}
}

/// <summary>
/// キー入力に応じてAddForceを呼びます。
/// </summary>
void RigidbodyManager::HandleDemoInput() {

	if (InputSystem.Keyboard.wasPressedThisFrame.A) {
		m_rigidbodyMap[m_boxIDDemo]->AddForce(m_boxJumpHeightDemo);
	}
}