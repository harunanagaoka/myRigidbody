//
// TetrahedronCollider.cpp
//

#include "TetrahedronCollider.h"

/*worldVertex�X�V��ɌĂԂ���*/
SimpleMath::Vector3 TetrahedronCollider::ComputeCenter() {

	
	SimpleMath::Vector3 center = SimpleMath::Vector3::Zero;

	for (const auto& v : m_worldVertices) {
		center += v;
	}

	center /= 4.0f;

	return center;
}