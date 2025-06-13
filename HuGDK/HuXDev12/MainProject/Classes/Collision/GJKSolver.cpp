//
// GJKSolver.cpp
//

#include "GJKSolver.h"

bool GJKSolver::GJK(const PhysicsCollider* collider_A, const PhysicsCollider* collider_B, ContactInfo& info) {

	unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal> visitedPoints;//�d���`�F�b�N�p�̃R���e�i

	int gjkMaxIter = 1000;

	bool checkSimplex = false;
	bool isDegenerate = false;

	bool isContact = false;

	const vector<SimpleMath::Vector3> vertices_A = collider_A->GetWorldVertices();
	const vector<SimpleMath::Vector3> vertices_B = collider_B->GetWorldVertices();

	array<PointInfo, g_maxSimplexSize> simplex;//�V���v���b�N�X
	PointInfo a;
	SimpleMath::Vector3 ao, bo, ab, ac, b, c, abperp, acperp;

	size_t index = 0;//���݂�simplex�̒��_��

	//�R���C�_�[�̒��S�_
	SimpleMath::Vector3 center_A = collider_A->ComputeCenter();
	SimpleMath::Vector3 center_B = collider_B->ComputeCenter();

	//1�_��
	SimpleMath::Vector3 d = center_A - center_B;

	if (d.LengthSquared() < 1e-6f)
	{
		d = SimpleMath::Vector3::UnitX;//���S�_���������̏���
	}

	a = simplex[0] = Support(vertices_A, vertices_B, d);
	CheckDuplicationAndSet(visitedPoints, a.point);
	index++;

	//2�_��
	d = -a.point;

	for (int i = 0; i < gjkMaxIter; i++)
	{

		a = Support(vertices_A, vertices_B, d);

		if (a.point.Dot(d) < 0)
		{
			//�������Ă��Ȃ�
			break;
		}

		if (!CheckDuplicationAndSet(visitedPoints, a.point))
		{
			isDegenerate = true;
			break;
		}
		simplex[index] = a;
		index++;

		//�V���v���b�N�X�ƌ��_�̏Փ˔���
		//�������X�V
		
		if (HandleSimplex(simplex, index, d) == true)//�Փ�
		{
			isContact = true;
			break;
		}

		//�X�V��̕����̒����`�F�b�N
		if (d.LengthSquared() < 1e-6f)
		{
			break;
		}
	}


	//�ȉ�CheckConvergence()�@4�܂ň�ĂȂ���gjk���������Ă邩�`�F�b�N

	int count = 0;

	while (index < g_maxSimplexSize || !isDegenerate || count <= gjkMaxIter) {

		count++;

		float squaredDistance = numeric_limits<float>::max();

		//������x�T�|�[�g�_���擾�������𑪂�
		PointInfo checkPoint;
		checkPoint = Support(vertices_A, vertices_B, d);

		//delta = ���̒T�������ɑ΂��āA�V�����T�|�[�g�_���ǂꂭ�炢�O�i�ł������B
		float delta = checkPoint.point.Dot(d);

		// ���ς�0���傫���@���@�V�T�|�[�g�_���A�������_�ɓ͂��Ȃ����x���ŉ����ꍇ
		if (delta > 0 && delta * delta > squaredDistance)
		{
			break;
		}

		if (!CheckDuplicationAndSet(visitedPoints, checkPoint.point)) {
			isDegenerate = true;
			break;
		}
		
		float approach = squaredDistance - delta;//�������ǂꂭ�炢���P���ꂽ��
		float relativeEpsilon = squaredDistance * 1e-6f;//�����Ƃ݂Ȃ��Ă悢�A�ŏ����̕ω���

		if (approach <= relativeEpsilon)//�߂Â��Ȃ����� ���@�V���v���b�N�X�������ĂȂ������Ƃ�
		{
			isDegenerate = true;
			break;
		}

		//��ɂ��Ă͂܂�Ȃ�������V�����L���ȃT�|�[�g�_�Ȃ̂ŃV���v���b�N�X�ɒǉ��A�Őڋߓ_�����߂�
		simplex[index] = checkPoint;
		index++;

		//closest()�ōŐڋߓ_�����߂�
		PointInfo closestPointInfo;
		if (ComputeClosestPoint(simplex, index, closestPointInfo) == false) {
			break;
		}

		if (closestPointInfo.point.LengthSquared() < 1e-6f) {//�ڐG
			isContact = true;
			break;
		}

		//�ŒZ�x�N�g���T��
		float previousSquaredDistance = squaredDistance;
		squaredDistance = closestPointInfo.point.LengthSquared();

		if (previousSquaredDistance - squaredDistance <= 1e-6f * previousSquaredDistance) {//�ڐG
			isContact = true;
			break;
		}

		d = closestPointInfo.point;

	}

		PointInfo closestPointInfo;
		ComputeClosestPoint(simplex, index, closestPointInfo);

		float kakaka = closestPointInfo.point.LengthSquared();
		float sususus = closestPointInfo.point.Length();

		if (closestPointInfo.point.LengthSquared() <= 3)
		{

			if (index < 4) {

				bool Contact = TryEncloseOrigin(vertices_A, vertices_B, simplex, index);
				if (!Contact && !isContact) {
					return false;
				}

				ComputeClosestPoint(simplex, index, closestPointInfo);
			}



			SimpleMath::Vector3 normal = closestPointInfo.point;
			normal.Normalize();

			info.normal = normal;
			info.penetrationDepth = closestPointInfo.point.Length();
			info.contactPointA = closestPointInfo.supA;
			info.contactPointB = closestPointInfo.supB;
			GenerateFrictionBasis(info.normal, info.tangent1, info.tangent2);
			info.hasValue = true; 

			bool abc = m_epaSolver.EPA(collider_A, collider_B, simplex, index, info);


			if (info.penetrationDepth < 0.12) {
				return true;
			}
			
		}
		
	info.hasValue = false;
	return false;
}

