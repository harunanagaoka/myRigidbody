//
// GJKSolver.h
//

#pragma once
#include "CollisionSupport.h"
#include "Collider/PhysicsCollider.h"
#include "..\Rigidbody.h"
#include "..\Resistry\PhysicsTypes.h"
#include "EPASolver.h"
#include <array>
#include <unordered_set>

using namespace CollisionSupport;

class GJKSolver {
public:

	bool GJK(const PhysicsCollider* collider_A, const PhysicsCollider* collider_B, ContactInfo& info);

	bool SimplexHitTest(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index);

	bool HandleTriangle(std::array<SimpleMath::Vector3, 4>& simplex, size_t& index, SimpleMath::Vector3& direction);

	bool HandleTetrahedron(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& d);

private:

	EPASolver m_epaSolver;
};