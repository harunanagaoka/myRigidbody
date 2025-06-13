//
// PhysicsColliderFactory.h
//

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include "..\..\Resistry\PhysicsTypes.h"
#include "PhysicsCollider.h"
#include "TetrahedronCollider.h"
#include "BoxCollider.h"

class PhysicsColliderFactory final {
public:

	static PhysicsCollider* CreateCollider(const ColliderType type)
	{
		PhysicsCollider* collider = nullptr;
		switch (type) {
		case ColliderType::Tetrahedron: collider = new TetrahedronCollider(type); break;
		case ColliderType::Box: collider = new BoxCollider(type); break;
		}
		return collider;
	}

};