bool GJKSolver::HandleSimplex(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction) {

	switch (index) {
	case 2:
		return HandlePoint(simplex, index, direction);
		break;
	case 3:
		return HandleTriangle(simplex, index, direction);
		break;
	case 4:
		return HandleTetrahedron(simplex, index, direction);
		break;
	default:
		break;
	}

	return false;
}

/// <summary>
/// 
/// </summary>
/// <param name="simplex"></param>
/// <param name="index"></param>
/// <param name="closestPointInfo"></param>
/// <returns>false index�̒l�Ɉُ킪����ꍇ</returns>
bool GJKSolver::ComputeClosestPoint(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, PointInfo& closestPointInfo)
{
	switch (index) {
	case 0:

		return false;
		
		break;
	case 1:

		closestPointInfo = simplex[0];

		break;
	case 2:

		ClosestPointOnSegment(simplex, closestPointInfo);

		break;
	case 3:

		ClosestPointOnTriangle(simplex, closestPointInfo);

		break;
	case 4:

		ClosestPointOnTetrahedron(simplex, closestPointInfo);

		break;
	default:
		return false;
		break;
	}

	return true;
}

SimpleMath::Vector3 GJKSolver::ClosestPointOnSegment(array<PointInfo, g_maxSimplexSize>& simplex, PointInfo& closestPointInfo) {
	SimpleMath::Vector3 A, B, AB, AO;
	A = simplex[1].point;
	B = simplex[0].point;
	AB = B - A;
	AO = -A;

	float abDotab = AB.Dot(AB);
	if (abDotab == 0) {

		closestPointInfo = simplex[1];
		return A;		//A == B
	}

	float t = AB.Dot(AO) / abDotab;
	t = std::clamp(t, 0.0f, 1.0f);//�����̒��ɐ�������

	closestPointInfo.point = A + t * AB;
	closestPointInfo.supA = simplex[1].supA * (1.0f - t) + simplex[0].supA * t;
	closestPointInfo.supB = simplex[1].supB * (1.0f - t) + simplex[0].supB * t;

	return closestPointInfo.point;
}

