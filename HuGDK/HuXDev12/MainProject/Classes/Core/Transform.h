//
// Transform.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"

class Transform {
public:
    Transform() : m_position(SimpleMath::Vector3::Zero),
        m_rotation(0.f, 0.f, 0.f, 1.f),
        m_scale(SimpleMath::Vector3::One){}

    // Set
    void SetPosition(const SimpleMath::Vector3& pos);
    void SetRotationFromEuler(const  SimpleMath::Vector3& euler);
    void SetRotationFromQuat(const  SimpleMath::Quaternion& quat);
    void SetScale(const SimpleMath::Vector3& scl);

    // Get
    const SimpleMath::Vector3& GetPosition() const { return m_position; }
    const  SimpleMath::Quaternion& GetRotation() const { return m_rotation; }
    const SimpleMath::Vector3& GetScale() const { return m_scale; }

    /// 現在のtransformの値に応じたワールド行列を返します。
    const SimpleMath::Matrix& GetWorldMatrix() const { 
        if (m_isDirty) {
            UpdateMatrix();
        }
        return m_worldMatrix; 
    }

private :
     void UpdateMatrix() const;

     SimpleMath::Vector3 m_position;
     SimpleMath::Quaternion m_rotation;
     SimpleMath::Vector3 m_scale;

     mutable SimpleMath::Matrix  m_worldMatrix;
     mutable bool m_isDirty = true;

};