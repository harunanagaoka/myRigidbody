//
// GJKSolver.cpp
//

#include "GJKSolver.h"

bool GJKSolver::GJK(const PhysicsCollider* collider_A, const PhysicsCollider* collider_B, ContactInfo& info) {

	unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal> visitedPoints;//�d���`�F�b�N�p�̃R���e�i
	int maxDuplication = 1000;//ToDo:�Ȃ����\��
	int duplicationCount = 0;
	int gjkMaxIter = 1000;

	bool checkSimplex = false;

	const vector<SimpleMath::Vector3> vertices_A = collider_A->GetWorldVertices();
	const vector<SimpleMath::Vector3> vertices_B = collider_B->GetWorldVertices();

	array<SimpleMath::Vector3, g_maxSimplexSize> simplex;//�V���v���b�N�X

	SimpleMath::Vector3 ao, bo, ab, ac, a, b, c, abperp, acperp;

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
	CheckDuplicationAndSet(visitedPoints, a);
	index++;

	//if (a.Dot(d) < 0)
	//{
	//	return false;
	//}//1�_�ڂ͂Ȃ���l�����ǉ�

	//2�_��
	d = -a;

	//if (d.LengthSquared() < 1e-6f)
	//{
	//	//�ڐG��EPA�@1�_�ڂ����Ă�Ȃ�K��������L���Ȃ̂ŏ�����
	//	return true;
	//}

	for (int i = 0; i < gjkMaxIter; i++)
	{

		a = Support(vertices_A, vertices_B, d);

		if (a.Dot(d) < 0)
		{
			//�������Ă��Ȃ��@status = -1;
			break;
		}

		CheckDuplicationAndSet(visitedPoints, a);//�d�������ꍇ�̃��W�b�N�ǉ�
		simplex[index] = a;
		index++;

		//�V���v���b�N�X�ƌ��_�̏Փ˔���
		//�������X�V
		if (HandleSimplex(simplex, index, d) == true)//�Փ�
		{
			break;
		}
		else
		{//��Փ�
			break;
		}

		//�X�V��̕����̒����`�F�b�N
		if (d.LengthSquared() < 1e-6f)
		{
			//status = -1;
			break;
		}
	}


	//�ȉ�CheckConvergence()�@4�܂ň�ĂȂ���gjk���������Ă邩�`�F�b�N
	while (true) {

		float squaredDistance = numeric_limits<float>::max();

		//������x�T�|�[�g�_���擾�������𑪂�
		SimpleMath::Vector3 checkPoint;
		checkPoint = Support(vertices_A, vertices_B, d);

		//delta = ���̒T�������ɑ΂��āA�V�����T�|�[�g�_���ǂꂭ�炢�O�i�ł������B
		float delta = checkPoint.Dot(d);

		// ���ς�0���傫���@���@�V�T�|�[�g�_���A�������_�ɓ͂��Ȃ����x���ŉ����ꍇ
		if (delta > 0 && delta * delta > squaredDistance)//&& (delta * delta > squaredDistance * input.m_maximumDistanceSquared)
		{
			checkSimplex = true;
			break;
		}

		if (!CheckDuplicationAndSet(visitedPoints, checkPoint)) {//�d���`�F�b�N�@checkduplication�𗬗p
			checkSimplex = true;
			break;
		}
		
		float approach = squaredDistance - delta;//�u�������ǂꂭ�炢���P���ꂽ���v
		float relativeEpsilon = squaredDistance * 1e-6f;//�u�����Ƃ݂Ȃ��Ă悢�A�ŏ����̕ω����vREL_ERROR2�͒��������_�B

		if (approach <= relativeEpsilon)//�߂Â��Ȃ�������
		{
			if (approach < 1e-6f)
			{
				//		//���S�ɒ�؂����i�[���i���j���ɒ[�ȑމ��P�[�X�̔���
				//		m_degenerateSimplex = 2;
			}
			else
			{
				//		//�u�[������Ȃ����ǁA�i���������Ȃ��v
				//		m_degenerateSimplex = 11;
			}

			checkSimplex = true;
			break;
		}

		//��ɂ��Ă͂܂�Ȃ�������V�����L���ȃT�|�[�g�_�Ȃ̂ŃV���v���b�N�X�ɒǉ��A�Őڋߓ_�����߂�
		CheckDuplicationAndSet(visitedPoints, checkPoint);
		simplex[index] = checkPoint;
		index++;

		//closest()�ōŐڋߓ_�����߂�
		SimpleMath::Vector3 closestPoint;
		if (ComputeClosestPoint(simplex, index, closestPoint) == false) {
			//	m_degenerateSimplex = 3;
			checkSimplex = true;
			break;
		}

		if (closestPoint.LengthSquared() < 1e-6f) {
			//closestPoint�𕪗����Ƃ��ĕۑ�
			// m_degenerateSimplex = 6;
			checkSimplex = true;
			break;
		}

		//�ŒZ�x�N�g���T�����
		float previousSquaredDistance = squaredDistance;
		squaredDistance = closestPoint.LengthSquared();

		if (previousSquaredDistance - squaredDistance <= 1e-6f * previousSquaredDistance) {
			checkSimplex = true;
			//m_degenerateSimplex = 12;
			break;
		}

		d = closestPoint;

		//�������[�v�΍􂱂��ɓ����

	//if ���[�v��������傤�ɂȂ�����@�G���[�@break;
		bool maxIndex = (index == 4);

		if (maxIndex)
		{
			//m_degenerateSimplex = 13;//�V���v���b�N�X�������ς��ŒT���ł����ɏI������n�̑މ�
			break;//checkSimplex�͍s��Ȃ��B
		}
	}

	//�f�X�g���N�^�I�ȏ���
	if (checkSimplex)//checkSimplex
	{
		SimpleMath::Vector3 closest;
		ComputeClosestPoint(simplex, index, closest);

		float a = closest.LengthSquared();

		if (closest.LengthSquared() <= 0.1f)
		{
			return true;
		}
		
		/*���̃V���v���b�N�X�i1�`4�_�j��ŁA
		�@���_�ɍł��߂��_��T����
		�@���̂Ƃ��� A, B ���ꂼ��̍ŋߓ_���o��*/

		//���̎��_�ŏՓː[�x���ߎ��ł���
		//��]�g���N�����߂���

	}

	//�ȉ�EPA�����Ȃ�g���B
	return false;
}

