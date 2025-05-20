//
// CircleCollider.h
//

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include "Collider.h"

class CircleCollider :public Collider{
public:

	void Update(SimpleMath::Vector2 position)override;
	float GetRadius() { return m_radius; }
	SimpleMath::Vector2 GetCenter() { return m_position; };

private:

	float m_radius = 1;

};
