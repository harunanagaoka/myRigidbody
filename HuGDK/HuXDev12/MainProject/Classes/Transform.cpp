#include "Transform.h"

Transform::Transform()
    : m_position(0.f, 0.f, 0.f),
      m_rotation(0.f, 0.f, 0.f, 1.f), 
      m_scale   (1.f, 1.f, 1.f)
{}

void Transform::SetPosition(const SimpleMath::Vector3& pos) {
    m_position = pos;
    m_isDirty = true;
}

void Transform::SetRotationFromQuat(const SimpleMath::Vector4& quat) {
    m_rotation = quat;
    m_isDirty = true;
}

void Transform::SetRotationFromEuler(const  SimpleMath::Vector3& euler){
    m_rotation = SimpleMath::Quaternion::CreateFromYawPitchRoll(euler.y, euler.x, euler.z);
    m_isDirty = true;
}

void Transform::SetScale(const SimpleMath::Vector3& scl) {
    m_scale = scl;
    m_isDirty = true;
}

void Transform::UpdateMatrix() const {
    SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(m_scale);
    SimpleMath::Matrix rotation = SimpleMath::Matrix::CreateFromQuaternion(m_rotation);
    SimpleMath::Matrix transform = SimpleMath::Matrix::CreateTranslation(m_position);

    m_worldMatrix = scale * rotation * transform;

    m_isDirty = false;
}