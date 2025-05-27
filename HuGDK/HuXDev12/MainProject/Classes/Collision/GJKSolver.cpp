//
// GJKSolver.cpp
//

#include "GJKSolver.h"

bool GJKSolver::GJK(const PhysicsCollider* collider_A, const PhysicsCollider* collider_B, ContactInfo& info) {

	unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal> visitedPoints;//�d���`�F�b�N�p�̃R���e�i
	int maxDuplication = 1000;//ToDo:�Ȃ����\��
	int duplicationCount = 0;

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
	d.Normalize();

	a = simplex[0] = Support(vertices_A, vertices_B, d);
	CheckDuplicationAndSet(visitedPoints, a);
	index++;

	if (a.Dot(d) < 0)
	{
		return false;
	}

	//2�_��
	d = -a;

	if (d.LengthSquared() < 1e-6f)
	{
		//�ڐG��EPA
		return true;
	}
	d.Normalize();

	a = Support(vertices_A, vertices_B, d);

	CheckDuplicationAndSet(visitedPoints, a);//�d�������ꍇ�̃��W�b�N�ǉ�
	simplex[index] = a;
	index++;

	if (a.Dot(d) < 0)
	{
		return false;
	}

	//3�_��
	d = ao = -a;

	if (d.LengthSquared() < 1e-6f)
	{
		if (SimplexHitTest(simplex, index) == true)
		{
			//�ڐG��EPA
			return true;
		}
		return false;
	}
	d.Normalize();

	int count = 0;
	duplicationCount = 0;
	while (count++ < 50)
	{
		a = Support(vertices_A, vertices_B, d);

		if (a.Dot(d) < 0)
		{
			if (SimplexHitTest(simplex, index) == true)
			{
				//�ڐG��EPA
				return true;
			}
			return false;
		}

		if (CheckDuplicationAndSet(visitedPoints, a) == false)
		{
			duplicationCount++;
			if (duplicationCount >= maxDuplication)
			{
				if (SimplexHitTest(simplex, index) == true)
				{
					//�ڐG��EPA
					return true;
				}
				return false;
			}

			//�T������d�̕ύX

			b = simplex[0];
			ab = b - simplex[1];
			ao = -simplex[1];
			d = ab.Cross(ao).Cross(ab);

			if (d.LengthSquared() < 1e-6f)
			{
				if (SimplexHitTest(simplex, index) == true)
				{
					//�ڐG��EPA
					return true;
				}
				return false;
			}

			d.Normalize();
			
			continue;
		}

		simplex[index] = a;//3�_��
		index++;

		if (HandleTriangle(simplex, index, d))
		{
			break;
		}
	}

	//4�_��
	count = 0;//�������[�v�h�~
	duplicationCount = 0;
	while (count++ < 50)
	{
		a = Support(vertices_A, vertices_B, d);

		if (a.Dot(d) < 0)
		{
			if (SimplexHitTest(simplex, index) == true)
			{
				//�ڐG��EPA
				return true;
			}
			return false;
		}

		if (CheckDuplicationAndSet(visitedPoints, a) == false)
		{
			duplicationCount++;
			if (duplicationCount >= maxDuplication)
			{
				if (SimplexHitTest(simplex, index) == true)
				{
					//�ڐG��EPA
					return true;
				}
				return false;
			}
			//�����ł͒T�������̕ύX���s��continue
			continue;
		}

		simplex[index] = a;
		index++;

		if (HandleTetrahedron(simplex, index, d))
		{
			info.hasValue = true;
			//info = m_epaSolver.EPA(collider_A, collider_B, simplex, info);
			return true;
		}
	}
	return false;
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


bool GJKSolver::HandleTriangle(std::array<SimpleMath::Vector3, 4>& simplex, size_t& index, SimpleMath::Vector3& direction) {

	const SimpleMath::Vector3& a = simplex[2];
	const SimpleMath::Vector3& b = simplex[1];
	const SimpleMath::Vector3& c = simplex[0];

	SimpleMath::Vector3 ao = -a;//���_����
	SimpleMath::Vector3 ab = b - a;
	SimpleMath::Vector3 ac = c - a;

	SimpleMath::Vector3 normal = ab.Cross(ac);
	normal.Normalize();

	//���_�Ɗe���_����v����@���@�O�p�`�̏�Ɍ��_������
	if (ao == a || ao == b || ao == c) {
		

		if (normal.Dot(ao) < 0)//���_�����ɒT��
		{
			normal = -normal;
		}
		direction = normal;
		direction.Normalize();
		return true;
	}

	SimpleMath::Vector3 acperp = normal.Cross(ac);//�O�����@��ac.Cross(ab).Cross(ac)
	normal.Normalize();

	//acperp.Dot(ao) <= 0 ���� abperp.Dot(ao) <= 0�ł���ΎO�p�`�̓����Ɍ��_������

	float aiueo = acperp.Dot(ao);

	if (acperp.Dot(ao) > 0)//���_��AC�̊O�ɂ��邩�ǂ���
	{
		//�_C���̂čēx�O�p�`���쐬
		simplex[0] = simplex[1];
		simplex[1] = simplex[2];
		index = 2;
		direction = acperp;
		direction.Normalize();
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
		direction.Normalize();
		return false;
	}

	//�����܂ŗ����猴�_���O�p�`�̓����ɂ��遨�l�ʑ̂ɓ_�𑝂₷

	/*SimpleMath::Vector3 normal = ab.Cross(ac);*///�ʂ̖@��
	direction = normal;
	direction.Normalize();
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

