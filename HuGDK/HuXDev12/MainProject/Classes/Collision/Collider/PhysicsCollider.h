//
// PhysicsCollider.h
//�R���C�_�[�̒��_�f�[�^����.

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include "..\..\Transform.h"
#include "..\..\Resistry\PhysicsTypes.h"

using namespace std;



class PhysicsCollider {
 public:
	 PhysicsCollider() = default;
	 PhysicsCollider(ColliderType type) :m_type(type) {
	 

	 }
	 virtual ~PhysicsCollider() = default;

	 /// @brief ���[�J�����_�����[���h���_�ɕϊ����܂��B
	 /// @warning Transform�̒l���ς������Ăяo�����ƁB
	 void UpdateWorldVertices(const SimpleMath::Matrix& worldMatrix) {
	 
		 m_worldVertices.resize(m_localVertices.size());

		 for (int i = 0; i < m_localVertices.size(); ++i) {

			 m_worldVertices[i] = SimpleMath::Vector3::Transform(m_localVertices[i], worldMatrix);
		 }
	 };

	 const vector<SimpleMath::Vector3> GetWorldVertices() const { return m_worldVertices; }

	 virtual SimpleMath::Vector3 ComputeCenter() const { return SimpleMath::Vector3::Zero; };
	 
protected:
	ColliderType m_type = ColliderType::Error;

	vector<SimpleMath::Vector3> m_localVertices;// ���[�J�����_ 

	vector<SimpleMath::Vector3> m_worldVertices;// ���[���h��Ԃ̒��_

};
