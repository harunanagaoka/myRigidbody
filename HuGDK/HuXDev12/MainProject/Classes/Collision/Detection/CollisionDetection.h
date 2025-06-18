//
// CollisionDetection.h
// 衝突検知アルゴリズム選定用クラス

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include "CollisionSupport.h"
#include "..\Collider/PhysicsCollider.h"
#include "..\..\Core\Rigidbody.h"
#include "GJKSolver.h"

using namespace std;
using namespace CollisionSupport;

class CollisionDetection {
public:

	ContactInfo DetectCollision(const Rigidbody* rb_1, const Rigidbody* rb_2);

private :
	GJKSolver m_gjkSolver;
};