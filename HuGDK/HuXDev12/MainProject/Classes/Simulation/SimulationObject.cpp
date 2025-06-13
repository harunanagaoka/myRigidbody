//
// SimulationObject.cpp
//

#include "SimulationObject.h"

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

void SimulationObject::SetSize(SimpleMath::Vector3 size) {
	m_size = size;
	m_shape = GeometricPrimitive::CreateBox(size);
	m_rigidbody->GetCollider()->SetSize(size);
}

void SimulationObject::Render(DirectXTK::BasicEffect& effect){
	if (!m_rigidbody) return;

	auto&& commandList = DXTK->CommandList;

	effect->SetWorld(m_rigidbody->GetWorldMatrix());
	effect->Apply(commandList);
	m_shape->Draw(commandList);
}

