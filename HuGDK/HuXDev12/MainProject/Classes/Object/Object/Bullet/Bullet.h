//
// Bullet.h
//

#pragma once

#include "..\..\..\..\Base\pch.h"
#include "..\..\..\..\Base\dxtk.h"
#include "..\..\..\Camera.h"
#include "..\Object.h"

class Bullet : public Object {
public:
	Bullet(int id, ObjectType type,Camera& camera,SimpleMath::Vector2 initPos) : Object(id, type,camera,initPos) {}

	void Initialize() override;

	void Update()override;

	void Render(SpriteManager& sp)override;

	void OnCollisionEnter(Object* other) override;

	void SetDirection(vector<SimpleMath::Vector2>& direction);

private :
	SimpleMath::Vector2 m_direction = SimpleMath::Vector2::Zero;

	float m_velocity = 1.5;

	float m_lifeTime = 2;

};