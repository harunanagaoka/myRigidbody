//
// PhysicsCollider.h
//コライダーの頂点データ所持.

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include "..\..\Transform.h"

using namespace std;

enum class ColliderType {/*そのうちクラスに分離する*/
	Tetrahedron,
	Error
};

class PhysicsCollider {
 public:
	 PhysicsCollider(ColliderType type) :m_type(type) {}
	 virtual ~PhysicsCollider() = default;

	 /// @brief ローカル頂点をワールド頂点に変換します。
	 /// @warning Transformの値が変わったら呼び出すこと。
	 void UpdateWorldVertices(const SimpleMath::Matrix& worldMatrix) {
	 
		 for (int i = 0; i < m_localVertices.size(); ++i) {

			 m_worldVertices[i] = SimpleMath::Vector3::Transform(m_localVertices[i], worldMatrix);
		 }
	 };

	 vector<SimpleMath::Vector3> GetWorldVertices() { return m_worldVertices; }

	 virtual SimpleMath::Vector3 ComputeCenter() { return SimpleMath::Vector3::Zero; };
	 
protected:
	ColliderType m_type = ColliderType::Error;

	vector<SimpleMath::Vector3> m_localVertices;// ローカル頂点 

	vector<SimpleMath::Vector3> m_worldVertices;// ワールド空間の頂点

};
