//
// Rigidbody.cpp
//

#include "Rigidbody.h"

void Rigidbody::Update(float deltaTime) {

	if (!m_isStatic) {

		m_velocity.y -= 9.8 * deltaTime;

		//m_angularVelocity *= 0.98f;

		if (m_angularVelocity.LengthSquared() < 0.1f) {
			m_angularVelocity = SimpleMath::Vector3::Zero;
		}

		SimpleMath::Vector3 nextPos = m_transform.GetPosition() + m_velocity * deltaTime;

		m_transform.SetPosition(nextPos);

		float angle = m_angularVelocity.Length() * deltaTime;

		if (angle > 0.0001f) {
			SimpleMath::Vector3 axis = m_angularVelocity;
			axis.Normalize();

			// �p���x�x�N�g������N�H�[�^�j�I������
			SimpleMath::Quaternion deltaRotation = SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle); 

			// ���݂̉�]�ɍ���
			SimpleMath::Quaternion newRotation = deltaRotation * m_transform.GetRotation();

			newRotation.Normalize();
			m_transform.SetRotationFromQuat(newRotation);
		}

	}
	
	//ToDo isStatic�̏ꍇ�Ă΂�Ȃ��悤�ɂ���
	m_collider->UpdateInverseInertiaTensorWorld(m_transform.GetRotation());
	m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());
}



void Rigidbody::ImpactResponse(const Rigidbody* hitObject, ContactInfo& info, bool isA) {
	
	if (m_isStatic) {
		return;
	}

	if (abs(info.normal.y - 1.0f) < 0.01f) {
		info.normal.y = 1;
	}

	if (isA)
	{

		Correct(hitObject, -info.normal,info.penetrationDepth);
		//SetStatic(true);
		SimpleMath::Vector3 relativeVelocity = m_velocity - hitObject->GetVelocity();
		float relNormalVel = relativeVelocity.Dot(-info.normal);
		if (relNormalVel > 0.0f) return; // ����Ă��牽�����Ȃ�
		float numerator = -(1.0f + m_restitution) * relNormalVel;
		float invMassSum = 1.0f / m_mass + 1.0f / hitObject->GetMass();

		float j = numerator / invMassSum;

		//����
		SimpleMath::Vector3 impulse = -info.normal * j;

		m_velocity += impulse / m_mass;

		ApplyFrictionImpulse(hitObject, info, isA);

		//if (abs(m_velocity.y) < 0.05f && info.normal.y > 0.9f) {
		//	m_velocity.y = 0.0f; // �n�ʂɂ���Ƃ������~�߂�
		//}


		SimpleMath::Vector3 r = info.contactPointA - m_transform.GetPosition();
		if (hitObject->IsStatic()) {
			r = SimpleMath::Vector3::Zero;
		}

		SimpleMath::Vector3 torqueImpulse = r.Cross(impulse);

		
		SimpleMath::Vector3 deltaAngularVelocity =
			SimpleMath::Vector3::Transform(torqueImpulse, m_collider->GetInverseInertiaTensorWorld());
		//�u���͂ɂ���Ĕ���������]�̕ω��ʁv
		m_angularVelocity += deltaAngularVelocity;
	}
	else
	{
		SimpleMath::Vector3 tr = m_transform.GetPosition();
		Correct(hitObject, info.normal, info.penetrationDepth);
		SimpleMath::Vector3 tr2 = m_transform.GetPosition();
		//SetStatic(true);
		SimpleMath::Vector3 relativeVelocity = m_velocity - hitObject->GetVelocity();
		float relNormalVel = relativeVelocity.Dot(info.normal);
		if (relNormalVel > 0.0f) return;
		float numerator = -(1.0f + m_restitution) * relNormalVel;
		float invMassSum = 1.0f / m_mass + 1.0f / hitObject->GetMass();

		float j = numerator / invMassSum;

		SimpleMath::Vector3 impulse = info.normal * j;

		m_velocity += impulse / m_mass;

		ApplyFrictionImpulse(hitObject, info, isA);

		//if (abs(m_velocity.y) < 0.05f && info.normal.y > 0.9f) {
		//	m_velocity.y = 0.0f; // �n�ʂɂ���Ƃ������~�߂�
		//}

		SimpleMath::Vector3 r = info.contactPointB - m_transform.GetPosition();
		if (hitObject->IsStatic()) {
			r = SimpleMath::Vector3::Zero;
		}
		
		SimpleMath::Vector3 torqueImpulse = r.Cross(impulse);

		////�u���͂ɂ���Ĕ���������]�̕ω��ʁv
		SimpleMath::Vector3 deltaAngularVelocity =
			SimpleMath::Vector3::Transform(torqueImpulse, m_collider->GetInverseInertiaTensorWorld());
		m_angularVelocity += deltaAngularVelocity;

	}

	if (m_velocity.LengthSquared() < 0.01) {
		m_velocity = SimpleMath::Vector3::Zero;
	}
	if (m_angularVelocity.LengthSquared() < 0.01) {
		m_angularVelocity = SimpleMath::Vector3::Zero;
	}

	//transform�̍X�V������Δ��f
	m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());
	m_collider->UpdateInverseInertiaTensorWorld(m_transform.GetRotation());
}

