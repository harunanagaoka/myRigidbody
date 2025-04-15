
#include "BoxCollider.h"

BoxCollider::BoxCollider() {
	m_localVertices = m_primitiveVertices;

}

///@brief �R���C�_�[�̃T�C�Y����͂����l�ɕύX���܂��B
void BoxCollider::SetSize(const SimpleMath::Vector3& size) {

	m_localSize = size;

	for (int i = 0; i < m_localVertices.size(); i++)
	{
		m_localVertices[i] = m_primitiveVertices[i] * m_localSize;
	}
}

/// @brief ���[�J�����_�����[���h���_�ɕϊ����܂��B
/// @warning Transform�̒l���ς������Ăяo�����ƁB
void BoxCollider::UpdateWorldVertices(const SimpleMath::Matrix& worldMatrix) {

	for (int i = 0; i < m_localVertices.size(); ++i) {

		m_worldVertices[i] = SimpleMath::Vector3::Transform(m_localVertices[i], worldMatrix);
	}
}