SimpleMath::Vector3 GJKSolver::ClosestPointOnSegment(PointInfo& pointA, PointInfo& pointB, PointInfo& closestPointInfo) {
	SimpleMath::Vector3 A, B, AB, AO;
	A = pointA.point;
	B = pointB.point;
	AB = B - A;
	AO = -A;

	float abDotab = AB.Dot(AB);
	if (abDotab == 0) {
		closestPointInfo = pointA;
		return A;		//A == B
	}

	float t = AB.Dot(AO) / abDotab;
	t = std::clamp(t, 0.0f, 1.0f);//�����̒��ɐ�������

	closestPointInfo.point = A + t * AB;
	closestPointInfo.supA = pointA.supA * (1.0f - t) + pointB.supA * t;
	closestPointInfo.supB = pointA.supB * (1.0f - t) + pointB.supB * t;

	return A + t * AB;

}

SimpleMath::Vector3 GJKSolver::ClosestPointOnTriangle(array<PointInfo, g_maxSimplexSize>& simplex, PointInfo& closestPointInfo) {
	//���_ (0, 0, 0) ����O�p�`���ʂɓ��e���āA���̓_���O�p�`���ɂ��邩�𔻒肷��
	//�O�p�`�O�Ȃ�A����AB,BC,CA�̂����ł��߂������̒��̈�_���v�Z���ĕԂ�
	PointInfo A, B, C;
	SimpleMath::Vector3 AB, AC, normal;
	A = simplex[2];
	B = simplex[1];
	C = simplex[0];

	AB = B.point - A.point;
	AC = C.point - A.point;
	normal = AB.Cross(AC);

	float area = normal.Length();
	if (area < 1e-6f)//�O�p�`���Ԃ�Ă���ꍇ
	{
		SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B,closestPointInfo);
		SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C, closestPointInfo);
		SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A, closestPointInfo);

		float dAB = pAB.LengthSquared();
		float dBC = pBC.LengthSquared();
		float dCA = pCA.LengthSquared();

		if (dAB <= dBC && dAB <= dCA) return pAB;
		if (dBC <= dCA) return pBC;
		return pCA;
	}

	normal.Normalize();

	float distance = A.point.Dot(normal);
	SimpleMath::Vector3 projected = -distance * normal;

	SimpleMath::Vector3 v0 = AB;
	SimpleMath::Vector3 v1 = AC;
	SimpleMath::Vector3 v2 = projected - A.point;

	float d00 = v0.Dot(v0);
	float d01 = v0.Dot(v1);
	float d11 = v1.Dot(v1);
	float d20 = v2.Dot(v0);
	float d21 = v2.Dot(v1);

	float denom = d00 * d11 - d01 * d01;
	if (denom == 0.0f)
	{
		closestPointInfo = simplex[2];
		return closestPointInfo.point;
	}
		

	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	// �O�p�`�������ǂ����`�F�b�N
	if (u >= 0 && v >= 0 && w >= 0)
	{
		closestPointInfo.point = u * A.point + v * B.point + w * C.point;

		closestPointInfo.supA =
			simplex[2].supA * u +
			simplex[1].supA * v +
			simplex[0].supA * w;

		closestPointInfo.supB =
			simplex[2].supB * u +
			simplex[1].supB * v +
			simplex[0].supB * w;

		return closestPointInfo.point;

	}

	// �O���Ȃ�ӏ�̍ŋߓ_��T��
	PointInfo closestAB, closestBC, closestCA;
	SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B, closestAB);
	SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C, closestBC);
	SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A, closestCA);

	float dAB = pAB.LengthSquared();
	float dBC = pBC.LengthSquared();
	float dCA = pCA.LengthSquared();

	if (dAB <= dBC && dAB <= dCA) {//���̃^�C�~���O�Ń[���ɂȂ��Ƃ�
		closestPointInfo = closestAB;
		return pAB;
	}
	if (dBC <= dCA) {
		closestPointInfo = closestBC;
		return pBC;
	}
	closestPointInfo = closestCA;
	return pCA;
}

