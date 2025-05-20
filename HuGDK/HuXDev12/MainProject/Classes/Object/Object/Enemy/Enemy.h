//
// Enemy.h
//

#pragma once

#include "..\..\..\..\Base\pch.h"
#include "..\..\..\..\Base\dxtk.h"
#include "..\Object.h"
#include "..\..\..\Camera.h"

class Enemy : public Object {
public:
	Enemy(int id, ObjectType type,Camera& camera) : Object(id, type,camera) {}

	void Initialize() override;

	void Update()override;

	void Render(SpriteManager& sp)override;

	void OnCollisionEnter(Object* other) override;

private :
	int m_hp = 10;
};