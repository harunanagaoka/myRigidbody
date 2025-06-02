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

}