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

        //形に応じた頂点を設定します。
      m_localVertices.assign(m_primitiveVertices.begin(), m_primitiveVertices.end());

      ComputeInertiaTensor();
      
    }

    SimpleMath::Vector3 ComputeCenter() const  override;

    void ComputeInertiaTensor() override;

    void SetSize(SimpleMath::Vector3 size) override;

private:

    //staticにしてもいいかも？
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
};

