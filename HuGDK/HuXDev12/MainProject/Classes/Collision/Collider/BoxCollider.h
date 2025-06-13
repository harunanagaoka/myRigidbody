//
// BoxCollider.h
//

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include "PhysicsCollider.h"
#include <array>

using namespace std;

class BoxCollider : public PhysicsCollider {
public:
    BoxCollider(ColliderType type) :PhysicsCollider(type) {

        //�`�ɉ��������_��ݒ肵�܂��B
      m_localVertices.assign(m_primitiveVertices.begin(), m_primitiveVertices.end());

      ComputeInertiaTensor();
      
    }

    SimpleMath::Vector3 ComputeCenter() const  override;

    void ComputeInertiaTensor() override;

    void SetSize(SimpleMath::Vector3 size) override;

private:

    //static�ɂ��Ă����������H
    const std::array<SimpleMath::Vector3, 8> m_primitiveVertices =
    {   // �P�ʗ����́i���S���_�j
        //�e���_��size�ivector3�j���������size�̑傫���̒����̂ɂȂ�
        SimpleMath::Vector3(-0.5f, -0.5f, -0.5f),
        SimpleMath::Vector3( 0.5f, -0.5f, -0.5f),
        SimpleMath::Vector3(-0.5f,  0.5f, -0.5f),
        SimpleMath::Vector3( 0.5f,  0.5f, -0.5f),
        SimpleMath::Vector3(-0.5f, -0.5f,  0.5f),
        SimpleMath::Vector3( 0.5f, -0.5f,  0.5f),
        SimpleMath::Vector3(-0.5f,  0.5f,  0.5f),
        SimpleMath::Vector3( 0.5f,  0.5f,  0.5f),
    };
};

