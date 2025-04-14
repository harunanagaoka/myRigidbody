///OriginalGeometric.h

#pragma once

//#include "..\Base\pch.h"
//#include "..\Base\dxtk.h"
#include <wrl/client.h> 
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

using Microsoft::WRL::ComPtr;
//using std::unique_ptr;
//using std::make_unique;
//using namespace DirectX;

namespace MyGeometory
{
	struct MyVertex//í∏ì_ÇÃç\ë¢ëÃ
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};

	class Primitive
	{
	public:

		void Initialize(ID3D12Device* device, const std::vector<DirectX::VertexPositionNormalTexture> vertices, const std::vector<uint16_t>& indices);

		void Draw(ID3D12GraphicsCommandList* commandList);

		void SetWorld(const DirectX::SimpleMath::Matrix& world) { m_world = world; };

		const DirectX::SimpleMath::Matrix& GetWorld() const { return m_world; }

	private:
		ComPtr<ID3D12Resource> m_indexBuffer;
		ComPtr<ID3D12Resource> m_vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};
		UINT m_indexCount;

		DirectX::SimpleMath::Matrix m_world = DirectX::SimpleMath::Matrix::Identity;
	};

}