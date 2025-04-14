
#include "PipelineManager.h"

void PipelineManager::BuildRootSignature()
{
	//���[�g�V�O�l�`���쐬
	CD3DX12_DESCRIPTOR_RANGE descRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	D3D12_ROOT_PARAMETER rootParam{};

	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParam.DescriptorTable.NumDescriptorRanges = 1;
	rootParam.DescriptorTable.pDescriptorRanges = &descRange;

	D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};

	rootSigDesc.NumParameters = 1;
	rootSigDesc.pParameters = &rootParam;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	m_result = DirectX::CreateRootSignature(//���[�g�V�O�l�`���I�u�W�F�N�g�쐬
		DXTK->Device,
		&rootSigDesc,
		m_rootSignature.ReleaseAndGetAddressOf());
	DX::ThrowIfFailed(m_result);
};


void PipelineManager::BuildPipelineState(const ComPtr<ID3DBlob>& vsBlob, const ComPtr<ID3DBlob>& psBlob)
{
	// �p�C�v���C���X�e�[�g�ݒ�
	DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;//�F�̌`�����w��

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc{};
	gpsDesc.pRootSignature = m_rootSignature.Get();//���[�g�V�O�l�`���w��@���łɍ�������
	gpsDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
	gpsDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };//HLSL�����

	D3D12_RENDER_TARGET_BLEND_DESC rtBlend{};//�F�̃u�����h�ݒ�
	rtBlend.BlendEnable = FALSE;//�F�������Ȃ�
	rtBlend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	gpsDesc.BlendState.RenderTarget[0] = rtBlend;
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//�ǂ��̃s�N�Z���ɕ`����

	//�|���S���̓h����A���ʃJ�����O
	gpsDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//�|���S����h��Ԃ�
	gpsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�|���S���̗��ʕ\��
	gpsDesc.RasterizerState.DepthClipEnable = TRUE;//�J�����O�͕`���Ȃ�
	gpsDesc.DepthStencilState.DepthEnable = TRUE;
	gpsDesc.DepthStencilState.StencilEnable = FALSE;
	gpsDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpsDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	//1���_���̃f�[�^
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0 
		}
	};
	gpsDesc.InputLayout = { inputLayout, _countof(inputLayout) };
	gpsDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	gpsDesc.NumRenderTargets = 1;
	gpsDesc.RTVFormats[0] = backBufferFormat;
	gpsDesc.DSVFormat = backBufferFormat;
	gpsDesc.SampleDesc.Count = 1;

	//Pipeline State Object�쐬�iDRAW���ł���悤�ɂȂ�j
	m_result = DXTK->Device->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(m_pipelineState.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(m_result);
};