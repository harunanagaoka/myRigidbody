//
// CameraManager.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include <d3dcompiler.h>
using Microsoft::WRL::ComPtr;

class CameraManager {
public:
	void Initialize();

	DirectXTK::Camera& Get()  { return m_camera; }

private :
	DirectXTK::Camera m_camera;

	struct CameraBuffer
	{
		DirectX::XMMATRIX m_viewProj;
	};

	ComPtr<ID3D12DescriptorHeap> m_cbvHeap;

	ComPtr<ID3D12Resource> m_cameraCB;

	D3D12_GPU_DESCRIPTOR_HANDLE m_cbvHandleGPU;
};