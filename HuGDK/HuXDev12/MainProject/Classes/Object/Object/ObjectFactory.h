//
// ƒNƒ‰ƒX‚Ð‚È‚ª‚½.h
//

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include "..\..\Camera.h"
#include "Object.h"
#include "..\ObjectRegistry.h"
#include "Player/Player.h"
#include "Enemy/Enemy.h"
#include "Bullet/Bullet.h"

class ObjectFactory final {
public:
	static Object* CreateObject(const ObjectType type, const int id,Camera& camera)
	{
		Object* obj = nullptr;
		switch (type) {
		case ObjectType::Player: obj = new Player(id, type,camera); break;
		case ObjectType::Enemy: obj = new Enemy(id, type, camera); break;
		}
		return obj;
	}

	static Object* CreateBullet(const int id, Camera& camera, SimpleMath::Vector2 initPos) {
		Object* obj = nullptr;
		ObjectType type = ObjectType::PlayerBullet;
		obj = new Bullet(id,type,camera,initPos);
		return obj;
	}

};
