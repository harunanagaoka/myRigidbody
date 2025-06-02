//
// TetrahedronCollider.cpp
//

#include "TetrahedronCollider.h"

/*worldVertex更新後に呼ぶこと*/
SimpleMath::Vector3 TetrahedronCollider::ComputeCenter() const {

	
	SimpleMath::Vector3 center = SimpleMath::Vector3::Zero;

	for (const auto& v : m_worldVertices) {
		center += v;
	}

	center /= 4.0f;

	return center;
}

void TetrahedronCollider::ComputeInertiaTensor() {

	// 正四面体の慣性テンソル対角要素（原点中心、各軸対称）
	float inertia = m_mass * 1.6f; //（近似値）

	m_inertiaTensorLocal = SimpleMath::Matrix::Identity;
	m_inertiaTensorLocal._11 = inertia;
	m_inertiaTensorLocal._22 = inertia;
	m_inertiaTensorLocal._33 = inertia;

}