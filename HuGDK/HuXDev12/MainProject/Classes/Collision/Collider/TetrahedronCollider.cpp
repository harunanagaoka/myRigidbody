//
// TetrahedronCollider.cpp
//

#include "TetrahedronCollider.h"

/// <summary>
/// コライダーの中心点の座標を返します。
/// </summary>
SimpleMath::Vector3 TetrahedronCollider::ComputeCenter() const {

	
	SimpleMath::Vector3 center = SimpleMath::Vector3::Zero;

	for (const auto& v : m_worldVertices) {
		center += v;
	}

	center /= 4.0f;

	return center;
}

/// <summary>
/// 回転テンソルを形状に合わせて更新します。
/// </summary>
void TetrahedronCollider::ComputeInertiaTensor() {

	float inertia = m_mass * 1.6f; //（近似値）

	m_inertiaTensorLocal = SimpleMath::Matrix::Identity;
	m_inertiaTensorLocal._11 = inertia;
	m_inertiaTensorLocal._22 = inertia;
	m_inertiaTensorLocal._33 = inertia;

}