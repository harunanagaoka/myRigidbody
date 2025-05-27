#include "PhysicsSolver.h"

void PhysicsSolver::Update(Transform& transform) {//FixedUpdate“à‚ÅŒÄ‚Ô—\’è

	if (m_isStatic)
	{
		return;
	}

	ComputeGravity(DXTK->Time.deltaTime);

	SimpleMath::Vector3 nextPos = transform.GetPosition() + m_velocity * DXTK->Time.deltaTime;

	transform.SetPosition(nextPos);
}

void PhysicsSolver::ComputeGravity(float dt) {

	if (m_isGravity)
	{
		m_velocity.y -= m_gravityAccelConst * m_gravityScale * dt;
	}
}