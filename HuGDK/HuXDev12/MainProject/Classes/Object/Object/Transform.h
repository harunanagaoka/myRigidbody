//
// Transform.h
//

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"

class Transform {
public:
	void SetPosition(const SimpleMath::Vector2& pos) { m_worldPosition = pos; };
	void SetScreenPosition(const SimpleMath::Vector2& pos) { m_screenPosition = pos; };

	const SimpleMath::Vector2& GetPosition() const { return m_worldPosition; }
	const SimpleMath::Vector2& GetScreenPosition() const { return m_screenPosition; }
	const float GetRotation() const { return m_rotation; }
	const SimpleMath::Vector2& GetScale() const { return m_scale; }

private:

	float m_rotation = 0;
	SimpleMath::Vector2 m_scale = SimpleMath::Vector2::One;

	SimpleMath::Vector2 m_worldPosition = SimpleMath::Vector2::Zero;
	SimpleMath::Vector2 m_screenPosition = SimpleMath::Vector2::Zero;
};