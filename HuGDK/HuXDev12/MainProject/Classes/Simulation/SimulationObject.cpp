//
// SimulationObject.cpp
//

#include "SimulationObject.h"

/// <summary>
/// �w�肵��ShapeName�ɉ�����GeometricPrimitive���쐬���܂��B
/// </summary>
/// <param name="shape">���݂�Box��Tetrahedron�̂ǂ��炩</param>
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
/// �����ڂƓ����蔻�藼���̑傫����ύX���܂��B
/// �����I�ɂ͕ʁX�ɐݒ�ł���悤�ύX�\��ł��B
/// </summary>
/// <param name="size">�����ځA�����蔻��̑傫��</param>
void SimulationObject::SetSize(SimpleMath::Vector3 size) {
	m_size = size;
	m_shape = GeometricPrimitive::CreateBox(size);
	m_rigidbody->GetCollider()->SetSize(size);
}

/// <summary>
/// �������Z�̌��ʂ��󂯎��A�����ڂɔ��f���܂��B
/// </summary>
void SimulationObject::Render(DirectXTK::BasicEffect& effect){
	if (!m_rigidbody) return;

	auto&& commandList = DXTK->CommandList;

	effect->SetWorld(m_rigidbody->GetWorldMatrix());
	effect->Apply(commandList);
	m_shape->Draw(commandList);
}

