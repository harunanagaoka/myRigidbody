#include "PhysicsSolver.h"

void PhysicsSolver::Update(Transform& transform) {
	//UpdateはRIgidbodyにまとめる
	//if (m_isStatic)
	//{
	//	return;
	//}

}

/// <summary>
/// 重力を計算してpositionに適用します。
/// </summary>
/// <param name="transform">オブジェクトが所持するTransform</param>
/// <param name="gravityScale">重力の大きさ</param>
SimpleMath::Vector3 PhysicsSolver::ComputeGravity(const SimpleMath::Vector3& position, SimpleMath::Vector3& velocity, const float gravityScale) {

	SimpleMath::Vector3 nextPos = position + velocity * DXTK->Time.deltaTime;

	velocity.y -= m_gravityAccelConst * gravityScale * DXTK->Time.deltaTime;

	return nextPos;
	//transform.SetPosition(nextPos);

}