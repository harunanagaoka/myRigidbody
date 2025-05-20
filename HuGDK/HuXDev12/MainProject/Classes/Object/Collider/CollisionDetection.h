//
// CollisionDetection.h
//

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include "Collider.h"
#include "CircleCollider.h"

class CollisionDetection {
public:
	bool IsCollide(Collider* colliderA,Collider* colliderB);
	bool CheckCircleVSCircle(CircleCollider* circleA, CircleCollider* circleB);

private:

};