SimpleMath::Vector3 GJKSolver::ClosestPointOnTriangle(PointInfo& pointA, PointInfo& pointB, PointInfo& pointC, PointInfo& closestPointInfo) {
	PointInfo A, B, C;
	SimpleMath::Vector3 AB, AC, normal;
	A = pointA;
	B = pointB;
	C = pointC;

	AB = B.point - A.point;
	AC = C.point - A.point;
	normal = AB.Cross(AC);

	float area = normal.Length();
	if (area < 1e-6f)//�O�p�`���Ԃ�Ă���ꍇ
	{
		PointInfo ABtmp, BCtmp, CAtmp;

		SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B, ABtmp);
		SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C, BCtmp);
		SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A, CAtmp);

		float dAB = pAB.LengthSquared();
		float dBC = pBC.LengthSquared();
		float dCA = pCA.LengthSquared();

		if (dAB <= dBC && dAB <= dCA) { 
			closestPointInfo = ABtmp;
			return pAB; }

		if (dBC <= dCA) { 
			closestPointInfo = BCtmp;
			return pBC; }

		closestPointInfo = CAtmp;
		return pCA;
	}

	normal.Normalize();

	float distance = A.point.Dot(normal);
	SimpleMath::Vector3 projected = -distance * normal;

	SimpleMath::Vector3 v0 = AB;
	SimpleMath::Vector3 v1 = AC;
	SimpleMath::Vector3 v2 = projected - A.point;

	float d00 = v0.Dot(v0);
	float d01 = v0.Dot(v1);
	float d11 = v1.Dot(v1);
	float d20 = v2.Dot(v0);
	float d21 = v2.Dot(v1);

	float denom = d00 * d11 - d01 * d01;
	if (denom == 0.0f)
	{
		closestPointInfo = A;
		return A.point;
	}
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	// �O�p�`�������ǂ����`�F�b�N
	if (u >= 0 && v >= 0 && w >= 0)
	{
		closestPointInfo.point = u * A.point + v * B.point + w * C.point;
		closestPointInfo.supA = u * A.supA + v * B.supA + w * C.supA;
		closestPointInfo.supB = u * A.supB + v * B.supB + w * C.supB;

		return u * A.point + v * B.point + w * C.point;
	}

	// �O���Ȃ�ӏ�̍ŋߓ_��T��
	PointInfo closestAB, closestBC, closestCA;
	SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B, closestAB);
	SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C, closestBC);
	SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A, closestCA);

	float dAB = pAB.LengthSquared();
	float dBC = pBC.LengthSquared();
	float dCA = pCA.LengthSquared();

	if (dAB <= dBC && dAB <= dCA) {
		closestPointInfo = closestAB;
		return pAB;
	}
	if (dBC <= dCA) {
		closestPointInfo = closestBC;
		return pBC;
	}
	closestPointInfo = closestCA;
	return pCA;
}

SimpleMath::Vector3 GJKSolver::ClosestPointOnTetrahedron(array<PointInfo, g_maxSimplexSize>& simplex, PointInfo& closestPointInfo) {
	PointInfo A, B, C, D,tmpABC, tmpACD, tmpABD, tmpBCD;
	A = simplex[3];
	B = simplex[2];
	C = simplex[1];
	D = simplex[0];

	// �l�ʑ̂�4�̖ʂ��\������O�p�`
	SimpleMath::Vector3 pABC = ClosestPointOnTriangle(A, B, C, tmpABC);
	SimpleMath::Vector3 pACD = ClosestPointOnTriangle(A, C, D, tmpACD);
	SimpleMath::Vector3 pABD = ClosestPointOnTriangle(A, B, D, tmpABD);
	SimpleMath::Vector3 pBCD = ClosestPointOnTriangle(B, C, D, tmpBCD);

	float dABC = pABC.LengthSquared();
	float dACD = pACD.LengthSquared();
	float dABD = pABD.LengthSquared();
	float dBCD = pBCD.LengthSquared();

	// ��ԋ߂����Ԃ�
	if (dABC <= dACD && dABC <= dABD && dABC <= dBCD) {
		closestPointInfo = tmpABC;
		return pABC;
	}
		
	if (dACD <= dABD && dACD <= dBCD) {
		closestPointInfo = tmpACD;
		return pACD;
	}

	if (dABD <= dBCD) {
		closestPointInfo = tmpABD;
		return pABD;
	}

	closestPointInfo = tmpBCD;
	return pBCD;
}

