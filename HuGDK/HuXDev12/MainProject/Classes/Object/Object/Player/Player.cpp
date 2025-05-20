//
// Player.h
//
#include "Player.h"

void Player::Initialize() {
	m_sprite = SpriteName::PlayerFront;
	m_camera.SetCamera(m_id, m_transform);
	m_camera.SetFollowingCamera(m_id);
	m_transform->SetPosition(SimpleMath::Vector2(50,100));
}

void Player::Update() {

	Move();
	static float Time = 0;
	Time += DXTK->Time.deltaTime;

	if (Time > 1)
	{
		m_isAttack = true;
		Time = 0;
	}

	if (m_hp <= 0)
	{
		m_isDead = true;
	}
}

void Player::Render(SpriteManager& sp) {

	sp.Render(m_sprite, m_transform->GetScreenPosition());

}

void Player::OnCollisionEnter(Object* other) {
	if (other->GetObjectType() == ObjectType::Enemy)
	{
		//ダメージ　//敵のATKとかをもらう

	}
	if (other->GetObjectType() == ObjectType::EnemyBullet)
	{
		//ダメージ　
	}
}

void Player::Move() {
	//斜め速くならないよう直す
	SimpleMath::Vector2 movePos = m_transform->GetPosition();

	if (InputSystem.Keyboard.isPressed.D)
	{
		movePos.x += m_velocity;
	}
	if (InputSystem.Keyboard.isPressed.A)
	{
		movePos.x -= m_velocity;
	}
	if (InputSystem.Keyboard.isPressed.W)
	{
		movePos.y -= m_velocity;
	}
	if (InputSystem.Keyboard.isPressed.S)
	{
		movePos.y += m_velocity;
	}

	m_transform->SetPosition(movePos);

}