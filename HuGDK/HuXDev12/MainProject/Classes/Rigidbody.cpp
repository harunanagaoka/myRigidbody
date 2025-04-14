#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "Rigidbody.h"
#include <random>

//static std::random_device aa;                         // �V�[�h�i�n�[�h�E�F�A�x�[�X�����j
//static std::mt19937 bbb(aa());

//����pos�������C�\��
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
    
    forceAccum = {}; // �̓��Z�b�g


    if (position.y < ground && velocity.y < 0)//isground
    {
        ApplyCollision();
    }

    shape->SetWorld(SimpleMath::Matrix::CreateTranslation(position)); // �ʒu���f
}

void Rigidbody::ApplyCollision()
{

    velocity *= -(hanpatsu);//�߂肱�݂��ǂ��ɂ����Ȃ��Ɖi���ɉ��ɒ���
    //�o�E���h�t���O����
}


/// <summary>
/// ���S�̍��W���v�Z����
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
/// AABB����
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


