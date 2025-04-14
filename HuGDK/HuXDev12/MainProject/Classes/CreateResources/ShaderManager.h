//
// ShaderManager.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include <d3dcompiler.h>
using Microsoft::WRL::ComPtr;

class ShaderManager {
public:
    void LoadShaders();

    const ComPtr<ID3DBlob>& GetVS() const { return m_vsBlob; }

    const ComPtr<ID3DBlob>& GetPS() const { return m_psBlob; }

private:
    HRESULT m_result;

    ComPtr<ID3DBlob> m_vsBlob;//頂点シェーダー
    const std::wstring m_vsPath = L"HLSL\\BasicVertexShader.hlsl";

    ComPtr<ID3DBlob> m_psBlob;//ピクセルシェーダー
    const std::wstring m_psPath = L"HLSL\\BasicPixelShader.hlsl";

    ComPtr<ID3DBlob> error_blob;//エラー用

   
};
