
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

bool CollisionDetection::IsDuplication(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, SimpleMath::Vector3 newPoint) {

	if (simplex.size() == 0)//���false�ɂȂ�̂łȂ���
	{
		return false;
	}

	for (int i = 0; i < simplex.size(); i++)
	{
		if ((simplex[i] - newPoint).LengthSquared() < 1e-6f)
		{
			return true;
		}
	}
	return false;
}

/// <summary>
/// �d�����Ă���false �o�^����Ȃ�
/// </summary>
bool CollisionDetection::CheckDuplicationAndSet(unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal>& container, SimpleMath::Vector3& value)
{
	auto result = container.insert(value);
	return result.second;
}

bool CollisionDetection::SimplexHitTest(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t index) 
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

		//SimpleMath::Vector3 A = simplex[1];
		//SimpleMath::Vector3 B = simplex[0];
		//SimpleMath::Vector3 AB = B - A;
		//SimpleMath::Vector3 AO = -A;

		//float ABdotAO = AB.Dot(AO);
		//float t = AB.Dot(AO) / AB.LengthSquared();
		//t = std::clamp(t, 0.0f, 1.0f);

		//SimpleMath::Vector3 closestPoint = A + AB * t;
		//float distance = closestPoint.Length();

		//if (distance < 1e-6f)
		//{
		//	return true;
		//}
		//else
		//{
		//	return false;
		//}

		//float t = ABdotAO / AB.LengthSquared();//(AB * AB�Ŋ���ƃG���[�ɂȂ邼�I�x�N�g���ł̊���Z�͑��݂��Ȃ����߁j
		//SimpleMath::Vector3 closestPoint;
		//if (0 < t && t < 1)
		//{
		//	closestPoint = A + AB * t;//A����X�^�[�g����AB*t�{�i�񂾂Ƃ���
		//	//	float test = closestPoint.Length();
		//}
		//else if (t <= 0)
		//{
		//	closestPoint = A;
		//}
		//else if (t >= 1)
		//{
		//	closestPoint = B;
		//}

		//if (0 <= ABdotAO && ABdotAO <= AB.LengthSquared() && closestPoint.Length() < 1e-6f)//���������_���ܕ�邩
		//{
		//	return true;
		//}
		//else
		//{
		//	return false;
		//}

	}

	if (index == 3)
	{
		SimpleMath::Vector3 A, B, C, AB, BC, CA,closestPointAB, closestPointBC, closestPointCA;
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
		closestPointAB = A + AB * clamp(t_AB, 0.0f,1.0f);
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

bool CollisionDetection::HandleTetrahedron(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& d) {
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


	//���_���܂܂�邩
	if (ABCNormal.Dot(AO) < 0)
	{
		simplex = { C, B, A };
		index = 3;
		d = -ABCNormal;
		d.Normalize();
		//m_polytope.erase(m_polytope.begin() + 0);
		return false;
	}

	if (ACDNormal.Dot(AO) < 0)
	{
		simplex = { D, C, A };
		index = 3;
		d = -ACDNormal;
		d.Normalize();
		//m_polytope.erase(m_polytope.begin() + 2);
		return false;
	}

	if (ADBNormal.Dot(AO) < 0)
	{
		simplex = { B, D, A };
		index = 3;
		d = -ADBNormal;
		d.Normalize();
		//m_polytope.erase(m_polytope.begin() + 1);
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

		SimpleMath::Vector3 normal = ab.Cross(ac);

		SimpleMath::Vector3 acperp = normal.Cross(ac);//�O�����@��ac.Cross(ab).Cross(ac)

		//acperp.Dot(ao) <= 0 ���� abperp.Dot(ao) <= 0�ł���ΎO�p�`�̓����Ɍ��_������

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
		if (normal.Dot(ao) < 0)//���_�����ɒT��
		{
			normal = -normal;
		}
		direction = normal;
		direction.Normalize();
		return true;
}

bool CollisionDetection::GJK(const BoxCollider* collider_1,const BoxCollider* collider_2, ContactInfo& info) {

	unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal> visitedPoints;//�d���`�F�b�N�p�̃R���e�i
	int maxDuplication = 5;
	int duplicationCount = 0;

	const array<SimpleMath::Vector3, 8> vertices_1 = collider_1->GetWorldVertices();
	const array<SimpleMath::Vector3, 8> vertices_2 = collider_2->GetWorldVertices();

	array<SimpleMath::Vector3, g_maxSimplexSize> simplex;
	SimpleMath::Vector3 ao,bo, ab, ac, a, b, c, abperp, acperp;

	size_t index = 0;//���݂�simplex�̒��_��

	//�R���C�_�[�̒��S�_
	SimpleMath::Vector3 center_1 = collider_1->ComputeCenter();
	SimpleMath::Vector3 center_2 = collider_2->ComputeCenter();

	//1�_��
	SimpleMath::Vector3 d = center_1 - center_2;
	

	if (d.LengthSquared() < 1e-6f)
	{
		d = SimpleMath::Vector3::UnitX;//���S�_���������̏���
	}
	d.Normalize();

	a = simplex[0] = Support(vertices_1, vertices_2, d);
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

	a = Support(vertices_1, vertices_2, d);

	CheckDuplicationAndSet(visitedPoints, a);
	simplex[index] = a;
	index++;

	if (a.Dot(d) < 0)
	{
		//���_�ɂ���ȏ�߂Â��Ȃ���GJK���I���������Ƃ̐ڐG�𔻒�
		//�����������̂łނ���O�p�`�ɂ��ē����ɂ��邩�𔻒�

			//AB��AO����镽�ʁi�܂��͒����j�́g���������h�ɕ⏕�_�����
			SimpleMath::Vector3 A = simplex[1];
			SimpleMath::Vector3 B = simplex[0];
			SimpleMath::Vector3 AB = B - A;
			SimpleMath::Vector3 AO = -A;
			SimpleMath::Vector3 normal = AB.Cross(AO);
			a = simplex[index] = Support(vertices_1, vertices_2, d);
			index++;
		if (CheckDuplicationAndSet(visitedPoints,a) == false)
		{
			//�މ��P�[�X
			return false;
		}

		if(SimplexHitTest(simplex,index) == true)
		{
			return true;
		}
		else
		{
			return false;
		}
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
	while (count++ < 50)
	{
		a = Support(vertices_1, vertices_2, d);

		if (a.Dot(d) < 0 )
		{
			if (SimplexHitTest(simplex, index) == true)
			{
				//�ڐG��EPA
				return true;
			}
			return false;
		}

		if (CheckDuplicationAndSet(visitedPoints,a) == false)
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
	while (count++ < 50)
	{
		a = Support(vertices_1, vertices_2, d);

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
			//info = EPA(collider_1, collider_2, simplex,info);
			return true;
		}
	}
	return false;
}

