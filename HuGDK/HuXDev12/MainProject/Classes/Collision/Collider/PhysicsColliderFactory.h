//
// PhysicsColliderFactory.h
//

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include "..\..\Resistry\PhysicsTypes.h"
#include "PhysicsCollider.h"
#include "TetrahedronCollider.h"

class PhysicsColliderFactory final {
public:

	static PhysicsCollider* CreateCollider(const ColliderType type)
	{
		PhysicsCollider* collider = nullptr;
		switch (type) {
		case ColliderType::Tetrahedron: collider = new TetrahedronCollider(type); break;
		}
		return collider;
	}

};