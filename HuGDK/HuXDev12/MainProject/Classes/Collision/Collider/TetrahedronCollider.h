//
// TetrahedronCollider.h
//

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include "PhysicsCollider.h"
#include "..\..\Resistry\PhysicsTypes.h"
#include <array>

class TetrahedronCollider : public PhysicsCollider{
public:
	TetrahedronCollider(ColliderType type):PhysicsCollider(type) {  

        //形に応じた頂点を設定します。
        m_localVertices.assign(m_primitiveVertices.begin(), m_primitiveVertices.end());
	}

    SimpleMath::Vector3 ComputeCenter() const  override;
    

private:



    const std::array<SimpleMath::Vector3, 4> m_primitiveVertices = {
        // 正四面体の頂点
        SimpleMath::Vector3(1.0f,  1.0f,  1.0f),
        SimpleMath::Vector3(-1.0f, -1.0f,  1.0f),
        SimpleMath::Vector3(-1.0f,  1.0f, -1.0f),
        SimpleMath::Vector3(1.0f, -1.0f, -1.0f),
    };
};

