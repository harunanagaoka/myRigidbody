//
// RigidbodyManager.cpp
// 

#include "RigidbodyManager.h"

/// <summary>
/// �V����Rigidbody�𐶐��A�Ǘ����X�g�ɒǉ����܂��B
/// </summary>
/// <param name="type">�R���C�_�[�̌`��</param>
/// <param name="position">�`��A�����蔻��̃|�W�V����</param>
/// <returns>��������Rigidbody�̃|�C���^</returns>
Rigidbody* RigidbodyManager::AddRigidbody(ColliderType type, SimpleMath::Vector3 position) {
	static int id = 0;
	
	PhysicsCollider* collider = m_colliderFactory.CreateCollider(type);

	Rigidbody* rb = new Rigidbody(id,type, collider,position);
	m_rigidbodyMap[id] = rb;
	id++;

	return rb;
}

/// <summary>
/// �S�Ă�Rigidbody�ǂ����̏Փˌ��o���s���܂��B
/// �Փ˂��Ă����ꍇ�A�Փˉ������s���܂��B
/// </summary>
void RigidbodyManager::CheckCollision() {

	// �d���Ȃ���������
	for (auto it1 = m_rigidbodyMap.begin(); it1 != m_rigidbodyMap.end(); ++it1) {
		auto it2 = it1;
		++it2; 

		for (; it2 != m_rigidbodyMap.end(); ++it2) {

			Rigidbody* rbA = it1->second;
			Rigidbody* rbB = it2->second;

			ContactInfo info = m_detector.DetectCollision(rbA, rbB);
			
			if (info.hasValue) {
				/* hasValue -> �Փ˂��� */
				rbA->ImpactResponse(rbB, info, true);
				rbB->ImpactResponse(rbA,info,false);
			}
		}
	}
}

/// <summary>
/// PhysicsUpdate�AAddForce���Ăяo���܂��B
/// </summary>
void RigidbodyManager::Update() {

	PhysicsUpdate();

	HandleDemoInput();
}

/// <summary>
/// �S�Ă�Rigidbody��Update�ƏՓ˔�����s���܂��B
/// </summary>
void RigidbodyManager::PhysicsUpdate() {

	float dt = DXTK->Time.deltaTime / m_step;

	dt = std::min(dt, m_maxDeltaTime);

	for (int i = 0; i < m_step; ++i) {
		for (auto& pair : m_rigidbodyMap) {
			if (pair.second != nullptr) {

				pair.second->Update(dt); // �e���̂�Update()
			}
		}

		CheckCollision(); // �Փ˔���
	}
}

/// <summary>
/// �L�[���͂ɉ�����AddForce���Ăт܂��B
/// </summary>
void RigidbodyManager::HandleDemoInput() {

	if (InputSystem.Keyboard.wasPressedThisFrame.A) {
		m_rigidbodyMap[m_boxIDDemo]->AddForce(m_boxJumpHeightDemo);
	}
}