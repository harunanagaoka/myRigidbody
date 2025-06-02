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
	 PhysicsCollider(ColliderType type) :m_type(type){

	 }
	 virtual ~PhysicsCollider() = default;

	 /// ToDo:�������ł���x�Ăяo��
	 /// @brief ���[�J�����_�����[���h���_�ɕϊ����܂��B
	 /// @warning Transform�̒l���ς������Ăяo�����ƁB
	 void UpdateWorldVertices(const SimpleMath::Matrix& worldMatrix) {
	 
		 m_worldVertices.resize(m_localVertices.size());

		 for (int i = 0; i < m_localVertices.size(); ++i) {

			 m_worldVertices[i] = SimpleMath::Vector3::Transform(m_localVertices[i], worldMatrix);
		 }
	 };

	 const vector<SimpleMath::Vector3> GetWorldVertices() const { return m_worldVertices; }

	 const SimpleMath::Matrix GetInverseInertiaTensorWorld() { return m_invInertiaTensorWorld; }

	 virtual SimpleMath::Vector3 ComputeCenter() const { return SimpleMath::Vector3::Zero; };

	 virtual void ComputeInertiaTensor() { m_inertiaTensorLocal = SimpleMath::Matrix::Identity; };

	 /// <summary>
	 /// Transform �̉�]���ς�邽�тɌĂԂ���
	 /// </summary>
	 /// <param name="rotation">Transform��rotation</param>
	 void UpdateInverseInertiaTensorWorld(const SimpleMath::Quaternion& rotation) {

		 SimpleMath::Matrix R = SimpleMath::Matrix::CreateFromQuaternion(rotation);

		 m_invInertiaTensorWorld = R * m_inertiaTensorLocal.Invert() * R.Transpose();

	 }

protected:
	ColliderType m_type = ColliderType::Error;

	float m_mass = 1.0f;

	vector<SimpleMath::Vector3> m_localVertices;// ���[�J�����_ 

	vector<SimpleMath::Vector3> m_worldVertices;// ���[���h��Ԃ̒��_

	SimpleMath::Matrix m_inertiaTensorLocal;   // ���[�J����Ԃł̊����e���\��

	SimpleMath::Matrix m_invInertiaTensorWorld; // ���[���h��Ԃł̊����e���\���̋t�s��(��]�Ɏg���j

};
