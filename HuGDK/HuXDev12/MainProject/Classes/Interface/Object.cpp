#include "Object.h"

//スマートポインタにするかは後ほどきめる

void Object::AddTransform() {
	if (m_transform == nullptr)
	{
		m_transform = new Transform();
	}
}

void Object::AddRigidbody() {
	if (m_rigidbody == nullptr)
	{
		m_rigidbody = new Rigidbody(m_transform,false,true);
	}
}

void Object::AddStaticRigidbody() {
	if (m_rigidbody == nullptr)
	{
		m_rigidbody = new Rigidbody(m_transform, true, false);
	}
}

void Object::AddBoxCollider() {
	if (m_collider == nullptr)
	{
		m_collider = new BoxCollider();
	}
}

