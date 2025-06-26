//
// EPASolver.h
//

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include "..\Collider/PhysicsCollider.h"
#include "CollisionSupport.h"
#include <array>
#include <unordered_set>

using namespace std;
using namespace CollisionSupport;

class EPASolver {
public:

	//面情報を保存する構造体
	struct FaceInfo {
		SimpleMath::Vector3 normal;//法線
		float distanceToOrigin;
		PointInfo pointA, pointB, pointC;//面の頂点
		FaceInfo* neighbors[3] = { nullptr,nullptr,nullptr };//隣接する面
		FaceInfo* prev = nullptr;
		FaceInfo* next = nullptr;
		unsigned char neighborEdges[3];//隣接面のどの辺に接しているか
		unsigned char visitedPass;
		unsigned char pass;
	};

	//面の数等を保存しておく構造体
	struct Horizon {
		FaceInfo* firstFace = nullptr;
		FaceInfo* currentFace = nullptr;
		unsigned int numberOfFaces = 0;
	};

	EPASolver() = default;
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

	EPASolver::FaceInfo* AddFace(PointInfo& pointA, PointInfo& pointB, PointInfo& pointC);

	void RemoveFace(FaceInfo* target);

	void InitNewFace(FaceInfo* newFace, PointInfo& pointA, PointInfo& pointB, PointInfo& pointC);

	EPASolver::FaceInfo* FindBest();

	PointInfo GetContactPoint(FaceInfo face);

private:

	//最後にerase
	vector<SimpleMath::Vector3> m_vertices_1;

	vector<SimpleMath::Vector3> m_vertices_2;

	vector<PointInfo> m_polytope;

	vector<FaceInfo*> m_FacesInfo;

	FaceInfo* m_Root = nullptr; 
	FaceInfo* m_Last = nullptr;

	const size_t m_EPAMaxIterations = 2000;
};
