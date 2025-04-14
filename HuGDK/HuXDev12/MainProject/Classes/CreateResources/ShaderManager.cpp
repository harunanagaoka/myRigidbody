#include "ShaderManager.h"

void ShaderManager::LoadShaders()
{
	m_result = D3DCompileFromFile(//頂点シェーダー
		m_vsPath.c_str() ,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS",
		"vs_5_0", 
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		m_vsBlob.ReleaseAndGetAddressOf(),
		error_blob.ReleaseAndGetAddressOf());
	DX::ThrowIfFailed(m_result);

	m_result = D3DCompileFromFile(//ピクセルシェーダー
		m_psPath.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 
		0,
		m_psBlob.ReleaseAndGetAddressOf(), 
		error_blob.ReleaseAndGetAddressOf());
	DX::ThrowIfFailed(m_result);
}

