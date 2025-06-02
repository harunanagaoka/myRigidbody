//
// EPASolver.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "Collider/PhysicsCollider.h"
#include "CollisionSupport.h"
#include <array>

using namespace std;
using namespace CollisionSupport;

class EPASolver {
//public:
//	ContactInfo EPA(const PhysicsCollider* collider_A, const PhysicsCollider* collider_B, array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, ContactInfo& info);
//
//	void ComputeBarycentric(FaceInfo minFace, SimpleMath::Vector3& contactPointA, SimpleMath::Vector3& contactPointB);
//
//	void ComputeTangentBasis(const SimpleMath::Vector3& normal, SimpleMath::Vector3& tangent1, SimpleMath::Vector3& tangent2);
//
//	void AddIfUniqueEdge(std::vector<std::pair<size_t, size_t>>& edges,
//		const std::vector<size_t>& faces, size_t a, size_t b);
//
//	size_t GetFaceNormals(const std::vector<SimpleMath::Vector3>& polytope,
//		const std::vector<size_t>& faces, vector<FaceInfo>& facesInfo);
//
//private:
//	vector<PointInfo> m_polytope;

};
