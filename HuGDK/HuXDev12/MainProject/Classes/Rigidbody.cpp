#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "Rigidbody.h"
#include <random>

//static std::random_device aa;                         // シード（ハードウェアベース乱数）
//static std::mt19937 bbb(aa());

//引数pos部分改修予定
Rigidbody::Rigidbody(std::shared_ptr<Primitive> primitive, SimpleMath::Matrix matrix, SimpleMath::Vector3 pos, bool useGravity)
{
    shape = primitive;
    m_matrix = matrix;
    std::vector<MyVertex> vertice = shape->GetVertices();
    position = ComputeCenterFromVertices(vertice);
    m_halfExtents = ComputeHalfExtentsFromVertices(vertice);
    isGravity = useGravity;
    //std::uniform_real_distribution<float> disthanpatsu(0.4f, 0.9f);
    //hanpatsu = disthanpatsu(bbb);
    initPos = pos;
    position = pos;
    //shape->SetWorld(SimpleMath::Matrix::CreateTranslation(position));
}

void Rigidbody::Update()
{
    float dt = DXTK->Time.deltaTime;

    if (isGravity)
    {
        velocity.y -= 9.8 * dt;
    }

    acceleration = forceAccum / mass;
    velocity += acceleration * dt;

    position += velocity * dt;
    
    forceAccum = {}; // 力リセット


    if (position.y < ground && velocity.y < 0)//isground
    {
        ApplyCollision();
    }

    shape->SetWorld(SimpleMath::Matrix::CreateTranslation(position)); // 位置反映
}

void Rigidbody::ApplyCollision()
{

    velocity *= -(hanpatsu);//めりこみをどうにかしないと永遠に下に沈む
    //バウンドフラグ導入
}


/// <summary>
/// 中心の座標を計算する
/// </summary>
SimpleMath::Vector3 Rigidbody::ComputeCenterFromVertices(const std::vector<MyVertex>& vertices) {
    SimpleMath::Vector3 min = vertices[0].position;
    SimpleMath::Vector3 max = vertices[0].position;

    for (const auto& v : vertices) {
        min = SimpleMath::Vector3::Min(min, v.position);
        max = SimpleMath::Vector3::Max(max, v.position);
    }

    return (min + max) * 0.5f;
}

/// <summary>
/// AABB生成
/// </summary>
SimpleMath::Vector3 Rigidbody::ComputeHalfExtentsFromVertices(const std::vector<MyVertex>& vertices) {
    SimpleMath::Vector3 min = vertices[0].position;
    SimpleMath::Vector3 max = vertices[0].position;

    for (const auto& v : vertices) {
        min = SimpleMath::Vector3::Min(min, v.position);
        max = SimpleMath::Vector3::Max(max, v.position);
    }

    return (max - min) * 0.5f;
}


