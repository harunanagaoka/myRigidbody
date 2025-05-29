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

	bool ComputeClosestPoint(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& closest);

	SimpleMath::Vector3 ClosestPointOnSegment(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex);

	SimpleMath::Vector3 ClosestPointOnSegment(SimpleMath::Vector3 pointA, SimpleMath::Vector3 pointB);

	SimpleMath::Vector3 ClosestPointOnTriangle(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex);

	SimpleMath::Vector3 ClosestPointOnTriangle(SimpleMath::Vector3 pointA, SimpleMath::Vector3 pointB, SimpleMath::Vector3 pointC);

	SimpleMath::Vector3 ClosestPointOnTetrahedron(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex);

	bool HandleSimplex(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction);

	bool HandlePoint(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction);

	bool HandleTriangle(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction);

	bool HandleTetrahedron(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& d);

private:

	EPASolver m_epaSolver;
};