//
// SimulationObject.cpp
//

#include "SimulationObject.h"

void SimulationObject::Render(DirectXTK::BasicEffect& effect){
	if (!m_rigidbody) return;

	auto&& commandList = DXTK->CommandList;

	effect->SetWorld(m_rigidbody->GetWorldMatrix());
	effect->Apply(commandList);
	m_shape->Draw(commandList);

}