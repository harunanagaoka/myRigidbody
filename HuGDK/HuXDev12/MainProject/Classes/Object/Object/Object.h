//
// ObjectBase.h
//

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include "..\ObjectRegistry.h"
#include "Transform.h"
#include "..\Collider/Collider.h"
#include "..\..\ResourceManager\SpriteManager.h"
#include "..\..\ResourceManager\SpriteNameRegistry.h"
#include "..\..\Camera.h"

class Object {
public:
	
	Object(int id, ObjectType type,Camera& camera) :m_id(id), m_camera(camera),m_transform(new Transform()), m_objectType(type), m_collider(new Collider) {};
	Object(int id, ObjectType type, Camera& camera, SimpleMath::Vector2 InitPos) :m_id(id), m_camera(camera), m_transform(new Transform()), m_objectType(type), m_collider(new Collider) { m_transform->SetPosition(InitPos); };

	//Object(int id,Camera& camera) :m_id(id),m_camera(camera), m_transform(new Transform()) ,m_collider(new Collider){};
	//Object(int id,SimpleMath::Vector2 initPos,ObjectType type,Camera& camera)
	//	:m_id(id), m_camera(camera), m_transform(new Transform()),m_objectType(type), m_collider(new Collider)
	//{ m_transform->SetPosition(initPos); }

	virtual ~Object() {
		delete m_transform;
		delete m_collider;
	}

	virtual void Initialize() {}

	virtual void Update() {}

	virtual void Render(SpriteManager& sp) {}

	virtual void OnCollisionEnter(Object* other) {}

	virtual void SetDirection(vector<SimpleMath::Vector2>& direction) {};

	void InvisibleCollider() { m_isCollide = false; }

	int GetID() { return m_id; }

	bool IsAttack() { return m_isAttack; }

	void ClearAttackFlag() { m_isAttack = false; }

	bool IsDead() { return m_isDead; }

	Transform* GetTransform() { return m_transform; }

	Collider* GetCollider() { 
		if(m_collider != nullptr)
		return m_collider; 
	}

	ObjectType& GetObjectType() { return m_objectType; }

	

protected:
	int m_id = -1;
	ObjectType m_objectType;
	Transform* m_transform;
	SpriteName m_sprite;
	Collider* m_collider = nullptr;
	Camera& m_camera;

	bool m_isDead = false;
	bool m_isAttack = false;
	bool m_isCollide = true;

	//Sprite vectorîzóÒÇ∆Ç©Ç≈éùÇ¡ÇƒÇ®Ç≠
};