bool GJKSolver::HandleSimplex(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction) {
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

bool GJKSolver::ComputeClosestPoint(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& closest)
{
	switch (index) {
	case 0:

		return false;
		
		break;
	case 1:

		closest = simplex[0];

		break;
	case 2:

		closest = ClosestPointOnSegment(simplex);

		break;
	case 3:

		closest = ClosestPointOnTriangle(simplex);

		break;
	case 4:

		closest = ClosestPointOnTetrahedron(simplex);

		break;
	default:
		return false;
		break;
	}

	return true;
}

SimpleMath::Vector3 GJKSolver::ClosestPointOnSegment(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex) {
	SimpleMath::Vector3 A, B, AB, AO;
	A = simplex[1];
	B = simplex[0];
	AB = B - A;
	AO = -A;

	float abDotab = AB.Dot(AB);
	if (abDotab == 0) {
		return A;		//A == B
	}

	float t = AB.Dot(AO) / abDotab;
	t = std::clamp(t, 0.0f, 1.0f);//�����̒��ɐ�������

	return A + t * AB;
}

SimpleMath::Vector3 GJKSolver::ClosestPointOnSegment(SimpleMath::Vector3 pointA, SimpleMath::Vector3 pointB) {
	SimpleMath::Vector3 A, B, AB, AO;
	A = pointA;
	B = pointB;
	AB = B - A;
	AO = -A;

	float abDotab = AB.Dot(AB);
	if (abDotab == 0) {
		return A;		//A == B
	}

	float t = AB.Dot(AO) / abDotab;
	t = std::clamp(t, 0.0f, 1.0f);//�����̒��ɐ�������

	return A + t * AB;

}

SimpleMath::Vector3 GJKSolver::ClosestPointOnTriangle(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex) {
	//���_ (0, 0, 0) ����O�p�`���ʂɓ��e���āA���̓_���O�p�`���ɂ��邩�𔻒肷��
	//�O�p�`�O�Ȃ�A����AB,BC,CA�̂����ł��߂������̒��̈�_���v�Z���ĕԂ�
	SimpleMath::Vector3 A, B, C, AB, AC,normal;
	A = simplex[2];
	B = simplex[1];
	C = simplex[0];

	AB = B - A;
	AC = C - A;
	normal = AB.Cross(AC);

	float area = normal.Length();
	if (area < 1e-6f)//�O�p�`���Ԃ�Ă���ꍇ
	{
		SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B);
		SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C);
		SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A);

		float dAB = pAB.LengthSquared();
		float dBC = pBC.LengthSquared();
		float dCA = pCA.LengthSquared();

		if (dAB <= dBC && dAB <= dCA) return pAB;
		if (dBC <= dCA) return pBC;
		return pCA;
	}

	normal.Normalize();

	float distance = A.Dot(normal);
	SimpleMath::Vector3 projected = -distance * normal;

	SimpleMath::Vector3 v0 = AB;
	SimpleMath::Vector3 v1 = AC;
	SimpleMath::Vector3 v2 = projected - A;

	float d00 = v0.Dot(v0);
	float d01 = v0.Dot(v1);
	float d11 = v1.Dot(v1);
	float d20 = v2.Dot(v0);
	float d21 = v2.Dot(v1);

	float denom = d00 * d11 - d01 * d01;
	if (denom == 0.0f)
		return A;

	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	// �O�p�`�������ǂ����`�F�b�N
	if (u >= 0 && v >= 0 && w >= 0)
	{
		return u * A + v * B + w * C;
	}

	// �O���Ȃ�ӏ�̍ŋߓ_��T��
	SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B);
	SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C);
	SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A);

	float dAB = pAB.LengthSquared();
	float dBC = pBC.LengthSquared();
	float dCA = pCA.LengthSquared();

	if (dAB <= dBC && dAB <= dCA) return pAB;
	if (dBC <= dCA) return pBC;
	return pCA;
}

