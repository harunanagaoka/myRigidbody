//
// Rigidbody.cpp
//

#include "Rigidbody.h"

void Rigidbody::Update() {

	if (m_isStatic)
	{
		m_transform.SetScale(SimpleMath::Vector3(1.0, 1.0, 1.0));
	}

	if (!m_isStatic) {

		m_velocity.y -= 9.8 * DXTK->Time.deltaTime;

		SimpleMath::Vector3 nextPos = m_transform.GetPosition() + m_velocity * DXTK->Time.deltaTime;

		m_transform.SetPosition(nextPos);

		float angle = m_angularVelocity.Length() * DXTK->Time.deltaTime;

		if (angle > 0.0001f) {
			SimpleMath::Vector3 axis = m_angularVelocity;
			axis.Normalize();

			// �p���x�x�N�g������N�H�[�^�j�I������
			SimpleMath::Quaternion deltaRotation = SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle); 

			// ���݂̉�]�ɍ����i���ԑ厖�I�j
			SimpleMath::Quaternion newRotation = deltaRotation * m_transform.GetRotation();

			newRotation.Normalize();
			m_transform.SetRotationFromQuat(newRotation);
		}

	}
	
	//ToDo isStatic�̏ꍇ�Ă΂�Ȃ��悤�ɂ���
	m_collider->UpdateInverseInertiaTensorWorld(m_transform.GetRotation());
	m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());
}



void Rigidbody::ImpactResponse(const Rigidbody* hitObject, const ContactInfo& info, bool isA) {
	
	if (m_isStatic) {
		return;
	}

	if (isA)
	{
		Correct(hitObject, info.normal,info.penetrationDepth);

		SimpleMath::Vector3 relativeVelocity = m_velocity - hitObject->GetVelocity();
		float relNormalVel = relativeVelocity.Dot(info.normal);
		if (relNormalVel > 0.0f) return; // ����Ă��牽�����Ȃ�
		float numerator = -(1.0f + m_restitution) * relNormalVel;
		float invMassSum = 1.0f / m_mass + 1.0f / hitObject->GetMass();

		float j = numerator / invMassSum;

		//����
		SimpleMath::Vector3 impulse = info.normal * j;


		SimpleMath::Vector3 r = info.contactPointA - m_transform.GetPosition();
		SimpleMath::Vector3 torqueImpulse = r.Cross(impulse);

		//�u���͂ɂ���Ĕ���������]�̕ω��ʁv
		SimpleMath::Vector3 deltaAngularVelocity =
			SimpleMath::Vector3::Transform(torqueImpulse, m_collider->GetInverseInertiaTensorWorld());
		m_angularVelocity += deltaAngularVelocity;
	}
	else
	{
		Correct(hitObject, -info.normal, info.penetrationDepth);

		SimpleMath::Vector3 relativeVelocity = m_velocity - hitObject->GetVelocity();
		float relNormalVel = relativeVelocity.Dot(-info.normal);
		if (relNormalVel > 0.0f) return;
		float numerator = -(1.0f + m_restitution) * relNormalVel;
		float invMassSum = 1.0f / m_mass + 1.0f / hitObject->GetMass();

		float j = numerator / invMassSum;

		SimpleMath::Vector3 impulse = -info.normal * j;
		SimpleMath::Vector3 r = info.contactPointB - m_transform.GetPosition();
		SimpleMath::Vector3 torqueImpulse = r.Cross(impulse);

		//�u���͂ɂ���Ĕ���������]�̕ω��ʁv
		SimpleMath::Vector3 deltaAngularVelocity =
			SimpleMath::Vector3::Transform(torqueImpulse, m_collider->GetInverseInertiaTensorWorld());
		m_angularVelocity += deltaAngularVelocity;

	}

	//transform�̍X�V������Δ��f
	m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());
	m_collider->UpdateInverseInertiaTensorWorld(m_transform.GetRotation());
}

void Rigidbody::Correct(const Rigidbody* hitObject,SimpleMath::Vector3 normal, float penetrationDepth) {

	if (m_isStatic) return;

	float totalMass = m_mass + hitObject->GetMass();

	if (totalMass == 0.0f) return; // �[�����Z�h�~

	float ratio;
	if (hitObject->IsStatic()) {
		ratio = 1.0f;
	}
	else {
		ratio = hitObject->GetMass() / (m_mass + hitObject->GetMass());
	}

	// �����������ׂ��␳�ʁi�ڐG�����ɉ����߂��j
	SimpleMath::Vector3 correction = normal * penetrationDepth * ratio;

	// �ʒu�ɕ␳��������
	m_transform.SetPosition(m_transform.GetPosition() + correction);

}