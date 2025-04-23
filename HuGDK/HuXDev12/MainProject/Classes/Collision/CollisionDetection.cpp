
#include "CollisionDetection.h"

size_t CollisionDetection::IndexOfFurthestPoint(const array<SimpleMath::Vector3, 8>& vertices,
	const SimpleMath::Vector3& direction) {
		
			float maxDot = -FLT_MAX;
			size_t index = 0;

			for (int i = 0; i < vertices.size(); i++)
			{
				float dot = vertices[i].Dot(direction);//direction��vertex�̓���

				if (dot > maxDot)//�ő�̓_�����߂�
				{
					maxDot = dot;
					index = i;
				}
			}

			return index;
}

//SimpleMath::Vector3 CollisionDetection::Support(const array<SimpleMath::Vector3, 8> vertices_1,
//	const array<SimpleMath::Vector3, 8> vertices_2,
//	const SimpleMath::Vector3& direction) {
//	size_t i = IndexOfFurthestPoint(vertices_1, direction);
//
//	size_t j = IndexOfFurthestPoint(vertices_2, -direction);
//
//	return vertices_1[i] - vertices_2[j];
//
//}

bool CollisionDetection::HandleTetrahedron(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& d) {
	// �l�ʑ̂̒��_ A, B, C, D �iA���ŐV��support�_�j
	const SimpleMath::Vector3& A = simplex[3];
	const SimpleMath::Vector3& B = simplex[2];
	const SimpleMath::Vector3& C = simplex[1];
	const SimpleMath::Vector3& D = simplex[0];

	// ���_�Ɍ������x�N�g��
	SimpleMath::Vector3 AO = -A;

	// �e�ʂ̖@�����v�Z�@
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


	//���_���܂܂�邩
	if (ABCNormal.Dot(AO) < 0)
	{
		simplex = { C, B, A };
		index = 3;
		d = -ABCNormal;
		m_polytope.erase(m_polytope.begin() + 0);//D�̏�������
		return false;
	}

	if (ACDNormal.Dot(AO) < 0)
	{
		simplex = { D, C, A };
		index = 3;
		d = -ACDNormal;
		m_polytope.erase(m_polytope.begin() + 2);
		return false;
	}

	if (ADBNormal.Dot(AO) < 0)
	{
		simplex = { B, D, A };
		index = 3;
		d = -ADBNormal;
		m_polytope.erase(m_polytope.begin() + 1);
		return false;
	}
	//�S�Ă̖ʂ̓������@�������_���� �� �l�ʑ̂����_���͂�ł���
	return true;

}

bool CollisionDetection::HandleTriangle(std::array<SimpleMath::Vector3, 4>& simplex, size_t& index, SimpleMath::Vector3& direction) {
	
		const SimpleMath::Vector3& a = simplex[2];
		const SimpleMath::Vector3& b = simplex[1];
		const SimpleMath::Vector3& c = simplex[0];

		SimpleMath::Vector3 ao = -a;//���_����
		SimpleMath::Vector3 ab = b - a;
		SimpleMath::Vector3 ac = c - a;

		SimpleMath::Vector3 acperp = ac.Cross(ab).Cross(ac);//�O�����@��

		//acperp.Dot(ao) <= 0 ���� abperp.Dot(ao) <= 0�ł���ΎO�p�`�̓����Ɍ��_������

		if (acperp.Dot(ao) > 0)//���_��AC�̊O�ɂ��邩�ǂ���
		{
			simplex[0] = simplex[1];
			simplex[1] = simplex[2];
			index = 2;
			direction = acperp;
			m_polytope.erase(m_polytope.begin());//C���̂Ă�
			return false;
		}

		SimpleMath::Vector3 abperp = ab.Cross(ac).Cross(ab);

		if (abperp.Dot(ao) > 0)//���_��AB�̊O�ɂ��邩�ǂ���
		{
			simplex[0] = simplex[1];
			simplex[1] = simplex[2];
			index = 2;
			m_polytope.erase(m_polytope.begin());//C���̂Ă�
			direction = abperp;
			return false;
		}

		//�����܂ŗ����猴�_���O�p�`�̓����ɂ��遨�l�ʑ̃t�F�[�Y��

		SimpleMath::Vector3 normal = ab.Cross(ac);//�ʂ̖@��
		if (normal.Dot(ao) < 0)
		{
			normal = -normal;
		}
		direction = normal;//���_�����ɒT������
		return true;
}

bool CollisionDetection::GJK(const BoxCollider& collider_1,const BoxCollider& collider_2) {
	const array<SimpleMath::Vector3, 8> vertices_1 = collider_1.GetWorldVertices();
	const array<SimpleMath::Vector3, 8> vertices_2 = collider_2.GetWorldVertices();

	array<SimpleMath::Vector3, g_maxSimplexSize> simplex;
	SimpleMath::Vector3 ao, ab, ac, a, b, c, abperp, acperp;

	size_t index = 0;//���݂�simplex�̒��_��

	//�R���C�_�[�̒��S�_�����߂�
	SimpleMath::Vector3 center_1 = collider_1.ComputeCenter();
	SimpleMath::Vector3 center_2 = collider_2.ComputeCenter();

	//index ��1�̎��@���V���v���b�N�X0�����̔���
	//center_2��center_1�̕��������߂�
	SimpleMath::Vector3 d = center_1 - center_2;

	if (d.LengthSquared() < 1e-6f)
	{
		d = SimpleMath::Vector3::UnitX;//���S�_�������Ȃ�ЂƂ܂�X�����猴�_������
	}

	//�ŏ��̃V���v���b�N�X�����߂�
	a = simplex[0] = Support(vertices_1, vertices_2, d);
	index++;

	if (a.Dot(d) < 0)
	{
		return false;//�ڐG�Ȃ�
	}

	d = -a;//���̒T�����������߂�

	//2�߂̃V���v���b�N�X�����߂�
	a = Support(vertices_1, vertices_2, d);

	if (a.Dot(d) < 0)
	{
		return false;
	}

	simplex[index++] = a;//2�_��

	d = ao = -a;//���_�����ւ̃x�N�g��

	//��������2�_�̎��̗�O����
	if (d.LengthSquared() < 1e-6f)
	{
		b = simplex[0];
		ab = b - a;
		d = ab.Cross(ao).Cross(ab);

		if (d.LengthSquared() < 1e-6f)
		{
			d = ab.Cross(SimpleMath::Vector3::UnitX);

			if (d.LengthSquared() < 1e-6f)
			{
				d = ab.Cross(SimpleMath::Vector3::UnitY);

				if (d.LengthSquared() < 1e-6f)
				{
					d = SimpleMath::Vector3::UnitZ;
				}
			}
		}
	}
	//�����܂ŗ�O����

	int count = 0;
	while (count++ < 50)
	{
		a = Support(vertices_1, vertices_2, d);

		if (a.Dot(d) < 0)
		{
			return false;
		}

		simplex[index++] = a;//3�_��

		if (HandleTriangle(simplex, index, d))
		{
			break;
		}

	}

	if (index <= 2)
	{
		return false;//HandleTriangle��true�ɂȂ�Ȃ�������
	}

	//�������[�v�h�~
	count = 0;
	while (count++ < 50)//�V���v���b�N�X��4�̎��̏���
	{
		a = Support(vertices_1, vertices_2, d);

		if (a.Dot(d) < 0)
		{
			return false;
		}

		simplex[index++] = a;//4�_�ڒǉ�

		if (HandleTetrahedron(simplex, index, d))
		{
			return true;
		}
	}
	return false;

}