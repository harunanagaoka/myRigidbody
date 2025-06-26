
#include "BoxCollider.h"

/// <summary>
/// コライダーの中心点の座標を返します。
/// </summary>
SimpleMath::Vector3 BoxCollider::ComputeCenter() const {
	SimpleMath::Vector3 center = SimpleMath::Vector3::Zero;

	for (const auto& v : m_worldVertices) {
		center += v;
	}

	center /= static_cast<float>(m_worldVertices.size());

	return center;
}

/// <summary>
/// コライダーの大きさを変更します。
/// </summary>
void BoxCollider::SetSize(SimpleMath::Vector3 size) {
	m_size = size; 

	for (size_t i = 0; i < m_primitiveVertices.size(); ++i) {
		m_localVertices[i] = m_primitiveVertices[i] * size;
	}

	ComputeInertiaTensor();//回転テンソル変更

}

/// <summary>
/// 回転テンソルを形状に合わせて更新します。
/// </summary>
void BoxCollider::ComputeInertiaTensor() {
	const float w = m_size.x;
	const float h = m_size.y;
	const float d = m_size.z;

	const float factor = (1.0f / 12.0f) * m_mass;

	const float i_x = factor * (h * h + d * d); 
	const float i_y = factor * (w * w + d * d); 
	const float i_z = factor * (w * w + h * h); 

	m_inertiaTensorLocal = SimpleMath::Matrix::Identity;
	m_inertiaTensorLocal._11 = i_x;
	m_inertiaTensorLocal._22 = i_y;
	m_inertiaTensorLocal._33 = i_z;

}