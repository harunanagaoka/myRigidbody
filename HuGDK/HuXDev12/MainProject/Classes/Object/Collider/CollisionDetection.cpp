//
// CollisionDetection.h
//

#include "CollisionDetection.h"

bool CollisionDetection::IsCollide(Collider* colliderA, Collider* colliderB) {
	ColliderType typeA = colliderA->GetType();
	ColliderType typeB = colliderB->GetType();

	//ƒ^ƒCƒv‘‚¦‚½‚çswich-case‚É‰üC

	if (typeA == ColliderType::Circle && typeB == ColliderType::Circle)
	{
		auto* circleA = static_cast<CircleCollider*>(colliderA);
		auto* circleB = static_cast<CircleCollider*>(colliderB);

		return CheckCircleVSCircle(circleA, circleB);
	}

	return false;
}

bool CollisionDetection::CheckCircleVSCircle(CircleCollider* circleA, CircleCollider* circleB) {

	float r_A = circleA->GetRadius();
	SimpleMath::Vector2 center_A = circleA->GetCenter();

	float r_B = circleB->GetRadius();
	SimpleMath::Vector2 center_B = circleB->GetCenter();

	float distance = (center_B - center_A).LengthSquared();
	float radiusSum = r_A + r_B;
	float radiusSumSq = radiusSum * radiusSum;

	return distance <= radiusSumSq;

}