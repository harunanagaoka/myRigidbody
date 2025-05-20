//
// Bullet.cpp
//

#include "Bullet.h"

void Bullet::Initialize() {
	m_sprite = SpriteName::PlShot;
	m_camera.SetCamera(m_id, m_transform);
}

void Bullet::Update() {

	SimpleMath::Vector2 pos = m_transform->GetPosition();
	pos += m_direction * m_velocity;
	m_transform->SetPosition(pos);

	m_lifeTime -= DXTK->Time.deltaTime;
	if (m_lifeTime < 0)
	{
		m_isDead = true;
	}
}

void Bullet::Render(SpriteManager& sp) {

	sp.Render(m_sprite, m_transform->GetScreenPosition());
}

void Bullet::SetDirection(vector<SimpleMath::Vector2>& direction) {
	int closest = -1;
	float min = std::numeric_limits<float>::max();
	SimpleMath::Vector2 thisPos = m_transform->GetPosition();

	for (int i = 0; i < direction.size(); i++)
	{
		float dist = (direction[i] - thisPos).LengthSquared();
		if (dist < min) {
			min = dist;
			closest = i;
		}
	}

	m_direction = direction[closest] - thisPos;
	m_direction.Normalize();
}

void Bullet::OnCollisionEnter(Object* other) {
	//Á‚¦‚é’e‚ÍÁ‚¦‚é
}