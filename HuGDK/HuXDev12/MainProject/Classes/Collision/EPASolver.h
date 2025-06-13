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
public:

	~EPASolver() {
		for (FaceInfo* face : m_FacesInfo) {
			delete face;
		}
		m_FacesInfo.clear();

	}
	bool EPA(const PhysicsCollider* collider_A, const PhysicsCollider* collider_B, array<PointInfo, g_maxSimplexSize>& simplex,size_t& index, ContactInfo& info);

	bool EncloseOrigin(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index);

	bool ExpandPolytope(unsigned char pass, PointInfo nextPoint,FaceInfo* face, unsigned int Index,Horizon& horizon);

	void InitPolytope();

	void BindFaces(FaceInfo* faceA, unsigned char edgeA, FaceInfo* faceB, unsigned char edgeB);

	FaceInfo* AddFace(PointInfo& pointA, PointInfo& pointB, PointInfo& pointC);

	void RemoveFace(FaceInfo* target);

	void InitNewFace(FaceInfo* newFace, PointInfo& pointA, PointInfo& pointB, PointInfo& pointC);

	FaceInfo* FindBest();

	PointInfo GetContactPoint(FaceInfo face);

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
private:

	//ç≈å„Ç…erase
	vector<SimpleMath::Vector3> m_vertices_1;

	vector<SimpleMath::Vector3> m_vertices_2;

	vector<PointInfo> m_polytope;

	vector<FaceInfo*> m_FacesInfo;

	FaceInfo* m_Root = nullptr; 
	FaceInfo* m_Last = nullptr;

	const size_t m_EPAMaxIterations = 2000;
};