bool GJKSolver::SimplexHitTest(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index)
{
	if (index == 1)
	{
		return simplex[0] == SimpleMath::Vector3(0, 0, 0);
	}

	if (index == 2)
	{
		SimpleMath::Vector3 A = simplex[1];
		SimpleMath::Vector3 B = simplex[0];
		SimpleMath::Vector3 AB = B - A;
		SimpleMath::Vector3 AO = -A;

		float ABdotAO = AB.Dot(AO);
		float t = ABdotAO / AB.LengthSquared();
		t = clamp(t, 0.0f, 1.0f);
		SimpleMath::Vector3 closestPoint = A + AB * t;
		float distance = closestPoint.Length();

		if (0.0f <= t && t <= 1.0f && distance < 1e-6f)//���������_���ܕ�邩
		{
			return true;//���Ȃ�͂Ȃ�ĂĂ�true�ɂȂ����Ⴄ
		}
		else
		{
			return false;
		}

	}

	if (index == 3)
	{
		SimpleMath::Vector3 A, B, C, AB, BC, CA, closestPointAB, closestPointBC, closestPointCA;
		A = simplex[2];
		B = simplex[1];
		C = simplex[0];
		AB = B - A;
		BC = C - B;
		CA = A - C;

		//�����̂ǂ���ւ�Ɍ��_�����邩�Z�o
		float t_AB = -A.Dot(AB) / AB.LengthSquared();
		float t_BC = -B.Dot(BC) / BC.LengthSquared();
		float t_CA = -C.Dot(CA) / CA.LengthSquared();

		//�Őڋߓ_�����߂�
		// ���_�������̊O���ɂ���Ƃ��A�������Ƃ̋����ɂȂ��Ă��܂��̂ŁAClamp�Ő������Ă��܂��j
		closestPointAB = A + AB * clamp(t_AB, 0.0f, 1.0f);
		closestPointBC = B + BC * clamp(t_BC, 0.0f, 1.0f);
		closestPointCA = C + CA * clamp(t_CA, 0.0f, 1.0f);

		if (closestPointAB.Length() < 1e-6f || closestPointBC.Length() < 1e-6f || closestPointCA.Length() < 1e-6)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool GJKSolver::HandlePoint(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction) {
	SimpleMath::Vector3 A, B, AB, AO; 
	A = simplex[1].point;
	B = simplex[0].point;
	AB = B - A;
	AO = -A;

	// ���_��A�������ʒu�Ȃ�܂܂��Ƃ݂Ȃ�
	if (A.LengthSquared() < 1e-6f) return true;
	if (B.LengthSquared() < 1e-6f) return true;

	// ���_������AB��ɂ���Ȃ�AAO��AB�͓��������i�܂��͋t�����j�œ��ς����A���� t �� [0,1]
	float abLenSq = AB.LengthSquared();
	if (abLenSq < 1e-6f) return false; // AB���قڃ[�����Ȃ� false

	float t = AB.Dot(AO) / abLenSq;

	// ���_��A��B�̊Ԃɂ��邩�`�F�b�N
	if (t < 0.0f || t > 1.0f)
		return false;

	// ������̍ŋߓ_�Ƃ̋������قڃ[�����`�F�b�N
	SimpleMath::Vector3 closestPoint = A + t * AB;
	return closestPoint.LengthSquared() < 1e-6f;
}

bool GJKSolver::HandleTriangle(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction) {

	const SimpleMath::Vector3& a = simplex[2].point;
	const SimpleMath::Vector3& b = simplex[1].point;
	const SimpleMath::Vector3& c = simplex[0].point;

	SimpleMath::Vector3 ao = -a;//���_����
	SimpleMath::Vector3 ab = b - a;
	SimpleMath::Vector3 ac = c - a;

	SimpleMath::Vector3 normal = ab.Cross(ac);

	//���_�Ɗe���_����v����@���@�O�p�`�̏�Ɍ��_������
	if (ao == a || ao == b || ao == c) {
		

		if (normal.Dot(ao) < 0)//���_�����ɒT��
		{
			normal = -normal;
		}
		direction = normal;
		return true;
	}

	SimpleMath::Vector3 acperp = normal.Cross(ac);//�O�����@��ac.Cross(ab).Cross(ac)

	//acperp.Dot(ao) <= 0 ���� abperp.Dot(ao) <= 0�ł���ΎO�p�`�̓����Ɍ��_������

	float aiueo = acperp.Dot(ao);

	if (acperp.Dot(ao) > 0)//���_��AC�̊O�ɂ��邩�ǂ���
	{
		//�_C���̂čēx�O�p�`���쐬
		simplex[0] = simplex[1];
		simplex[1] = simplex[2];
		index = 2;
		direction = acperp;
		return false;
	}

	SimpleMath::Vector3 abperp = normal.Cross(ab);//ab.Cross(ac).Cross(ab)

	float kakiku = abperp.Dot(ao);

	if (abperp.Dot(ao) > 0)//���_��AB�̊O�ɂ��邩�ǂ���
	{
		//�_C���̂čēx�O�p�`���쐬
		simplex[0] = simplex[1];
		simplex[1] = simplex[2];
		index = 2;
		direction = abperp;
		return false;
	}

	//�����܂ŗ����猴�_���O�p�`�̓����ɂ��遨�l�ʑ̂ɓ_�𑝂₷

	/*SimpleMath::Vector3 normal = ab.Cross(ac);*///�ʂ̖@��
	direction = normal;
	return true;
}


bool GJKSolver::HandleTetrahedron(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& d) {
	// �l�ʑ̂̒��_
	const SimpleMath::Vector3& A = simplex[3].point;
	const SimpleMath::Vector3& B = simplex[2].point;
	const SimpleMath::Vector3& C = simplex[1].point;
	const SimpleMath::Vector3& D = simplex[0].point;

	// ���_�Ɍ������x�N�g��
	SimpleMath::Vector3 AO = -A;

	// �e�ʂ̖@��
	SimpleMath::Vector3 ABCNormal = (B - A).Cross(C - A);// ABC��
	SimpleMath::Vector3 ACDNormal = (C - A).Cross(D - A);// ACD��
	SimpleMath::Vector3 ADBNormal = (D - A).Cross(B - A);// ADB��

	// �@����������ɑ����� �E�E�E�E��n�����A�����̂ł́H
	if (ABCNormal.Dot(D - A) < 0)
	{
		ABCNormal = -ABCNormal;
	}
	if (ACDNormal.Dot(B - A) < 0)
	{
		ACDNormal = -ACDNormal;
	}
	if (ADBNormal.Dot(C - A) < 0)
	{
		ADBNormal = -ADBNormal;
	}

	ABCNormal.Normalize();
	ACDNormal.Normalize();
	ADBNormal.Normalize();

	//���_���܂܂�邩
	if (ABCNormal.Dot(AO) < 0)
	{
		simplex = { C, B, A };
		index = 3;
		d = -ABCNormal;
		//m_polytope.erase(m_polytope.begin() + 0);
		return false;
	}

	if (ACDNormal.Dot(AO) < 0)
	{
		simplex = { D, C, A };
		index = 3;
		d = -ACDNormal;
		//m_polytope.erase(m_polytope.begin() + 2);
		return false;
	}

	if (ADBNormal.Dot(AO) < 0)
	{
		simplex = { B, D, A };
		index = 3;
		d = -ADBNormal;
		//m_polytope.erase(m_polytope.begin() + 1);
		return false;
	}
	//�S�Ă̖ʂ̓������@�������_���� �� �l�ʑ̂����_���͂�ł���
	return true;
}

