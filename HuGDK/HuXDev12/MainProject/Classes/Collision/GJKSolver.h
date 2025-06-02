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

	bool ComputeClosestPoint(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, PointInfo& closestPointInfo);

	SimpleMath::Vector3 ClosestPointOnSegment(array<PointInfo, g_maxSimplexSize>& simplex, PointInfo& closestPointInfo);

	SimpleMath::Vector3 ClosestPointOnSegment(PointInfo& pointA, PointInfo& pointB, PointInfo& closestPointInfo);

	SimpleMath::Vector3 ClosestPointOnTriangle(array<PointInfo, g_maxSimplexSize>& simplex, PointInfo& closestPointInfo);

	SimpleMath::Vector3 ClosestPointOnTriangle(PointInfo& pointA, PointInfo& pointB, PointInfo& pointC, PointInfo& closestPointInfo);

	SimpleMath::Vector3 ClosestPointOnTetrahedron(array<PointInfo, g_maxSimplexSize>& simplex, PointInfo& closestPointInfo);

	bool HandleSimplex(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction);

	bool HandlePoint(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction);

	bool HandleTriangle(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction);

	bool HandleTetrahedron(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& d);

private:

	EPASolver m_epaSolver;
	float m_contactMargin = 0.15;
};