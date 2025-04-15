//
// BoxCollider.h
//

#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include <array>

using namespace std;

class BoxCollider {
public:
    BoxCollider();//なんも入れなければ単位立方体を、Vec3入れた時は任意サイズにできるようにする

    void SetSize(const SimpleMath::Vector3& size); // 任意サイズの設定

    // Transformを反映(Transformが変わった時、必ず所有者側から呼び出すこと）
    void UpdateWorldVertices(const SimpleMath::Matrix& worldMatrix); 

    const array<SimpleMath::Vector3, 8> GetWorldVertices() const { return m_worldVertices; }

private:
    SimpleMath::Vector3 m_localSize = SimpleMath::Vector3(1.0f, 1.0f, 1.0f); // コライダーのローカルサイズ
     
    array<SimpleMath::Vector3, 8> m_localVertices;// ローカル頂点 

    array<SimpleMath::Vector3, 8> m_worldVertices; // ワールド空間の頂点

    const std::array<SimpleMath::Vector3, 8> m_primitiveVertices =
    {   // 単位立方体（中心原点）
        //各頂点にsize（vector3）をかけるとsizeの大きさの直方体になる
        SimpleMath::Vector3(-0.5f, -0.5f, -0.5f),
        SimpleMath::Vector3( 0.5f, -0.5f, -0.5f),
        SimpleMath::Vector3(-0.5f,  0.5f, -0.5f),
        SimpleMath::Vector3( 0.5f,  0.5f, -0.5f),
        SimpleMath::Vector3(-0.5f, -0.5f,  0.5f),
        SimpleMath::Vector3( 0.5f, -0.5f,  0.5f),
        SimpleMath::Vector3(-0.5f,  0.5f,  0.5f),
        SimpleMath::Vector3( 0.5f,  0.5f,  0.5f),
    };

    //（外から）衝突してるカモ→コライダー側でtransformからワールド頂点取得
    //ワールド頂点返す→他のボックスのワールド頂点と比べて衝突点を調べてもらう

    // 三角形インデックス（各面2枚） 必要かは謎
    //const std::array<unsigned int, 36> m_triangleIndices = {
    //    // 前面 (z+)
    //    4, 6, 5,
    //    5, 6, 7,

    //    // 背面 (z-)
    //    0, 1, 2,
    //    1, 3, 2,

    //    // 左面 (x-)
    //    0, 2, 4,
    //    4, 2, 6,

    //    // 右面 (x+)
    //    1, 5, 3,
    //    5, 7, 3,

    //    // 上面 (y+)
    //    2, 3, 6,
    //    3, 7, 6,

    //    // 下面 (y-)
    //    0, 4, 1,
    //    1, 4, 5
    //};
};