void Rigidbody::Correct(const Rigidbody* hitObject,SimpleMath::Vector3 normal, float penetrationDepth) {

	if (m_isStatic || penetrationDepth < 0.0001f) //
		return;

	float totalMass = m_mass + hitObject->GetMass();

	if (totalMass == 0.0f) return; // �[�����Z�h�~

	float ratio;
	if (hitObject->IsStatic()) {
		ratio = 1.0f;
	}
	else {
		ratio = hitObject->GetMass() / (m_mass + hitObject->GetMass());
	}

	
	const float correctionBias = 1.2f;
	SimpleMath::Vector3 biasedNormal = normal;
	biasedNormal.y = biasedNormal.y * correctionBias;


	// �����������ׂ��␳�ʁi�ڐG�����ɉ����߂��j
	SimpleMath::Vector3 correction = biasedNormal * penetrationDepth * correctionBias * ratio;

	

	// �ʒu�ɕ␳��������
	m_transform.SetPosition(m_transform.GetPosition() + correction);

}

void Rigidbody::ApplyFrictionImpulse(const Rigidbody* hitObject, const ContactInfo& info, bool isA) {
	if (m_isStatic) return;

	const float maxTorque = 2.0f;

	SimpleMath::Vector3 relativeVel;

	if (isA) {
		relativeVel = m_velocity - hitObject->GetVelocity();
	}
	else {
		relativeVel = hitObject->GetVelocity() - m_velocity;
	}
	

	// ���Α��x�̐ڐ������itangent�����j
	float vt1 = relativeVel.Dot(info.tangent1);
	float vt2 = relativeVel.Dot(info.tangent2);

	// �����C�W���i�f�ނ��Ƃɕς��Ă�OK�P���j
	float frictionCoef = std::sqrt(m_friction * hitObject->GetFriction());

	// ���C�C���p���X�iClamped Coulomb ���C���f���ɂ��Ă������P���j
	float jt1 = -vt1 * frictionCoef;
	float jt2 = -vt2 * frictionCoef;

	SimpleMath::Vector3 frictionImpulse = info.tangent1 * jt1 + info.tangent2 * jt2;

	m_velocity += frictionImpulse / m_mass;

	SimpleMath::Vector3 r;

	// ��]�ւ̉e���i�ڐG�_�����S���炸��Ă���΁j
	if (isA) {
		r = info.contactPointA - m_transform.GetPosition();
	}
	else {
		r = info.contactPointB - m_transform.GetPosition();
	}
	
	SimpleMath::Vector3 torqueImpulse = r.Cross(frictionImpulse);

	if (torqueImpulse.LengthSquared() > maxTorque * maxTorque) {
		torqueImpulse.Normalize();
		torqueImpulse *= maxTorque;
	}

	SimpleMath::Vector3 deltaAngular = SimpleMath::Vector3::Transform(torqueImpulse, m_collider->GetInverseInertiaTensorWorld());
	m_angularVelocity += deltaAngular;

}