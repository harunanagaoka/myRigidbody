//
//#include "EPASolver.h"
//
//ContactInfo EPASolver::EPA(const PhysicsCollider* collider_A, const PhysicsCollider* collider_B, array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, ContactInfo& info)
//{
//	const vector<SimpleMath::Vector3> vertices_1 = collider_A->GetWorldVertices();
//	const vector<SimpleMath::Vector3> vertices_2 = collider_B->GetWorldVertices();
//
//	//GJK����󂯎�����V���v���b�N�X��vector�Ɉڂ��i�|���g�[�v�j
//	vector<SimpleMath::Vector3> polytope(simplex.begin(), simplex.end());
//	vector<size_t> faces = { 0, 1, 2,
//							0, 3, 1,
//							0, 2, 3,
//							1, 3, 2 };//�l�ʑ�
//	vector<FaceInfo> facesInfo;
//
//	//���ׂĂ̎O�p�ʂ̖@���i+�����j���v�Z���A���_�ɍł��߂��ʂ�T���ĕԂ�
//	size_t minFace = GetFaceNormals(polytope, faces, facesInfo);//���_�Ɉ�ԋ߂��Ǝv����ʂ̖@���Ƌ���
//	//���߂�l��minFace�݂̂ɂ���
//
//	SimpleMath::Vector3 minNormal;
//	float minDistance = FLT_MAX;
//	SimpleMath::Vector3 support;
//
//	minNormal = SimpleMath::Vector3(facesInfo[minFace].normal);//�@���x�N�g��
//	minDistance = facesInfo[minFace].distanceToOrigin;//����
//
//	int count = 0;
//	while (count++ < 50) {//���C�����[�v
//
//		support = Support(vertices_1, vertices_2, facesInfo[minFace].normal);
//
//		//��avoidface�|���g�[�v�j��
//		std::vector<size_t> facesToRemove;
//		std::vector<std::pair<size_t, size_t>> uniqueEdges;
//
//		for (size_t i = 0; i < facesInfo.size(); i++) {
//			if (facesInfo[i].normal.Dot(support) > 0) {
//				size_t f = i * 3;
//
//				AddIfUniqueEdge(uniqueEdges, faces, f, f + 1);
//				AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
//				AddIfUniqueEdge(uniqueEdges, faces, f + 2, f);
//
//				facesToRemove.push_back(i);//�����\��̔ԍ�
//			}
//		}
//		std::sort(facesToRemove.rbegin(), facesToRemove.rend());//�d�������@��납�����
//		for (size_t index : facesToRemove) {
//			// FaceInfo�폜
//			facesInfo.erase(facesInfo.begin() + index);
//
//			// �Ή�����faces�C���f�b�N�X3���폜�iindex*3 ����3�A�j
//			size_t base = index * 3;
//			faces.erase(faces.begin() + base, faces.begin() + base + 3);
//		}
//		//��avoidface
//
//		polytope.push_back(support);//�T�|�[�g�_�ǉ�
//
//		//��addnewface
//		std::vector<FaceInfo> newFacesInfo;
//		for (auto [edgeIndex1, edgeIndex2] : uniqueEdges) {
//			FaceInfo newFaceInfo;
//			size_t supportIndex = polytope.size() - 1;
//
//			newFaceInfo.indexA = polytope[edgeIndex1];
//			newFaceInfo.indexB = polytope[edgeIndex2];
//			newFaceInfo.indexC = polytope[supportIndex];
//			newFaceInfo.Anum = edgeIndex1;
//			newFaceInfo.Bnum = edgeIndex2;
//			newFaceInfo.Cnum = supportIndex;
//			newFacesInfo.push_back(newFaceInfo);
//			faces.push_back(edgeIndex1);
//			faces.push_back(edgeIndex2);
//			faces.push_back(supportIndex);
//		}
//
//		float oldMinDistance = facesInfo[minFace].distanceToOrigin;
//		minFace = GetFaceNormals(polytope, faces, newFacesInfo);
//		//���_���V���v���b�N�X�Őڋߓ_�������߂�
//		//�T�|�[�g�֐��ł������ōł������_v��T��
//		//��addnewface
//
//		for (size_t i = 0; i < newFacesInfo.size(); i++) {
//			if (newFacesInfo[i].distanceToOrigin < oldMinDistance) {
//				minDistance = newFacesInfo[i].distanceToOrigin;
//				minFace = facesInfo.size() + i;
//			}
//		}
//		facesInfo.insert(facesInfo.end(), newFacesInfo.begin(), newFacesInfo.end());
//		//��addnewface
//		//��checkdistance
//		float sDistance = facesInfo[minFace].normal.Dot(support);
//
//		if (abs(sDistance - minDistance) <= 1e-6f) {//�I���`�F�b�N
//			//minDistance = FLT_MAX;
//			break;
//		}
//		//��checkdistance
//
//		minDistance = FLT_MAX;//���[�v�p��
//	}
//
//	//�f�X�g���N�^
//	SimpleMath::Vector3 tangent1, tangent2, contactPointA, contactPointB;
//	//minNormal�����˕Ԃ����
//	//tangent1�����C���̇@�i���Ƃ��Ή������X���b�v�j�������̊�����~�߂�
//	//tangent2�����C���̇A�i���Ƃ��ΑO��X���b�v�j�O������̊�����~�߂�
//
//	//���^���W�F���g�̌v�Z
//	ComputeTangentBasis(minNormal, tangent1, tangent2);
//
//
//	//�ڐG�_�̌v�Z
//	//��FaceInfo[minFace]��n��
//	ComputeBarycentric(facesInfo[minFace], contactPointA, contactPointB);
//
//	info.contactPointA = contactPointA;
//	info.contactPointB = contactPointB;
//	info.tangent1 = tangent1;
//	info.tangent2 = tangent2;
//	info.normal = minNormal;
//	info.penetrationDepth = facesInfo[minFace].distanceToOrigin;
//	//info.penetrationDepth = max(0.0f, facesInfo[minFace].distanceToOrigin - bias);
//	//�߂肱�ޏꍇ�͏�L�����Ă݂�
//
//	//����false��Ԃ��Ă邯�ǏՓˏ��̍\���̂�Ԃ��\��
//	return info;
//}
//
//void EPASolver::ComputeBarycentric(FaceInfo minFace, SimpleMath::Vector3& contactPointA, SimpleMath::Vector3& contactPointB)
//{
//	//���ߕ�
//	// �O�p�` A, B, C �ƁA�����̓_ P �ɑ΂��āF
//
//	PointInfo& pA = m_polytope[minFace.Anum];
//	PointInfo& pB = m_polytope[minFace.Bnum];
//	PointInfo& pC = m_polytope[minFace.Cnum];
//
//	SimpleMath::Vector3 A, B, C, P;
//	A = minFace.indexA;
//	B = minFace.indexB;
//	C = minFace.indexC;
//	P = SimpleMath::Vector3::Zero;//���_
//
//	SimpleMath::Vector3 v0 = B - A;
//	SimpleMath::Vector3 v1 = C - A;
//	SimpleMath::Vector3 v2 = P - A;
//
//	float d00 = v0.Dot(v0);
//	float d01 = v0.Dot(v1);
//	float d11 = v1.Dot(v1);
//	float d20 = v2.Dot(v0);
//	float d21 = v2.Dot(v1);
//
//	float denom = d00 * d11 - d01 * d01;
//	float v = (d11 * d20 - d01 * d21) / denom;
//	float w = (d00 * d21 - d01 * d20) / denom;
//	float u = 1.0f - v - w;
//	//�����W���������O�p�`�̍\���̎󂯎��v�Z
//	contactPointA = u * pA.supA + v * pB.supA + w * pC.supA;
//	contactPointB = u * pA.supB + v * pB.supB + w * pC.supB;
//}
//
//void EPASolver::ComputeTangentBasis(const SimpleMath::Vector3& normal, SimpleMath::Vector3& tangent1, SimpleMath::Vector3& tangent2)
//{
//	if (normal.x >= 0.57735f)//��(1 / 3) �̋ߎ��l
//	{
//		tangent1 = SimpleMath::Vector3(normal.y, -normal.x, 0.0f);
//	}
//	else
//	{
//		tangent1 = SimpleMath::Vector3(0.0f, normal.z, -normal.y);
//	}
//
//	tangent1.Normalize();
//	tangent2 = normal.Cross(tangent1);
//}
//
//void EPASolver::AddIfUniqueEdge(
//	std::vector<std::pair<size_t, size_t>>& edges,
//	const std::vector<size_t>& faces,
//	size_t a,
//	size_t b)
//{
//	auto reverse = std::find(                       //      0--<--3
//		edges.begin(),                              //     / \ B /   A: 2-0
//		edges.end(),                                //    / A \ /    B: 0-2
//		std::make_pair(faces[b], faces[a]) //   1-->--2
//	);
//
//	if (reverse != edges.end()) {
//		edges.erase(reverse);
//	}
//
//	else {
//		edges.emplace_back(faces[a], faces[b]);
//	}
//}
//
////bool SameDirection(SimpleMath::Vector4 normal, SimpleMath::Vector3 support) {
////	SimpleMath::Vector3 vec3normal = SimpleMath::Vector3(normal.x, normal.y, normal.z);
//
////	return vec3normal.Dot(support) > 0;
////}
//
//size_t EPASolver::GetFaceNormals(const std::vector<SimpleMath::Vector3>& polytope,
//	const std::vector<size_t>& faces, vector<FaceInfo>& facesInfo)
//{
//	std::vector<SimpleMath::Vector4> normals;
//	size_t minTriangle = 0;
//	float  minDistance = FLT_MAX;
//
//	for (size_t i = 0; i < faces.size(); i += 3) {
//
//		FaceInfo faceInfo;
//
//
//		SimpleMath::Vector3 a = faceInfo.indexA = polytope[faces[i]];
//		faceInfo.Anum = faces[i];
//
//		SimpleMath::Vector3 b = faceInfo.indexB = polytope[faces[i + 1]];
//		faceInfo.Bnum = faces[i + 1];
//
//		SimpleMath::Vector3 c = faceInfo.indexC = polytope[faces[i + 2]];
//		faceInfo.Cnum = faces[i + 2];
//		//������abc�̃C���f�b�N�X��ێ�����ivector�ŕۑ����邩�����Ƃ�j
//
//		SimpleMath::Vector3 ab = b - a;
//		SimpleMath::Vector3 ac = c - a;
//		SimpleMath::Vector3 normal = ab.Cross(ac);
//
//		// �����[�����ʐσ[���̎O�p�`
//		if (normal.LengthSquared() < 1e-6f) {
//			continue; // �� ����face�͖����Ƃ��ăX�L�b�v
//		}
//
//		normal.Normalize();
//
//		faceInfo.normal = normal;
//
//		float distance = normal.Dot(a);//���_�i0,0,0�j����@�������Ɍ�������
//
//		if (distance < 0) {
//			normal *= -1;
//			distance *= -1;
//		}
//		faceInfo.distanceToOrigin = distance;
//
//		normals.emplace_back(SimpleMath::Vector4(normal.x, normal.y, normal.z, distance));
//
//		if (distance < minDistance) {
//			minTriangle = i / 3;
//			minDistance = distance;
//		}
//
//		faceInfo.normal = normal;
//		facesInfo.push_back(faceInfo);
//	}
//
//	return minTriangle;
//}
//
