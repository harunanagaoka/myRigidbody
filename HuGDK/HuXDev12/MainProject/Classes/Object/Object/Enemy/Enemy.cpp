//
// Enemy.cpp
//

#include "Enemy.h"

void Enemy::Initialize() {
	m_camera.SetCamera(m_id, m_transform);
	m_sprite = SpriteName::BLACKRECT;
}

void Enemy::Update() {

}

void Enemy::Render(SpriteManager& sp) {

	sp.Render(m_sprite, m_transform->GetScreenPosition());

}

void Enemy::OnCollisionEnter(Object* other) {
	if (other->GetObjectType() == ObjectType::Enemy)
	{
		//押し出しする
	}

	if (other->GetObjectType() == ObjectType::Player)
	{
		//ダメージ　//敵のATKとかをもらう

	}
	if (other->GetObjectType() == ObjectType::PlayerBullet)
	{
		//ダメージ　
	}
}