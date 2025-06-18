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

	//�ʏ���ۑ�����\����
	struct FaceInfo {
		SimpleMath::Vector3 normal;//�@��
		float distanceToOrigin;
		PointInfo pointA, pointB, pointC;//�ʂ̒��_
		FaceInfo* neighbors[3] = { nullptr,nullptr,nullptr };//�אڂ����
		FaceInfo* prev = nullptr;
		FaceInfo* next = nullptr;
		unsigned char neighborEdges[3];//�אږʂ̂ǂ̕ӂɐڂ��Ă��邩
		unsigned char visitedPass;
		unsigned char pass;
	};

	//�ʂ̐�����ۑ����Ă����\����
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

	//�Ō��erase
	vector<SimpleMath::Vector3> m_vertices_1;

	vector<SimpleMath::Vector3> m_vertices_2;

	vector<PointInfo> m_polytope;

	vector<FaceInfo*> m_FacesInfo;

	FaceInfo* m_Root = nullptr; 
	FaceInfo* m_Last = nullptr;

	const size_t m_EPAMaxIterations = 2000;
};
