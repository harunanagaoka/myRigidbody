//
// TetrahedronCollider.cpp
//

#include "TetrahedronCollider.h"

/*worldVertex�X�V��ɌĂԂ���*/
SimpleMath::Vector3 TetrahedronCollider::ComputeCenter() const {

	
	SimpleMath::Vector3 center = SimpleMath::Vector3::Zero;

	for (const auto& v : m_worldVertices) {
		center += v;
	}

	center /= 4.0f;

	return center;
}

void TetrahedronCollider::ComputeInertiaTensor() {

	// ���l�ʑ̂̊����e���\���Ίp�v�f�i���_���S�A�e���Ώ́j
	float inertia = m_mass * 1.6f; //�i�ߎ��l�j

	m_inertiaTensorLocal = SimpleMath::Matrix::Identity;
	m_inertiaTensorLocal._11 = inertia;
	m_inertiaTensorLocal._22 = inertia;
	m_inertiaTensorLocal._33 = inertia;

}