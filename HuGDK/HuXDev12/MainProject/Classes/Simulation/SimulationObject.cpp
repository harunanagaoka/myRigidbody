//
// SimulationObject.cpp
//

#include "SimulationObject.h"

/// <summary>
/// 指定したShapeNameに応じたGeometricPrimitiveを作成します。
/// </summary>
/// <param name="shape">現在はBoxかTetrahedronのどちらか</param>
void SimulationObject::SetShape(ShapeName shape) {
	switch (shape) {
	case ShapeName::Tetrahedron:

		m_shape = GeometricPrimitive::CreateTetrahedron();

		break;
	case ShapeName::Box:

		m_shape = GeometricPrimitive::CreateBox(m_size);

		break;
	default:
		break;
	}
}

/// <summary>
/// 見た目と当たり判定両方の大きさを変更します。
/// 将来的には別々に設定できるよう変更予定です。
/// </summary>
/// <param name="size">見た目、当たり判定の大きさ</param>
void SimulationObject::SetSize(SimpleMath::Vector3 size) {
	m_size = size;
	m_shape = GeometricPrimitive::CreateBox(size);
	m_rigidbody->GetCollider()->SetSize(size);
}

/// <summary>
/// 物理演算の結果を受け取り、見た目に反映します。
/// </summary>
void SimulationObject::Render(DirectXTK::BasicEffect& effect){
	if (!m_rigidbody) return;

	auto&& commandList = DXTK->CommandList;

	effect->SetWorld(m_rigidbody->GetWorldMatrix());
	effect->Apply(commandList);
	m_shape->Draw(commandList);
}

