//
// GJKSolver.cpp
//

#include "GJKSolver.h"

/// <summary>
/// �T�|�[�g�֐���p����2�̃R���C�_�[�̒��_����~���R�t�X�L���}�`��̓_���擾���A���̓_���`������}�`�����_(0,0)���܂ނ��ǂ����ŏՓˌ��o���s���܂��B
/// </summary>
/// <param name="info">�Փˏ�������\����</param>
/// <returns>�Փ˂������ǂ���</returns>
bool GJKSolver::GJK(const PhysicsCollider* collider_A, const PhysicsCollider* collider_B, ContactInfo& info) {

	unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal> visitedPoints;//�d���`�F�b�N�p�̃R���e�i

	bool checkSimplex = false;
	bool isDegenerate = false;
	bool isContact = false;

	const vector<SimpleMath::Vector3> vertices_A = collider_A->GetWorldVertices();
	const vector<SimpleMath::Vector3> vertices_B = collider_B->GetWorldVertices();

	//�V���v���b�N�X�̒��_��ۑ�����z��B
	array<PointInfo, g_maxSimplexSize> simplex;

	PointInfo newPoint;
	SimpleMath::Vector3 ao, bo, ab, ac, b, c, abperp, acperp;

	size_t index = 0;//���݂�simplex�̒��_��

	//�R���C�_�[�̒��S�_
	SimpleMath::Vector3 center_A = collider_A->ComputeCenter();
	SimpleMath::Vector3 center_B = collider_B->ComputeCenter();

	//�T������
	SimpleMath::Vector3 d = center_A - center_B;

	if (d.LengthSquared() < 1e-6f)
	{
		d = SimpleMath::Vector3::UnitX;//���S�_����������X�����ɒT��
	}

	newPoint = simplex[0] = Support(vertices_A, vertices_B, d);
	CheckDuplicationAndSet(visitedPoints, newPoint.point);
	index++;

	//2�_�ڈȍ~�̒T��
	d = -newPoint.point;
	for (int i = 0; i < m_gjkMaxIter; i++)
	{

		newPoint = Support(vertices_A, vertices_B, d);

		if (newPoint.point.Dot(d) < 0)
		{
			//�������Ă��Ȃ�
			break;
		}

		if (!CheckDuplicationAndSet(visitedPoints, newPoint.point))
		{
			isDegenerate = true;
			break;
		}
		simplex[index] = newPoint;
		index++;
		
		if (HandleSimplex(simplex, index, d) == true)//�Փ�
		{
			isContact = true;
			break;
		}

		//�X�V��̕����̒����`�F�b�N
		if (d.LengthSquared() < 1e-6f)
		{
			isContact = true;
			break;
		}
	}

	//��������V���v���b�N�X����ĂȂ���gjk���������Ă��邩�ǂ����␳�m�ȍŐڋߓ_�A�Փˏ��`�F�b�N���Ă��܂��B

	int count = 0;

	while (index < g_maxSimplexSize || !isDegenerate || count <= m_gjkMaxIter || !isContact) {

		count++;

		float squaredDistance = numeric_limits<float>::max();

		//������x�T�|�[�g�_���擾�������𑪂�
		PointInfo checkPoint;
		checkPoint = Support(vertices_A, vertices_B, d);

		//delta = ���̒T�������ɑ΂��āA�V�����T�|�[�g�_���ǂꂭ�炢�O�i�ł������B
		float delta = checkPoint.point.Dot(d);

		if (!CheckDuplicationAndSet(visitedPoints, checkPoint.point)) {//�d���`�F�b�N
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

		//�����܂ŗ�����L���ȃT�|�[�g�_�Ȃ̂ŃV���v���b�N�X�ɒǉ����A�Őڋߓ_�����߂�
		simplex[index] = checkPoint;
		index++;

		//�Őڋߓ_�����߂�
		PointInfo closestPointInfo;
		if (ComputeClosestPoint(simplex, index, closestPointInfo) == false) {//index�̒l�Ɉُ킪����
			return false;
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

		//�����[�v�̒T������
		d = closestPointInfo.point;
	}

	PointInfo closestPointInfo;
	ComputeClosestPoint(simplex, index, closestPointInfo);

	if (closestPointInfo.point.LengthSquared() <= m_closestPointEpsilon)
	{

		if (index < g_maxSimplexSize) {//�V���v���b�N�X��4�_�܂ň���Ă��Ȃ������ꍇ�̏���

			bool Contact = TryEncloseOrigin(vertices_A, vertices_B, simplex, index);
			if (!Contact && !isContact) {
				return false;
			}

			ComputeClosestPoint(simplex, index, closestPointInfo);
		}

		info.hasValue = true; 

		//���݂͕K��EPA�ɂȂ��Ă��܂����A�މ���l�̃p�^�[���ɂ�菈���𕪊򂳂���\��ł��B
		bool calculateNormal = m_epaSolver.EPA(collider_A, collider_B, simplex, index, info);

		if (info.penetrationDepth < m_penetrationEpsilon || calculateNormal) {
			return true;
		}
	}
		
	info.hasValue = false;
	return false;
}

/// <summary>
/// �V���v���b�N�X�����_���܂ނ��ǂ����𔻒肵�܂��B
/// </summary>
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
/// �V���v���b�N�X��2�_�̏ꍇ���_���܂ނ��ǂ����𔻒肵�܂��B
/// </summary>
bool GJKSolver::HandlePoint(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction) {
	SimpleMath::Vector3 A, B, AB, AO;
	A = simplex[1].point;
	B = simplex[0].point;
	AB = B - A;
	AO = -A;

	// ���_��A���قړ����ʒu�̏ꍇ
	if (A.LengthSquared() < 1e-6f) {return true;}
	if (B.LengthSquared() < 1e-6f) {return true;}

	// ���_������AB��ɂ���Ȃ�AAO��AB�͓��������i�܂��͋t�����j�œ��ς����A���� t �� [0,1]
	float abLenSq = AB.LengthSquared();
	if (abLenSq < 1e-6f) { return false; }

	float t = AB.Dot(AO) / abLenSq;

	// ���_��A��B�̊Ԃɂ��邩�`�F�b�N
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	// ������̍ŋߓ_�Ƃ̋������قڃ[�����`�F�b�N
	SimpleMath::Vector3 closestPoint = A + t * AB;

	return closestPoint.LengthSquared() < 1e-6f;
}

/// <summary>
/// �V���v���b�N�X��3�_�̏ꍇ���_���܂ނ��ǂ����𔻒肵�܂��B
/// </summary>
bool GJKSolver::HandleTriangle(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction) {

	const SimpleMath::Vector3& A = simplex[2].point;
	const SimpleMath::Vector3& B = simplex[1].point;
	const SimpleMath::Vector3& C = simplex[0].point;

	SimpleMath::Vector3 AO = -A;//���_����
	SimpleMath::Vector3 AB = B - A;
	SimpleMath::Vector3 AC = C - A;

	SimpleMath::Vector3 normal = AB.Cross(AC);

	//���_�Ɗe���_����v����@���@�O�p�`�̏�Ɍ��_������
	if (AO == A || AO == B || AO == C) {


		if (normal.Dot(AO) < 0)//���_�����ɒT��
		{
			normal = -normal;
		}
		direction = normal;
		return true;
	}

	//�O�����@��
	SimpleMath::Vector3 acperp = normal.Cross(AC);

	if (acperp.Dot(AO) > 0)//���_��AC�̊O�ɂ��邩�ǂ���
	{
		//�_C���̂čēx�O�p�`���쐬
		simplex[0] = simplex[1];
		simplex[1] = simplex[2];
		index = 2;
		direction = acperp;
		return false;
	}

	SimpleMath::Vector3 abperp = normal.Cross(AB);

	if (abperp.Dot(AO) > 0)//���_��AB�̊O�ɂ��邩�ǂ���
	{
		//�_C���̂čēx�O�p�`���쐬
		simplex[0] = simplex[1];
		simplex[1] = simplex[2];
		index = 2;
		direction = abperp;
		return false;
	}

	//�����܂ŗ����猴�_���O�p�`�̓����ɂ���
	direction = normal;
	return true;
}

/// <summary>
/// �V���v���b�N�X��4�_�̏ꍇ���_���܂ނ��ǂ����𔻒肵�܂��B
/// </summary>
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

	// �@����������ɑ�����
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
		return false;
	}

	if (ACDNormal.Dot(AO) < 0)
	{
		simplex = { D, C, A };
		index = 3;
		d = -ACDNormal;
		return false;
	}

	if (ADBNormal.Dot(AO) < 0)
	{
		simplex = { B, D, A };
		index = 3;
		d = -ADBNormal;
		return false;
	}
	//�S�Ă̖ʂ̓������@�������_���� �� �l�ʑ̂����_���͂�ł���
	return true;
}

