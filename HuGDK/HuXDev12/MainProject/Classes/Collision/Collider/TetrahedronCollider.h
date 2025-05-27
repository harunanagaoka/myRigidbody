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

        //�`�ɉ��������_��ݒ肵�܂��B
        m_localVertices.assign(m_primitiveVertices.begin(), m_primitiveVertices.end());
	}

    SimpleMath::Vector3 ComputeCenter() const  override;
    

private:



    const std::array<SimpleMath::Vector3, 4> m_primitiveVertices = {
        // ���l�ʑ̂̒��_
        SimpleMath::Vector3(1.0f,  1.0f,  1.0f),
        SimpleMath::Vector3(-1.0f, -1.0f,  1.0f),
        SimpleMath::Vector3(-1.0f,  1.0f, -1.0f),
        SimpleMath::Vector3(1.0f, -1.0f, -1.0f),
    };
};

