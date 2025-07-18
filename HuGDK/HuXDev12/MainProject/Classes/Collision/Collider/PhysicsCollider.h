//
// PhysicsCollider.h
//コライダーの基底クラスです

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include "..\..\Core\Transform.h"
#include "..\..\Resistry\PhysicsTypes.h"

using namespace std;

class PhysicsCollider {
 public:
	 PhysicsCollider() = default;
	 PhysicsCollider(ColliderType type) :m_type(type){}
	 virtual ~PhysicsCollider() = default;
	 /// ローカル頂点をワールド頂点に変換します。
	 /// Transformの値が変わったら呼び出すこと。
	 void UpdateWorldVertices(const SimpleMath::Matrix& worldMatrix) {
	 
		 m_worldVertices.resize(m_localVertices.size());

		 for (int i = 0; i < m_localVertices.size(); ++i) {

			 m_worldVertices[i] = SimpleMath::Vector3::Transform(m_localVertices[i], worldMatrix);
		 }
	 };

	 /// <summary>
	 /// 回転が変わるたびに呼ぶこと
	 /// </summary>
	 /// <param name="rotation">Transformのrotation</param>
	 void UpdateInverseInertiaTensorWorld(const SimpleMath::Quaternion& rotation) {

		 SimpleMath::Matrix R = SimpleMath::Matrix::CreateFromQuaternion(rotation);

		 m_invInertiaTensorWorld = R * m_inertiaTensorLocal.Invert() * R.Transpose();
	 }

	 virtual SimpleMath::Vector3 ComputeCenter() const { return SimpleMath::Vector3::Zero; };
	 virtual void ComputeInertiaTensor() { m_inertiaTensorLocal = SimpleMath::Matrix::Identity; };

	 // Get
	 const vector<SimpleMath::Vector3> GetWorldVertices() const { return m_worldVertices; }
	 const SimpleMath::Matrix GetInverseInertiaTensorWorld() { return m_invInertiaTensorWorld; }

	 // Set
	 virtual void SetSize(SimpleMath::Vector3 size) { m_size = size; }

protected:
	ColliderType m_type = ColliderType::Error;
	
	SimpleMath::Vector3 m_size = SimpleMath::Vector3::One;//Boxの時に使用

	float m_mass = 1.0f;	

	vector<SimpleMath::Vector3> m_localVertices;// ローカル頂点 

	vector<SimpleMath::Vector3> m_worldVertices;// ワールド空間の頂点

	SimpleMath::Matrix m_inertiaTensorLocal;   // ローカル空間での慣性テンソル

	SimpleMath::Matrix m_invInertiaTensorWorld; // ワールド空間での慣性テンソルの逆行列(回転に使う）
};
