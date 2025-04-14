//
// CameraManager.cpp
//

#include "CameraManager.h"

void CameraManager::Initialize()
{
	//m_cameraCB ���\�[�X�쐬
	UINT cbSize = (sizeof(CameraBuffer) + 255) & ~255;

	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC cbResDesc = CD3DX12_RESOURCE_DESC::Buffer(cbSize);

	DX::ThrowIfFailed(DXTK->Device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&cbResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_cameraCB.ReleaseAndGetAddressOf())
	));

	CameraBuffer cbData;
	cbData.m_viewProj = XMMatrixTranspose(m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix());

	CameraBuffer* cbPtr = nullptr;
	m_cameraCB->Map(0, nullptr, reinterpret_cast<void**>(&cbPtr));
	*cbPtr = cbData;
	m_cameraCB->Unmap(0, nullptr);

	// CBV�p�̃f�B�X�N���v�^�q�[�v�im_cbvHandleGPU �̌��j�쐬
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;


	DX::ThrowIfFailed(DXTK->Device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap)));

	// CBV �쐬
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = m_cameraCB->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = cbSize;

	DXTK->Device->CreateConstantBufferView(&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());

	// �n���h���ۑ�
	m_cbvHandleGPU = m_cbvHeap->GetGPUDescriptorHandleForHeapStart();
}