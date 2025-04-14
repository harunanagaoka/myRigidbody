#include "ShaderManager.h"

void ShaderManager::LoadShaders()
{
	m_result = D3DCompileFromFile(//���_�V�F�[�_�[
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

	m_result = D3DCompileFromFile(//�s�N�Z���V�F�[�_�[
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

