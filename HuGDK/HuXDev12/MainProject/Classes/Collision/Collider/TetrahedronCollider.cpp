//
// TetrahedronCollider.cpp
//

#include "TetrahedronCollider.h"

/// <summary>
/// �R���C�_�[�̒��S�_�̍��W��Ԃ��܂��B
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
/// ��]�e���\�����`��ɍ��킹�čX�V���܂��B
/// </summary>
void TetrahedronCollider::ComputeInertiaTensor() {

	float inertia = m_mass * 1.6f; //�i�ߎ��l�j

	m_inertiaTensorLocal = SimpleMath::Matrix::Identity;
	m_inertiaTensorLocal._11 = inertia;
	m_inertiaTensorLocal._22 = inertia;
	m_inertiaTensorLocal._33 = inertia;

}