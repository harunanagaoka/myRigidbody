//
// Rigidbody.cpp
//

#include "Rigidbody.h"

/// <summary>
/// ��]���x�A���s���x���v�Z���A���f���܂��B
/// </summary>
void Rigidbody::Update(float deltaTime) {

	if (m_isStatic) {

		m_collider->UpdateInverseInertiaTensorWorld(m_transform.GetRotation());
		m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());
		return;
	}
	if (m_isGravity) {
		m_velocity.y -= m_gravitationalAcceleration * deltaTime;
	}
		
	if (m_angularVelocity.LengthSquared() < m_angularVelocityThreshold) {
		m_angularVelocity = SimpleMath::Vector3::Zero;
	}

	SimpleMath::Vector3 nextPos = m_transform.GetPosition() + m_velocity * deltaTime;

	m_transform.SetPosition(nextPos);

	float angle = m_angularVelocity.Length() * deltaTime;

	if (angle > m_angularVelocityThreshold) {
		// �p���x�x�N�g�������]�N�H�[�^�j�I���𐶐����A���݂̉�]�ɍ���

		SimpleMath::Vector3 axis = m_angularVelocity;
		axis.Normalize();

		// �p���x�x�N�g������N�H�[�^�j�I������
		SimpleMath::Quaternion deltaRotation = SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle); 

		// ���݂̉�]�ɍ���
		SimpleMath::Quaternion newRotation = deltaRotation * m_transform.GetRotation();

		newRotation.Normalize();
		m_transform.SetRotationFromQuat(newRotation);
	}

	m_collider->UpdateInverseInertiaTensorWorld(m_transform.GetRotation());
	m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());
}


/// <summary>
/// �Փˏ����󂯎�蕽�s���x�A�p���x�ɔ��f���܂��B
/// </summary>
/// <param name="hitObject">�Փ˂�������</param>
/// <param name="info">�Փˏ��</param>
/// <param name="isA">�Փˏ��̂ǂ���g����</param>
void Rigidbody::ImpactResponse(const Rigidbody* hitObject, ContactInfo& info, bool isA) {
	
	if (m_isStatic) {
		return;
	}

	//�Փ˖@�����قڏ�����iY�������j��������A���S��(0, 1, 0) �ɂ���
	//�덷�΍�ł��B
	if (abs(info.normal.y - 1.0f) < EPSILON) {
		info.normal.y = 1;
	}

	if (isA)
	{

		Correct(hitObject, -info.normal,info.penetrationDepth);

		//���Α��x�����߂Đi�s�����𒲂ׂĂ��܂��B
		SimpleMath::Vector3 relativeVelocity = m_velocity - hitObject->GetVelocity();
		float relNormalVel = relativeVelocity.Dot(-info.normal);

		// �Փ˖ʂ��痣�������ɓ����Ă����甽��(impulse)���N�����Ȃ��B
		if (relNormalVel > 0.0f) {
			return;
		} 

		// impulse���Z�o���A���s���x�ɔ��f���܂��B
		float numerator = -(1.0f + m_restitution) * relNormalVel;//�����������
		float invMassSum = 1.0f / m_mass + 1.0f / hitObject->GetMass();//���ʂ��d���قǔ������Â炢
		float j = numerator / invMassSum;//������
		SimpleMath::Vector3 impulse = -info.normal * j;
		m_velocity += impulse / m_mass;

		ApplyFrictionImpulse(hitObject, info, isA);

		//�Փ˓_�����S����Y���Ă����ꍇ�A��]�𐶂ޏ���
		SimpleMath::Vector3 r = info.contactPointA - m_transform.GetPosition();

		if (hitObject->IsStatic()) {
			r = SimpleMath::Vector3::Zero;
		}

		//��]�g���N�𐶐����A�p���x�ɕϊ����܂��B
		SimpleMath::Vector3 torqueImpulse = r.Cross(impulse);
		SimpleMath::Vector3 deltaAngularVelocity =
		SimpleMath::Vector3::Transform(torqueImpulse, m_collider->GetInverseInertiaTensorWorld());
		m_angularVelocity += deltaAngularVelocity;
	}
	else
	{
		Correct(hitObject, info.normal, info.penetrationDepth);

		//���Α��x�����߂Đi�s�����𒲂ׂĂ��܂��B
		SimpleMath::Vector3 relativeVelocity = m_velocity - hitObject->GetVelocity();
		float relNormalVel = relativeVelocity.Dot(info.normal);

		// �Փ˖ʂ��痣�������ɓ����Ă����甽��(impulse)���N�����Ȃ��B
		if (relNormalVel > 0.0f) {	
			return;
		}

		// impulse���Z�o���A���s���x�ɔ��f���܂��B
		float numerator = -(1.0f + m_restitution) * relNormalVel;//�����������
		float invMassSum = 1.0f / m_mass + 1.0f / hitObject->GetMass();//���ʂ��d���قǔ������Â炢
		float j = numerator / invMassSum;//������
		SimpleMath::Vector3 impulse = info.normal * j;
		m_velocity += impulse / m_mass;

		ApplyFrictionImpulse(hitObject, info, isA);

		//�Փ˓_�����S����Y���Ă����ꍇ�A��]�𐶂ޏ���
		SimpleMath::Vector3 r = info.contactPointB - m_transform.GetPosition();
		if (hitObject->IsStatic()) {
			r = SimpleMath::Vector3::Zero;
		}
		
		//��]�g���N�𐶐����A�p���x�ɕϊ����܂��B
		SimpleMath::Vector3 torqueImpulse = r.Cross(impulse);
		SimpleMath::Vector3 deltaAngularVelocity =
			SimpleMath::Vector3::Transform(torqueImpulse, m_collider->GetInverseInertiaTensorWorld());
		m_angularVelocity += deltaAngularVelocity;

	}

	if (m_velocity.LengthSquared() < EPSILON) {
		m_velocity = SimpleMath::Vector3::Zero;
	}
	if (m_angularVelocity.LengthSquared() < EPSILON) {
		m_angularVelocity = SimpleMath::Vector3::Zero;
	}

	//transform�̍X�V������Δ��f
	m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());
	m_collider->UpdateInverseInertiaTensorWorld(m_transform.GetRotation());
}

