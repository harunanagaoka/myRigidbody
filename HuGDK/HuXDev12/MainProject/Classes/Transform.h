//
// Transform.h
//

#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"

using namespace DirectX;

class Transform {
public:
	Transform();

    // Set
    void SetPosition(const SimpleMath::Vector3& pos);
    void SetRotationFromEuler(const  SimpleMath::Vector3& euler);
    void SetRotationFromQuat(const  SimpleMath::Vector4& quat); // Quaternion
    void SetScale(const SimpleMath::Vector3& scl);

    // Get
    const SimpleMath::Vector3& GetPosition() const { return m_position; }
    const  SimpleMath::Vector4& GetRotation() const { return m_rotation; }
    const SimpleMath::Vector3& GetScale() const { return m_scale; }

    const SimpleMath::Matrix& GetWorldMatrix() const { 
        if (m_isDirty) {
            UpdateMatrix();
        }
        return m_worldMatrix; 
    }

private :
     SimpleMath::Vector3 m_position;
     SimpleMath::Vector4 m_rotation;
     SimpleMath::Vector3 m_scale;

     mutable SimpleMath::Matrix  m_worldMatrix;

     mutable bool m_isDirty = true;

    void UpdateMatrix() const;

};