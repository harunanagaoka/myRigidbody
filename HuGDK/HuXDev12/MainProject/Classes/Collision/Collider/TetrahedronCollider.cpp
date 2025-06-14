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