#pragma once

#include<array>

#include "CollisionSupport.h"


namespace CollisionSupport {

	/// <summary>
	/// direction�����̃x�N�g���ň�ԉ������_�̃C���f�b�N�X��Ԃ��܂�.
	/// </summary>
	/// <param name="vertices">���_�f�[�^</param>
	/// <param name="direction">�T������</param>
	size_t IndexOfFurthestPoint(const vector<SimpleMath::Vector3>& vertices,
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

	/// <summary>
	/// 2�̐}�`�̃~���R�t�X�L���}�`�̂����T�������ň�ԉ����_��Ԃ��܂��B
	/// </summary>
	/// <param name="vertices_A">�}�`A�̒��_�f�[�^</param>
	/// <param name="vertices_B">�}�`B�̒��_�f�[�^</param>
	/// <param name="direction">�T������</param>
	PointInfo Support(const vector<SimpleMath::Vector3>& vertices_A, const vector<SimpleMath::Vector3>& vertices_B, SimpleMath::Vector3& direction) {
		PointInfo pointInfo;

		size_t i = IndexOfFurthestPoint(vertices_A, direction);
		pointInfo.supA = vertices_A[i];

		size_t j = IndexOfFurthestPoint(vertices_B, -direction);
		pointInfo.supB = vertices_B[j];

		pointInfo.point = pointInfo.supA - pointInfo.supB;
		//m_polytope.push_back(pointInfo);

		return pointInfo;
	}

	/// <summary>
	/// ���܂ŎZ�o�������_�ƐV���_value���d�����Ă��Ȃ����m�F����B�d�����Ă��Ȃ������璸�_���R���e�i�ɕۑ��B
	/// </summary>
	/// <param name="container">�d���`�F�b�N�Ɏg���R���e�i��p�ӂ��邱��</param>
	/// <param name="value">�V���_</param>
	bool CheckDuplicationAndSet(unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal>& container, SimpleMath::Vector3& value)
	{
		auto result = container.insert(value);
		return result.second;
	}

	bool TryEncloseOrigin(const vector<SimpleMath::Vector3>& vertices_A, const vector<SimpleMath::Vector3>& vertices_B, array<PointInfo, g_maxSimplexSize>& simplex, size_t& index) {
		SimpleMath::Vector3 UnitX = SimpleMath::Vector3::Right;
		SimpleMath::Vector3 UnitY = SimpleMath::Vector3::Up;
		SimpleMath::Vector3 UnitZ = SimpleMath::Vector3::Forward;

		std::array<DirectX::SimpleMath::Vector3, 3> axis{ UnitX,UnitY,UnitZ };

		SimpleMath::Vector3 A;
		SimpleMath::Vector3 B;
		SimpleMath::Vector3 C;
		SimpleMath::Vector3 D;

		switch (index) {
		case 1: {

			axis = { UnitX,UnitY,UnitZ };

			for (int i = 0; i < 3; i++) {

				simplex[index] = Support(vertices_A, vertices_B, axis[i]);
				index++;
			}

			if (TryEncloseOrigin(vertices_A, vertices_B, simplex, index)) {
				return true;
			}
			else {
				axis = { -UnitX,-UnitY,-UnitZ };
				index = 1;
				for (int i = 0; i < 3; i++) {

					simplex[index] = Support(vertices_A, vertices_B, axis[i]);
					index++;
				}

				if (TryEncloseOrigin(vertices_A, vertices_B, simplex, index)) {
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
					simplex[2] = Support(vertices_A, vertices_B, p);
					index = 3;
					if (TryEncloseOrigin(vertices_A, vertices_B, simplex, index)) {
						return true;
					}
					else {
						p = -p;
						simplex[2] = Support(vertices_A, vertices_B, p);
						index = 3;
						if (TryEncloseOrigin(vertices_A, vertices_B, simplex, index)) {
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

				simplex[3] = Support(vertices_A, vertices_B, normal);
				index = 4;
				if (TryEncloseOrigin(vertices_A, vertices_B, simplex, index)) {
					return true;
				}
				else {
					normal = -normal;
					simplex[3] = Support(vertices_A, vertices_B, normal);
					index = 4;
					if (TryEncloseOrigin(vertices_A, vertices_B, simplex, index)) {
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

			return IsEncloseOrigin(A, B, C, D);
			break;
		}
		}
		return false;
	}


	/// <summary>
	/// EPA�p
	/// </summary>
	bool IsValidTetrahedron(SimpleMath::Vector3 pointA, SimpleMath::Vector3 pointB, SimpleMath::Vector3 pointC, SimpleMath::Vector3 pointD) {
		const SimpleMath::Vector3 AB = pointB - pointA;
		const SimpleMath::Vector3 AC = pointC - pointA;
		const SimpleMath::Vector3 AD = pointD - pointA;

		//�l�ʑ̂̑̐�
		float volume = abs(AB.Dot(AC.Cross(AD))) / 6.0f;

		if (volume > 0)
		{
			const SimpleMath::Vector3 P = SimpleMath::Vector3::Zero;

			return true;
		}

		return false;
	}

	bool CollisionSupport::IsEncloseOrigin(SimpleMath::Vector3 pointA, SimpleMath::Vector3 pointB, SimpleMath::Vector3 pointC, SimpleMath::Vector3 pointD) {
		auto SignTest = [](const SimpleMath::Vector3& p1, const SimpleMath::Vector3& p2, const SimpleMath::Vector3& p3, const SimpleMath::Vector3& testPoint) {
			SimpleMath::Vector3 n = (p2 - p1).Cross(p3 - p1);
			return n.Dot(testPoint - p1) >= 0.0f;
			};

		bool s1 = SignTest(pointA, pointB, pointC, SimpleMath::Vector3::Zero);
		bool s2 = SignTest(pointA, pointC, pointD, SimpleMath::Vector3::Zero);
		bool s3 = SignTest(pointA, pointD, pointB, SimpleMath::Vector3::Zero);
		bool s4 = SignTest(pointB, pointD, pointC, SimpleMath::Vector3::Zero);

		return (s1 == s2) && (s2 == s3) && (s3 == s4);


		//SimpleMath::Vector3 nABC = (pointB - pointA).Cross(pointC - pointA);
		//SimpleMath::Vector3 nACD = (pointC - pointA).Cross(pointD - pointA);
		//SimpleMath::Vector3 nADB = (pointD - pointA).Cross(pointB - pointA);
		//SimpleMath::Vector3 nCBD = (pointB - pointC).Cross(pointD - pointC);

		//bool insideABC = nABC.Dot(-pointA) > 0;
		//bool insideACD = nACD.Dot(-pointA) > 0;
		//bool insideADB = nADB.Dot(-pointA) > 0;
		//bool insideCBD = nCBD.Dot(-pointC) > 0;

		//return insideABC && insideACD && insideADB && insideCBD;
	}

	void GenerateFrictionBasis(SimpleMath::Vector3 normal, SimpleMath::Vector3& tangent1, SimpleMath::Vector3& tangent2) {

		if (abs(normal.x) > abs(normal.z)) {
			// normal�Ɛ����ȃx�N�g�������ix���������傫���Ƃ��j
			tangent1 = SimpleMath::Vector3(-normal.y, normal.x, 0.0f);
		}
		else {
			// z���������傫���Ƃ�
			tangent1 = SimpleMath::Vector3(0.0f, -normal.z, normal.y);
		}

		tangent1.Normalize();

		// tangent2 �� normal �� tangent1 �̊O�ςō��
		tangent2 = normal.Cross(tangent1);
	}
}

