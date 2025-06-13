
#include "EPASolver.h"

/*edge 0 = A��B

edge 1 = B��C

edge 2 = C��A*/

/// <summary>
/// gjk���璸�_�����󂯎��R���C�_�[���m�̏Փː[�x���v�Z���AContactInfo�ɕۑ����܂��B
/// </summary>
bool EPASolver::EPA(const PhysicsCollider* collider_A, const PhysicsCollider* collider_B, array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, ContactInfo& info) {

    m_vertices_1 = collider_A->GetWorldVertices();
	m_vertices_2 = collider_B->GetWorldVertices();

	if (index > 0 && EncloseOrigin(simplex,index)) {

		m_polytope.clear();
		m_polytope.insert(m_polytope.end(), simplex.begin(), simplex.end());

		//���_�̏��Ԃ��E��n�ɐ�����
		const SimpleMath::Vector3& DA = m_polytope[3].point - m_polytope[0].point;
		const SimpleMath::Vector3& DB = m_polytope[1].point - m_polytope[3].point;
		const SimpleMath::Vector3& DC = m_polytope[2].point - m_polytope[3].point;

		if (DA.Dot(DB.Cross(DC)) < 0) {

			swap(m_polytope[0], m_polytope[1]);
		}


		InitPolytope();//MakePolytope�ƕ�����

		FaceInfo* best = FindBest();
		FaceInfo currentFace = *best;
		PointInfo nextPoint;
		unsigned int pass = 0;
		unsigned int iterations = 0;
		BindFaces(m_FacesInfo[0], 0, m_FacesInfo[1], 0);
		BindFaces(m_FacesInfo[0], 1, m_FacesInfo[2], 0);
		BindFaces(m_FacesInfo[0], 2, m_FacesInfo[3], 0);
		BindFaces(m_FacesInfo[1], 1, m_FacesInfo[3], 2);
		BindFaces(m_FacesInfo[1], 2, m_FacesInfo[2], 1);
		BindFaces(m_FacesInfo[2], 2, m_FacesInfo[3], 1);

		//EPA���C�����[�v
		for (int i = 0; i < m_EPAMaxIterations; i++) {

			Horizon horizon;
			bool success = true;
			best->pass = ++pass;
			nextPoint = Support(m_vertices_1, m_vertices_2, best->normal);
			float distance = best->normal.Dot(nextPoint.point) - best->distanceToOrigin;
			if (distance > 0.1) {

				//�|���g�[�v�g��
				for (int j = 0; (j < 3) && success; j++) {
					success = ExpandPolytope(pass,nextPoint,best->neighbors[j],best->neighborEdges[j],horizon);
					//if (!success) {
					//	break;
					//}
				}

				if (success && horizon.numberOfFaces >= 3) {
					BindFaces(horizon.currentFace,1, horizon.firstFace, 2);
					RemoveFace(best);
					best = FindBest();
					currentFace = *best;
				}
				else {
					//m_status = eStatus::Failed;
					//m_status = eStatus::InvalidHull;?������
					break;
				}
			}
			else {
				//m_status = eStatus::AccuraryReached;
				break;
			}
		}

		info.hasValue = true;
		info.normal = currentFace.normal;
		info.penetrationDepth = currentFace.distanceToOrigin;
		PointInfo contactPoint = GetContactPoint(currentFace);
		info.contactPointA = contactPoint.supA;
		info.contactPointB = contactPoint.supB;
		GenerateFrictionBasis(info.normal, info.tangent1, info.tangent2);

		for (FaceInfo* f : m_FacesInfo) {
			delete f;
		}
		m_FacesInfo.clear();

		return true;
	}

	return false;
}

