//
// UIBase.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "..\Object\Object\Transform.h"
#include "..\Object\Collider\Collider.h"
#include "..\ResourceManager\FontManager.h"


class UIBase {
public:
public:
	UIBase() :m_transform(new Transform()) {}
	UIBase(SimpleMath::Vector2 initpos,wstring text) :m_transform(new Transform()) ,m_text(text){ m_transform->SetPosition(initpos); }

	virtual ~UIBase() {
		delete m_transform;

		if(m_collider != nullptr)
		delete m_collider;
	}

	virtual void Initialize() {}

	virtual void Update() {}

	virtual void Render(FontManager& fm) {}

	void SetCollider() { m_collider = new Collider(); }

	wstring m_text;

	Transform* m_transform;

	Collider* m_collider = nullptr;
	
	bool isDelete = false;
};