/// <summary>
/// �Őڋߓ_���v�Z���܂��B
/// </summary>
/// <param name="closestPointInfo">�Őڋߓ_�̏�����͂��܂�</param>
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

/// <summary>
/// �V���v���b�N�X�����̎��̍Őڋߓ_���v�Z���܂��B
/// </summary>
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

/// <summary>
/// 2�_�̊Ԃōł����_�ɋ߂����W���v�Z���܂��B
/// </summary>
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

/// <summary>
/// �V���v���b�N�X���O�p�`�̎��̍Őڋߓ_���v�Z���܂��B
/// ���_ (0, 0, 0) ����O�p�`���ʂɓ��e���āA���̓_���O�p�`���ɂ��邩�𔻒肵�܂��B
/// �O�p�`�O�Ȃ�A����AB,BC,CA�̂����ł��߂������̒��̈�_���v�Z���ĕԂ��܂��B
/// </summary>
SimpleMath::Vector3 GJKSolver::ClosestPointOnTriangle(array<PointInfo, g_maxSimplexSize>& simplex, PointInfo& closestPointInfo) {
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

/// <summary>
/// 3�_�̊Ԃōł����_�ɋ߂����W���v�Z���܂��B
/// ���_ (0, 0, 0) ����O�p�`���ʂɓ��e���āA���̓_���O�p�`���ɂ��邩�𔻒肵�܂��B
/// �O�p�`�O�Ȃ�A����AB,BC,CA�̂����ł��߂������̒��̈�_���v�Z���ĕԂ��܂��B
/// </summary>
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

/// <summary>
/// �V���v���b�N�X���O�p�`�̎��̍Őڋߓ_���v�Z���܂��B
/// </summary>
SimpleMath::Vector3 GJKSolver::ClosestPointOnTetrahedron(array<PointInfo, g_maxSimplexSize>& simplex, PointInfo& closestPointInfo) {
	PointInfo A, B, C, D,tmpABC, tmpACD, tmpABD, tmpBCD;
	A = simplex[3];
	B = simplex[2];
	C = simplex[1];
	D = simplex[0];

	// �l�ʑ̂�4�̖ʂ��\������O�p�`���ꂼ��̍Őڋߓ_�����߂�
	SimpleMath::Vector3 pABC = ClosestPointOnTriangle(A, B, C, tmpABC);
	SimpleMath::Vector3 pACD = ClosestPointOnTriangle(A, C, D, tmpACD);
	SimpleMath::Vector3 pABD = ClosestPointOnTriangle(A, B, D, tmpABD);
	SimpleMath::Vector3 pBCD = ClosestPointOnTriangle(B, C, D, tmpBCD);

	float dABC = pABC.LengthSquared();
	float dACD = pACD.LengthSquared();
	float dABD = pABD.LengthSquared();
	float dBCD = pBCD.LengthSquared();

	// ��ԋ߂��l��Ԃ�
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
