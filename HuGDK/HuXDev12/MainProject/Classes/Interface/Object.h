//
// Object.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "..\Transform.h"
#include "..\Rigidbody.h"
#include "..\Collision\BoxCollider.h"

class Object {
public:
	Object(int id) :m_id(id){};
	~Object() {
		delete m_transform;
		delete m_rigidbody;
		delete m_collider;
	}

	bool isTransformDirty() {	return m_transform && m_transform->isDirty();}

	void AddTransform();

	void AddRigidbody();

	void AddStaticRigidbody();

	void AddBoxCollider();

	int GetID() { return m_id; }

	Transform* GetTransform() { return m_transform; }

	Rigidbody* GetRigidbody() { return m_rigidbody; }

	BoxCollider* GetCollider() { return m_collider; }
private:
	int m_id;

	Transform* m_transform = nullptr;

	Rigidbody* m_rigidbody = nullptr;

	BoxCollider* m_collider = nullptr;
};
