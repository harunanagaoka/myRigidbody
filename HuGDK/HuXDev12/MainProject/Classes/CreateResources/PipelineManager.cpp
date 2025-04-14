
#include "PipelineManager.h"

void PipelineManager::BuildRootSignature()
{
	//ルートシグネチャ作成
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

	m_result = DirectX::CreateRootSignature(//ルートシグネチャオブジェクト作成
		DXTK->Device,
		&rootSigDesc,
		m_rootSignature.ReleaseAndGetAddressOf());
	DX::ThrowIfFailed(m_result);
};


void PipelineManager::BuildPipelineState(const ComPtr<ID3DBlob>& vsBlob, const ComPtr<ID3DBlob>& psBlob)
{
	// パイプラインステート設定
	DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式を指定

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc{};
	gpsDesc.pRootSignature = m_rootSignature.Get();//ルートシグネチャ指定　すでに作ったやつ
	gpsDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
	gpsDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };//HLSLを入力

	D3D12_RENDER_TARGET_BLEND_DESC rtBlend{};//色のブレンド設定
	rtBlend.BlendEnable = FALSE;//色を混ぜない
	rtBlend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	gpsDesc.BlendState.RenderTarget[0] = rtBlend;
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//どこのピクセルに描くか

	//ポリゴンの塗り方、裏面カリング
	gpsDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//ポリゴンを塗りつぶす
	gpsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//ポリゴンの両面表示
	gpsDesc.RasterizerState.DepthClipEnable = TRUE;//カメラ外は描かない
	gpsDesc.DepthStencilState.DepthEnable = TRUE;
	gpsDesc.DepthStencilState.StencilEnable = FALSE;
	gpsDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpsDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	//1頂点分のデータ
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

	//Pipeline State Object作成（DRAWができるようになる）
	m_result = DXTK->Device->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(m_pipelineState.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(m_result);
};