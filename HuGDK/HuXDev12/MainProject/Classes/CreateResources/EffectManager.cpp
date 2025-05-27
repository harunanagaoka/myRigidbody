//
// EffectManager.cpp
//

#include "EffectManager.h"

void EffectManager::Initialize()
{

	DXGI_FORMAT colorFormat = DXTK->SwapChain.Format;
	DXGI_FORMAT depthFormat = DXTK->SwapChain.DepthFormat;
	RenderTargetState rtState(colorFormat, depthFormat);

	EffectPipelineStateDescription pd(
		&GeometricPrimitive::VertexType::InputLayout,
		CommonStates::Opaque,
		CommonStates::DepthDefault,
		CommonStates::CullNone,
		rtState,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
	);

	m_shaderEffect = DirectXTK::CreateBasicEffect(DXTK->Device, EffectFlags::Lighting, pd);
	m_shaderEffect->EnableDefaultLighting();

	m_shaderEffect->SetLightEnabled(0, true);
	m_shaderEffect->SetLightDiffuseColor(0, Colors::White);
	m_shaderEffect->SetLightDirection(0, DirectX::SimpleMath::Vector3::UnitZ);

}
