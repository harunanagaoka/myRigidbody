
#include "CollisionDetection.h"

/// <summary>
/// 2‚Â‚ÌRigidbody‚ğó‚¯æ‚èÕ“Ë”»’è‚ğs‚¢‚Ü‚·B
/// </summary>
/// <returns>Õ“Ëî•ñ</returns>
ContactInfo CollisionDetection::DetectCollision(const Rigidbody* rb_1, const Rigidbody* rb_2) {
	ContactInfo info;
	PhysicsCollider* collider_A = rb_1->GetCollider();
	PhysicsCollider* collider_B = rb_2->GetCollider();

	m_gjkSolver.GJK(collider_A, collider_B, info);
	return info;
}




