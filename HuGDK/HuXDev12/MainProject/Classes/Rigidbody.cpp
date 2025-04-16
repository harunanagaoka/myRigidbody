#include "Rigidbody.h"

void Rigidbody::Update() {//FixedUpdate“à‚ÅŒÄ‚Ô—\’è

	ComputeGravity(DXTK->Time.deltaTime);

	SimpleMath::Vector3 nextPos = m_transform.GetPosition() + m_velocity;

	m_transform.SetPosition(nextPos);
}

void Rigidbody::ComputeGravity(float dt) {

	if (m_isGravity)
	{
		m_velocity.y -= m_gravityAccelConst * m_gravityScale * dt;
	}
}