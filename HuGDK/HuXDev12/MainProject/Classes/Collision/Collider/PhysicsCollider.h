//
// PhysicsCollider.h
//�R���C�_�[�̒��_�f�[�^����.

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include "..\..\Transform.h"

using namespace std;

enum class ColliderType {/*���̂����N���X�ɕ�������*/
	Tetrahedron,
	Error
};

class PhysicsCollider {
 public:
	 PhysicsCollider(ColliderType type) :m_type(type) {}
	 virtual ~PhysicsCollider() = default;

	 /// @brief ���[�J�����_�����[���h���_�ɕϊ����܂��B
	 /// @warning Transform�̒l���ς������Ăяo�����ƁB
	 void UpdateWorldVertices(const SimpleMath::Matrix& worldMatrix) {
	 
		 for (int i = 0; i < m_localVertices.size(); ++i) {

			 m_worldVertices[i] = SimpleMath::Vector3::Transform(m_localVertices[i], worldMatrix);
		 }
	 };

	 vector<SimpleMath::Vector3> GetWorldVertices() { return m_worldVertices; }

	 virtual SimpleMath::Vector3 ComputeCenter() { return SimpleMath::Vector3::Zero; };
	 
protected:
	ColliderType m_type = ColliderType::Error;

	vector<SimpleMath::Vector3> m_localVertices;// ���[�J�����_ 

	vector<SimpleMath::Vector3> m_worldVertices;// ���[���h��Ԃ̒��_

};