/// <summary>
/// �V���v���b�N�X��4�_�ɖ����Ȃ��ꍇ�A4�_�ɑ��₵�ăV���v���b�N�X���މ����Ă��Ȃ����𔻒肵�܂��B
/// </summary>
/// <param name="simplex">gjk�œ���ꂽ�V���v���b�N�X</param>
/// <param name="index">�V���v���b�N�X�̓_�̐�</param>
bool EPASolver::EncloseOrigin(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index) {

	SimpleMath::Vector3 UnitX = SimpleMath::Vector3::Right;
	SimpleMath::Vector3 UnitY = SimpleMath::Vector3::Up;
	SimpleMath::Vector3 UnitZ = SimpleMath::Vector3::Forward;

	array <SimpleMath::Vector3, 3> axis;

	SimpleMath::Vector3 A;
	SimpleMath::Vector3 B;
	SimpleMath::Vector3 C;
	SimpleMath::Vector3 D;
	
	switch (index) {
		case 1: {

			axis = { UnitX,UnitY,UnitZ };

			for (int i = 0; i < 3; i++) {

				simplex[index] = Support(m_vertices_1, m_vertices_2, axis[i]);
				index++;
			}

			if (EncloseOrigin(simplex, index)) {
				return true;
			}
			else {
				array <SimpleMath::Vector3, 3> axis = { -UnitX,-UnitY,-UnitZ };
				index = 1;
				for (int i = 0; i < 3; i++) {

					simplex[index] = Support(m_vertices_1, m_vertices_2, axis[i]);
					index++;
				}

				if (EncloseOrigin(simplex, index)) {
					return true;
				}
				else {
					index = 1;
					for (int i = 0; i < 3; i++) {

						simplex[index] = {};
						index++;
					}

					index = 1;
					return false;
				}

			}

			break;
		}
		case 2: {
			axis = { UnitX,UnitY,UnitZ };
			//���������
			SimpleMath::Vector3 d = simplex[1].point - simplex[0].point;

			for (int i = 0; i < 3; i++) {
				SimpleMath::Vector3 p = d.Cross(axis[i]);

				if (p.LengthSquared() > 0)
				{
					simplex[2] = Support(m_vertices_1, m_vertices_2, p);
					index = 3;
					if (EncloseOrigin(simplex, index)) {
						return true;
					}
					else {
						p = -p;
						simplex[2] = Support(m_vertices_1, m_vertices_2, p);
						index = 3;
						if (EncloseOrigin(simplex, index)) {
							return true;
						}
					}
				}
			}

			return false;

			break;
		}
		case 3: {
			A = simplex[2].point;
			B = simplex[1].point;
			C = simplex[0].point;

			SimpleMath::Vector3 normal = (B - A).Cross(C - A);//�E��n�x�N�g��

			if (normal.LengthSquared() > 0) {

				simplex[3] = Support(m_vertices_1, m_vertices_2, normal);
				index = 4;
				if (EncloseOrigin(simplex, index)) {
					return true;
				}
				else {
					normal = -normal;
					simplex[3] = Support(m_vertices_1, m_vertices_2, normal);
					index = 4;
					if (EncloseOrigin(simplex, index)) {
						return true;
					}
				}
			}

			return false;
			break;
		}
		case 4: {
			A = simplex[3].point;
			B = simplex[2].point;
			C = simplex[1].point;
			D = simplex[0].point;

			const SimpleMath::Vector3 AB = B - A;
			const SimpleMath::Vector3 AC = C - A;
			const SimpleMath::Vector3 AD = D - A;

			//�l�ʑ̂̑̐�
			float volume = abs(AB.Dot(AC.Cross(AD))) / 6.0f;

			if (volume > 0)
			{
				const SimpleMath::Vector3 P = SimpleMath::Vector3::Zero;

				return true;
			}

			return false;
			break;
		}
	}
	return false;
}

bool EPASolver::ExpandPolytope(unsigned char pass, PointInfo nextPoint, FaceInfo* face, unsigned int Index, Horizon& horizon) {
	static const unsigned int i1m3[] = { 1, 2, 0 };
	static const unsigned int i2m3[] = { 2, 0, 1 };//�O�p�`�p�C���f�b�N�X
	
	if (face->pass != pass) {

		const unsigned int Index1 = i1m3[Index];

		if (face->normal.Dot(nextPoint.point) - face->distanceToOrigin < 0) {
			FaceInfo* newFace = AddFace(face->pointB, face->pointA, nextPoint);
			//FaceInfo* newFace = AddFace(face->pointA, nextPoint, face->pointB);?

			BindFaces(newFace, 0, face, Index);

			if (horizon.currentFace != nullptr) {
				BindFaces(horizon.currentFace, 1, newFace, 2);
			}
			else {
				horizon.firstFace = newFace;
			}

			horizon.currentFace = newFace;
			++horizon.numberOfFaces;

			//face->pass = pass;
			return true;
		}
		else {
			
			const unsigned int Index2 = i2m3[Index];
			face->pass = pass;
			if (ExpandPolytope(pass, nextPoint, face->neighbors[Index1], face->neighborEdges[Index1], horizon) &&
				ExpandPolytope(pass, nextPoint, face->neighbors[Index2], face->neighborEdges[Index2], horizon)) {
				
				
				return true;
			}
		}
		//m_bestFace�ɗאڂ���3�̖ʂɑ΂��ĐV�����_���猩����ʂ�����
		//���̋��E��V�����_�ƂȂ��č\�z
	}
	return false;
}


