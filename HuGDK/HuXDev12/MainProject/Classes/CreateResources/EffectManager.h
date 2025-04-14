//
// EffectManager.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include <d3dcompiler.h>
using Microsoft::WRL::ComPtr;

class EffectManager {
public:
	void Initialize();
	DirectXTK::BasicEffect& Get() { return m_shaderEffect; }
private:
	DirectXTK::BasicEffect        m_shaderEffect;
};

