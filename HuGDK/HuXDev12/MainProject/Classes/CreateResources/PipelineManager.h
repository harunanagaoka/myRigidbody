//
// PipelineManager.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include <d3dcompiler.h>
using Microsoft::WRL::ComPtr;

class PipelineManager {
	public:
        void BuildRootSignature();

        void BuildPipelineState(const ComPtr<ID3DBlob>& vsBlob, const ComPtr<ID3DBlob>& psBlob);

        const ComPtr<ID3D12RootSignature> GetRootSignature() const { return m_rootSignature; }

        const ComPtr<ID3D12PipelineState> GetPipelineState() const { return m_pipelineState; }

private :
    HRESULT m_result;

    ComPtr<ID3D12RootSignature> m_rootSignature;

    ComPtr<ID3D12PipelineState> m_pipelineState;

};
/*class PipelineBuilder {
public:
    void Build(ID3D12Device* device, ID3DBlob* vs, ID3DBlob* ps);
    ID3D12PipelineState* GetPipelineState() const;
    ID3D12RootSignature* GetRootSignature() const;
};*/