/// <summary>
/// �����|���g�[�v����ʏ������܂��B
/// </summary>
void EPASolver::InitPolytope() {

	vector<size_t> faces = {
	0, 1, 2,   // ��0
	1, 0, 3,   // ��1
	2, 1, 3,   // ��2
	0, 2, 3    // ��3
	};

	//vector<size_t> faces = {
	//0, 1, 2,
	//0, 1, 3,
	//1, 2, 3,
	//2, 0, 3
	//};


	for (int i = 0; i < faces.size(); i += 3) {
		FaceInfo* newface = new FaceInfo();

		newface->pointA = m_polytope[faces[i]];
		newface->pointB = m_polytope[faces[i + 1]];
		newface->pointC = m_polytope[faces[i + 2]];
		
		SimpleMath::Vector3 ab = newface->pointB.point - newface->pointA.point;
		SimpleMath::Vector3 ac = newface->pointC.point - newface->pointA.point;
		newface->normal = ab.Cross(ac);

		if (newface->normal.Dot(newface->pointA.point) < 0) {
			newface->normal = -newface->normal;
		}

		newface->normal.Normalize();
		newface->distanceToOrigin = newface->normal.Dot(newface->pointA.point);

		m_FacesInfo.push_back(newface);
	}
}

/// <summary>
/// faceA��faceB��edgeA��edgeB�łȂ����Ă��邱�Ƃ��L�^���܂��B
/// </summary>
void EPASolver::BindFaces(FaceInfo* faceA, unsigned char edgeA, FaceInfo* faceB, unsigned char edgeB) {
	faceA->neighbors[edgeA] = faceB;
	faceA->neighborEdges[edgeA] = edgeB;

	faceB->neighbors[edgeB] = faceA;
	faceB->neighborEdges[edgeB] = edgeA;
}

FaceInfo* EPASolver::AddFace(PointInfo& pointA, PointInfo& pointB, PointInfo& pointC) {

	FaceInfo* newFace = new FaceInfo();
	InitNewFace(newFace, pointA, pointB, pointC);
	
	newFace->prev = m_Last;
	newFace->next = nullptr;

	if (m_Last) {

		m_Last->next = newFace;
	}
	else {

		m_Root = newFace;
	}

	m_Last = newFace;

	return newFace;
}

//�V�����ʂ��O�p�`�ɂȂ��Ă邩�`�F�b�N���鏈���ǉ�
void EPASolver::InitNewFace(FaceInfo* newFace, PointInfo& pointA, PointInfo& pointB, PointInfo& pointC) {
	newFace->pointA = pointA;
	newFace->pointB = pointB;
	newFace->pointC = pointC;

	SimpleMath::Vector3 ab = newFace->pointB.point - newFace->pointA.point;
	SimpleMath::Vector3 ac = newFace->pointC.point - newFace->pointA.point;
	newFace->normal = ab.Cross(ac);
	newFace->normal.Normalize();

	if (newFace->normal.Dot(newFace->pointA.point) < 0) {
		newFace->normal = -newFace->normal;
	}

	newFace->distanceToOrigin = newFace->normal.Dot(newFace->pointA.point);


	m_FacesInfo.push_back(newFace);

}

void EPASolver::RemoveFace(FaceInfo* target) {
	auto it = std::remove(m_FacesInfo.begin(), m_FacesInfo.end(), target);
	if (it != m_FacesInfo.end()){
		m_FacesInfo.erase(it, m_FacesInfo.end());
	}
		
}

/// <summary>
/// �ł����_�ɋ߂��ʂ��Z�o���܂��B
/// </summary>
/// <returns>�ł����_�ɋ߂��ʂ̃|�C���^</returns>
FaceInfo* EPASolver::FindBest() {

	FaceInfo* bestf = nullptr;
	float bestd = FLT_MAX;
	for (auto& face : m_FacesInfo) {
		
		if (face->distanceToOrigin < bestd && face->normal.Dot(face->pointA.point) > 0) {
			bestd = face->distanceToOrigin;
			bestf = face;
		}

	}
	return bestf;
}

PointInfo EPASolver::GetContactPoint(FaceInfo face) {

	PointInfo contactPoint;

	const SimpleMath::Vector3& A = face.pointA.point;
	const SimpleMath::Vector3& B = face.pointB.point;
	const SimpleMath::Vector3& C = face.pointC.point;

	// ���_���O�p�`ABC�Ɏˉe�����o���Z���^�[�W�������߂�P��
	SimpleMath::Vector3 AB = B - A;
	SimpleMath::Vector3 AC = C - A;
	SimpleMath::Vector3 AP = -A; // ���_ - A

	float d00 = AB.Dot(AB);
	float d01 = AB.Dot(AC);
	float d11 = AC.Dot(AC);
	float d20 = AP.Dot(AB);
	float d21 = AP.Dot(AC);
	float denom = d00 * d11 - d01 * d01;

	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	// ���ꂼ��̌`��̕�ԓ_�����߂�
	contactPoint.supA = u * face.pointA.supA + v * face.pointB.supA + w * face.pointC.supA;
	contactPoint.supB = u * face.pointA.supB + v * face.pointB.supB + w * face.pointC.supB;

	// �ڐG�_
	contactPoint.point = (contactPoint.supA + contactPoint.supB) * 0.5f;

	return contactPoint;

}