/// <summary>
/// �Փː[�x�ɉ����Ă߂荞�ݕ␳���s���܂��B
/// </summary>
/// <param name="hitObject">�Փˑ���</param>
/// <param name="normal">�Փ˖@��</param>
/// <param name="penetrationDepth">�Փː[�x</param>
void Rigidbody::Correct(const Rigidbody* hitObject,SimpleMath::Vector3 normal, float penetrationDepth) {

	if (m_isStatic || penetrationDepth < EPSILON) {
		return;
	}
		
	float totalMass = m_mass + hitObject->GetMass();
	if (totalMass == 0.0f) {  // �[�����Z�h�~
		return;
	}

	//�y���������傫���䗦�ňړ������܂��B
	float ratio;
	if (hitObject->IsStatic()) {
		ratio = 1.0f;
	}
	else {
		ratio = hitObject->GetMass() / (m_mass + hitObject->GetMass());
	}

	SimpleMath::Vector3 biasedNormal = normal;
	biasedNormal.y = biasedNormal.y * m_correctionBias;

	// �␳�ʁi�ڐG�����ɉ����߂��j
	SimpleMath::Vector3 correction = biasedNormal * penetrationDepth * m_correctionBias * ratio;
	m_transform.SetPosition(m_transform.GetPosition() + correction);
}

/// <summary>
/// ���C���v�Z�����x�ɔ��f���܂�
/// </summary>
/// <param name="hitObject">�Փˑ���</param>
/// <param name="info">�Փˏ��</param>
/// <param name="isA">�ǂ̏Փˏ����g����</param>
void Rigidbody::ApplyFrictionImpulse(const Rigidbody* hitObject, const ContactInfo& info, bool isA) {
	if (m_isStatic) { 
		return;
	}

	const float maxTorque = 2.0f;

	//���Α��x
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

	// �����C�W���̌v�Z
	float frictionCoef = std::sqrt(m_friction * hitObject->GetFriction());

	// ���C�C���p���X�̌v�Z
	float jt1 = -vt1 * frictionCoef;
	float jt2 = -vt2 * frictionCoef;

	SimpleMath::Vector3 frictionImpulse = info.tangent1 * jt1 + info.tangent2 * jt2;
	m_velocity += frictionImpulse / m_mass;

	// ��]�ւ̉e���̌v�Z�i�ڐG�_�����S���炸��Ă���΁j
	SimpleMath::Vector3 r;

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

/// <summary>
/// ���͂����x�N�g���̗͂������܂��B
/// </summary>
void Rigidbody::AddForce(const SimpleMath::Vector3& force) {
	if (m_isStatic) { 
		return;
	}

	m_velocity += force / m_mass;

}