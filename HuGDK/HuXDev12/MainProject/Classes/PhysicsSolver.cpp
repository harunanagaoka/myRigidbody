#include "PhysicsSolver.h"

void PhysicsSolver::Update(Transform& transform) {
	//Update��RIgidbody�ɂ܂Ƃ߂�
	//if (m_isStatic)
	//{
	//	return;
	//}

}

/// <summary>
/// �d�͂��v�Z����position�ɓK�p���܂��B
/// </summary>
/// <param name="transform">�I�u�W�F�N�g����������Transform</param>
/// <param name="gravityScale">�d�͂̑傫��</param>
SimpleMath::Vector3 PhysicsSolver::ComputeGravity(const SimpleMath::Vector3& position, SimpleMath::Vector3& velocity, const float gravityScale) {

	SimpleMath::Vector3 nextPos = position + velocity * DXTK->Time.deltaTime;

	velocity.y -= m_gravityAccelConst * gravityScale * DXTK->Time.deltaTime;

	return nextPos;
	//transform.SetPosition(nextPos);

}