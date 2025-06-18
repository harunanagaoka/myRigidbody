//
// Rigidbody.cpp
//

#include "Rigidbody.h"

/// <summary>
/// 回転速度、平行速度を計算し、反映します。
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
		// 角速度ベクトルから回転クォータニオンを生成し、現在の回転に合成

		SimpleMath::Vector3 axis = m_angularVelocity;
		axis.Normalize();

		// 角速度ベクトルからクォータニオン生成
		SimpleMath::Quaternion deltaRotation = SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle); 

		// 現在の回転に合成
		SimpleMath::Quaternion newRotation = deltaRotation * m_transform.GetRotation();

		newRotation.Normalize();
		m_transform.SetRotationFromQuat(newRotation);
	}

	m_collider->UpdateInverseInertiaTensorWorld(m_transform.GetRotation());
	m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());
}


/// <summary>
/// 衝突情報を受け取り平行速度、角速度に反映します。
/// </summary>
/// <param name="hitObject">衝突した相手</param>
/// <param name="info">衝突情報</param>
/// <param name="isA">衝突情報のどれを使うか</param>
void Rigidbody::ImpactResponse(const Rigidbody* hitObject, ContactInfo& info, bool isA) {
	
	if (m_isStatic) {
		return;
	}

	//衝突法線がほぼ上向き（Y軸方向）だったら、完全に(0, 1, 0) にする
	//誤差対策です。
	if (abs(info.normal.y - 1.0f) < EPSILON) {
		info.normal.y = 1;
	}

	if (isA)
	{

		Correct(hitObject, -info.normal,info.penetrationDepth);

		//相対速度を求めて進行方向を調べています。
		SimpleMath::Vector3 relativeVelocity = m_velocity - hitObject->GetVelocity();
		float relNormalVel = relativeVelocity.Dot(-info.normal);

		// 衝突面から離れる方向に動いていたら反発(impulse)を起こさない。
		if (relNormalVel > 0.0f) {
			return;
		} 

		// impulseを算出し、平行速度に反映します。
		float numerator = -(1.0f + m_restitution) * relNormalVel;//反発させる量
		float invMassSum = 1.0f / m_mass + 1.0f / hitObject->GetMass();//質量が重いほど反発しづらい
		float j = numerator / invMassSum;//反発力
		SimpleMath::Vector3 impulse = -info.normal * j;
		m_velocity += impulse / m_mass;

		ApplyFrictionImpulse(hitObject, info, isA);

		//衝突点が中心からズレていた場合、回転を生む処理
		SimpleMath::Vector3 r = info.contactPointA - m_transform.GetPosition();

		if (hitObject->IsStatic()) {
			r = SimpleMath::Vector3::Zero;
		}

		//回転トルクを生成し、角速度に変換します。
		SimpleMath::Vector3 torqueImpulse = r.Cross(impulse);
		SimpleMath::Vector3 deltaAngularVelocity =
		SimpleMath::Vector3::Transform(torqueImpulse, m_collider->GetInverseInertiaTensorWorld());
		m_angularVelocity += deltaAngularVelocity;
	}
	else
	{
		Correct(hitObject, info.normal, info.penetrationDepth);

		//相対速度を求めて進行方向を調べています。
		SimpleMath::Vector3 relativeVelocity = m_velocity - hitObject->GetVelocity();
		float relNormalVel = relativeVelocity.Dot(info.normal);

		// 衝突面から離れる方向に動いていたら反発(impulse)を起こさない。
		if (relNormalVel > 0.0f) {	
			return;
		}

		// impulseを算出し、平行速度に反映します。
		float numerator = -(1.0f + m_restitution) * relNormalVel;//反発させる量
		float invMassSum = 1.0f / m_mass + 1.0f / hitObject->GetMass();//質量が重いほど反発しづらい
		float j = numerator / invMassSum;//反発力
		SimpleMath::Vector3 impulse = info.normal * j;
		m_velocity += impulse / m_mass;

		ApplyFrictionImpulse(hitObject, info, isA);

		//衝突点が中心からズレていた場合、回転を生む処理
		SimpleMath::Vector3 r = info.contactPointB - m_transform.GetPosition();
		if (hitObject->IsStatic()) {
			r = SimpleMath::Vector3::Zero;
		}
		
		//回転トルクを生成し、角速度に変換します。
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

	//transformの更新があれば反映
	m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());
	m_collider->UpdateInverseInertiaTensorWorld(m_transform.GetRotation());
}

/// <summary>
/// 衝突深度に応じてめり込み補正を行います。
/// </summary>
/// <param name="hitObject">衝突相手</param>
/// <param name="normal">衝突法線</param>
/// <param name="penetrationDepth">衝突深度</param>
void Rigidbody::Correct(const Rigidbody* hitObject,SimpleMath::Vector3 normal, float penetrationDepth) {

	if (m_isStatic || penetrationDepth < EPSILON) {
		return;
	}
		
	float totalMass = m_mass + hitObject->GetMass();
	if (totalMass == 0.0f) {  // ゼロ除算防止
		return;
	}

	//軽い方をより大きい比率で移動させます。
	float ratio;
	if (hitObject->IsStatic()) {
		ratio = 1.0f;
	}
	else {
		ratio = hitObject->GetMass() / (m_mass + hitObject->GetMass());
	}

	SimpleMath::Vector3 biasedNormal = normal;
	biasedNormal.y = biasedNormal.y * m_correctionBias;

	// 補正量（接触方向に押し戻す）
	SimpleMath::Vector3 correction = biasedNormal * penetrationDepth * m_correctionBias * ratio;
	m_transform.SetPosition(m_transform.GetPosition() + correction);
}

/// <summary>
/// 摩擦を計算し速度に反映します
/// </summary>
/// <param name="hitObject">衝突相手</param>
/// <param name="info">衝突情報</param>
/// <param name="isA">どの衝突情報を使うか</param>
void Rigidbody::ApplyFrictionImpulse(const Rigidbody* hitObject, const ContactInfo& info, bool isA) {
	if (m_isStatic) { 
		return;
	}

	const float maxTorque = 2.0f;

	//相対速度
	SimpleMath::Vector3 relativeVel;
	if (isA) {
		relativeVel = m_velocity - hitObject->GetVelocity();
	}
	else {
		relativeVel = hitObject->GetVelocity() - m_velocity;
	}
	
	// 相対速度の接線成分（tangent方向）
	float vt1 = relativeVel.Dot(info.tangent1);
	float vt2 = relativeVel.Dot(info.tangent2);

	// 動摩擦係数の計算
	float frictionCoef = std::sqrt(m_friction * hitObject->GetFriction());

	// 摩擦インパルスの計算
	float jt1 = -vt1 * frictionCoef;
	float jt2 = -vt2 * frictionCoef;

	SimpleMath::Vector3 frictionImpulse = info.tangent1 * jt1 + info.tangent2 * jt2;
	m_velocity += frictionImpulse / m_mass;

	// 回転への影響の計算（接触点が中心からずれていれば）
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
/// 入力したベクトルの力を加えます。
/// </summary>
void Rigidbody::AddForce(const SimpleMath::Vector3& force) {
	if (m_isStatic) { 
		return;
	}

	m_velocity += force / m_mass;

}