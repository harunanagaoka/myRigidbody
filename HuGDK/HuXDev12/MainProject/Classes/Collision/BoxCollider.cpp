
#include "BoxCollider.h"

/// @brief 引数を何も入力しない場合は1×1×1の大きさになります。
BoxCollider::BoxCollider() {

	m_localVertices = m_primitiveVertices;

}

BoxCollider::BoxCollider(SimpleMath::Vector3 size) {

	SetSize(size);
}

///@brief コライダーのサイズを入力した値に変更します。
void BoxCollider::SetSize(const SimpleMath::Vector3& size) {

	m_localSize = size;

	for (int i = 0; i < m_localVertices.size(); i++)
	{
		m_localVertices[i] = m_primitiveVertices[i] * m_localSize;
	}
}

/// @brief ローカル頂点をワールド頂点に変換します。
/// @warning Transformの値が変わったら呼び出すこと。
void BoxCollider::UpdateWorldVertices(const SimpleMath::Matrix& worldMatrix) {

	for (int i = 0; i < m_localVertices.size(); ++i) {

		m_worldVertices[i] = SimpleMath::Vector3::Transform(m_localVertices[i], worldMatrix);
	}
}

SimpleMath::Vector3 BoxCollider::ComputeCenter() const {
	SimpleMath::Vector3 sum = SimpleMath::Vector3::Zero;

	for (const auto& v : m_worldVertices) {
		sum += v;
	}

	if (sum == SimpleMath::Vector3::Zero)
	{
		return SimpleMath::Vector3::Zero;/*ゼロ徐算回避*/
	}

	return sum / 8.0f;
}
