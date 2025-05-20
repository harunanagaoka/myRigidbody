//
// Player.h
//

#pragma once

#include "..\..\..\..\Base\pch.h"
#include "..\..\..\..\Base\dxtk.h"
#include "..\..\..\ResourceManager\SpriteManager.h"
#include "..\Object.h"
#include "..\..\..\Camera.h"

class Player : public Object {
public:
	Player(int id, ObjectType type,Camera& camera) : Object(id, type,camera) {}

	void Initialize() override;

	void Update()override;

	void Render(SpriteManager& sp)override;

	void OnCollisionEnter(Object* other) override;

	void Move();

private:
	float m_velocity = 1.0f;
	int m_hp = 10;
};