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
	struct MyVertex//頂点の構造体
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;//normal vector(法線ベクトル)

		static const D3D12_INPUT_ELEMENT_DESC InputElements[2];
		static const D3D12_INPUT_LAYOUT_DESC InputLayout;
	};

	class Primitive
	{
	public:

		static std::shared_ptr<Primitive> CreateCubePrimitive(float size, ID3D12Device* device);

		void Initialize(ID3D12Device* device, const std::vector<MyVertex>& vertices, const std::vector<uint16_t>& indices);
		
		void Draw(ID3D12GraphicsCommandList* commandList);

		void SetWorld(const DirectX::SimpleMath::Matrix& world) { m_world = world; };
		
		const DirectX::SimpleMath::Matrix& GetWorld() const { return m_world; }

		const std::vector<MyVertex> GetVertices() const { return m_vertices; }



		static void ComputeBox(
			std::vector<MyVertex>& vertices,
			std::vector<uint16_t>& indices,
			const DirectX::XMFLOAT3& size);
	
	private:
		ComPtr<ID3D12Resource> m_indexBuffer;
		ComPtr<ID3D12Resource> m_vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};
		UINT m_indexCount;

		DirectX::SimpleMath::Matrix m_world = DirectX::SimpleMath::Matrix::Identity;

		std::vector<MyVertex> m_vertices; // 追加：頂点保持
		std::vector<uint16_t> m_indices;
	};

}