//cpp

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "OriginalGeometric.h"
#include "VertexTypes.h"

using namespace MyGeometory;

void Primitive::Initialize(ID3D12Device* device, const std::vector<MyVertex>& vertices, const std::vector<uint16_t>& indices)
{
    if (vertices.size() >= USHRT_MAX)
        return;

    if (indices.size() > UINT32_MAX)
        return;

    m_vertices = vertices;
    m_indices = indices;

    // 頂点バッファ作成
    UINT vbSize = static_cast<UINT>(vertices.size() * sizeof(MyVertex));
    CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(vbSize);

    device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer));

    // データコピー
    MyVertex* vbPtr = nullptr;
    m_vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vbPtr));
    memcpy(vbPtr, vertices.data(), vbSize);
    m_vertexBuffer->Unmap(0, nullptr);

    // 頂点バッファビュー設定
    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(MyVertex);
    m_vertexBufferView.SizeInBytes = vbSize;

    // インデックスバッファ作成
    UINT ibSize = static_cast<UINT>(indices.size() * sizeof(uint16_t));
    CD3DX12_RESOURCE_DESC ibResDesc = CD3DX12_RESOURCE_DESC::Buffer(ibSize);

    device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &ibResDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_indexBuffer));

    uint16_t* ibPtr = nullptr;
    m_indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&ibPtr));
    memcpy(ibPtr, indices.data(), ibSize);
    m_indexBuffer->Unmap(0, nullptr);

    // インデックスバッファビュー設定
    m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
    m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
    m_indexBufferView.SizeInBytes = ibSize;

    // インデックス数保存
    m_indexCount = static_cast<UINT>(indices.size());
}

std::shared_ptr<Primitive> Primitive::CreateCubePrimitive(float size, ID3D12Device* device)
{
    std::vector<MyVertex> vertices;
    std::vector<uint16_t> indices;

    ComputeBox(vertices, indices, DirectX::XMFLOAT3(size, size, size));

    std::shared_ptr<Primitive> primitive = std::make_shared<Primitive>();
    primitive->Initialize(device, vertices, indices);

    return primitive;

}

void Primitive::Draw(ID3D12GraphicsCommandList* commandList)
{
    // 三角形
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 頂点バッファとインデックスバッファ
    commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    commandList->IASetIndexBuffer(&m_indexBufferView);

    // 描画実行
    commandList->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
}

void Primitive::ComputeBox(std::vector<MyVertex>& vertices,std::vector<uint16_t>& indices,const DirectX::XMFLOAT3& size) 
{
    float width = size.x * 0.5f;
    float height = size.y * 0.5f;
    float depth = size.z * 0.5f;

    // 頂点位置（右手系）
    XMFLOAT3 positions[8] =
    {
        { -width, -height, -depth },
        { -width, +height, -depth },
        { +width, +height, -depth },
        { +width, -height, -depth },
        { -width, -height, +depth },
        { -width, +height, +depth },
        { +width, +height, +depth },
        { +width, -height, +depth },
    };

    // 各面の法線とインデックス
    static const uint16_t faceIndices[6][6] =
    {
        { 0, 1, 2, 0, 2, 3 }, // -Z
        { 4, 6, 5, 4, 7, 6 }, // +Z
        { 4, 5, 1, 4, 1, 0 }, // -X
        { 3, 2, 6, 3, 6, 7 }, // +X
        { 1, 5, 6, 1, 6, 2 }, // +Y
        { 4, 0, 3, 4, 3, 7 }, // -Y
    };

    static const XMFLOAT3 faceNormals[6] =
    {
        {  0,  0, -1 },
        {  0,  0, +1 },
        { -1,  0,  0 },
        { +1,  0,  0 },
        {  0, +1,  0 },
        {  0, -1,  0 },
    };

    vertices.clear();
    indices.clear();

    for (int f = 0; f < 6; ++f)
    {
        const auto& normal = faceNormals[f];
        const auto& idx = faceIndices[f];

        for (int i = 0; i < 6; ++i)
        {
            uint16_t vertIndex = idx[i];
            MyVertex v;
            v.position = positions[vertIndex];
            v.normal = normal;
            vertices.push_back(v);
            indices.push_back(static_cast<uint16_t>(vertices.size() - 1));
        }
    }
}

const D3D12_INPUT_ELEMENT_DESC MyGeometory::MyVertex::InputElements[2] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(MyGeometory::MyVertex, position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(MyGeometory::MyVertex, normal),   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

const D3D12_INPUT_LAYOUT_DESC MyGeometory::MyVertex::InputLayout =
{
    

};