SimpleMath::Vector3 GJKSolver::ClosestPointOnTriangle(SimpleMath::Vector3 pointA, SimpleMath::Vector3 pointB, SimpleMath::Vector3 pointC) {
	SimpleMath::Vector3 A, B, C, AB, AC, normal;
	A = pointA;
	B = pointB;
	C = pointC;

	AB = B - A;
	AC = C - A;
	normal = AB.Cross(AC);

	float area = normal.Length();
	if (area < 1e-6f)//�O�p�`���Ԃ�Ă���ꍇ
	{
		SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B);
		SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C);
		SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A);

		float dAB = pAB.LengthSquared();
		float dBC = pBC.LengthSquared();
		float dCA = pCA.LengthSquared();

		if (dAB <= dBC && dAB <= dCA) return pAB;
		if (dBC <= dCA) return pBC;
		return pCA;
	}

	normal.Normalize();

	float distance = A.Dot(normal);
	SimpleMath::Vector3 projected = -distance * normal;

	SimpleMath::Vector3 v0 = AB;
	SimpleMath::Vector3 v1 = AC;
	SimpleMath::Vector3 v2 = projected - A;

	float d00 = v0.Dot(v0);
	float d01 = v0.Dot(v1);
	float d11 = v1.Dot(v1);
	float d20 = v2.Dot(v0);
	float d21 = v2.Dot(v1);

	float denom = d00 * d11 - d01 * d01;
	if (denom == 0.0f)
		return A;

	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	// �O�p�`�������ǂ����`�F�b�N
	if (u >= 0 && v >= 0 && w >= 0)
	{
		return u * A + v * B + w * C;
	}

	// �O���Ȃ�ӏ�̍ŋߓ_��T���P��
	SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B);
	SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C);
	SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A);

	float dAB = pAB.LengthSquared();
	float dBC = pBC.LengthSquared();
	float dCA = pCA.LengthSquared();

	if (dAB <= dBC && dAB <= dCA) return pAB;
	if (dBC <= dCA) return pBC;
	return pCA;
}

SimpleMath::Vector3 GJKSolver::ClosestPointOnTetrahedron(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex) {
	SimpleMath::Vector3 A, B, C, D;
	A = simplex[3];
	B = simplex[2];
	C = simplex[1];
	D = simplex[0];

	// �l�ʑ̂�4�̖ʂ��\������O�p�`
	SimpleMath::Vector3 pABC = ClosestPointOnTriangle(A, B, C);
	SimpleMath::Vector3 pACD = ClosestPointOnTriangle(A, C, D);
	SimpleMath::Vector3 pABD = ClosestPointOnTriangle(A, B, D);
	SimpleMath::Vector3 pBCD = ClosestPointOnTriangle(B, C, D);

	float dABC = pABC.LengthSquared();
	float dACD = pACD.LengthSquared();
	float dABD = pABD.LengthSquared();
	float dBCD = pBCD.LengthSquared();

	// ��ԋ߂����Ԃ��P��
	if (dABC <= dACD && dABC <= dABD && dABC <= dBCD) return pABC;
	if (dACD <= dABD && dACD <= dBCD) return pACD;
	if (dABD <= dBCD) return pABD;
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

bool GJKSolver::HandlePoint(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction) {
	SimpleMath::Vector3 A, B, AB, AO; 
	A = simplex[1];
	B = simplex[0];
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

bool GJKSolver::HandleTriangle(std::array<SimpleMath::Vector3, 4>& simplex, size_t& index, SimpleMath::Vector3& direction) {

	const SimpleMath::Vector3& a = simplex[2];
	const SimpleMath::Vector3& b = simplex[1];
	const SimpleMath::Vector3& c = simplex[0];

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


bool GJKSolver::HandleTetrahedron(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& d) {
	// �l�ʑ̂̒��_
	const SimpleMath::Vector3& A = simplex[3];
	const SimpleMath::Vector3& B = simplex[2];
	const SimpleMath::Vector3& C = simplex[1];
	const SimpleMath::Vector3& D = simplex[0];

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

