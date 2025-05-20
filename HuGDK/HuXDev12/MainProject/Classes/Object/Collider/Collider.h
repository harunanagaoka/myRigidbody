//
// Collider.h
//

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"

enum class ColliderType {
	Circle
};

class Collider {
public:
	Collider(){}
	virtual void Update(SimpleMath::Vector2 position) { m_position = position; };//transformposition�Ƃ����󂯎��
	ColliderType GetType() { return m_type; }

protected:
	SimpleMath::Vector2 m_position = SimpleMath::Vector2::Zero;
	ColliderType m_type = ColliderType::Circle;//��{�̃R���C�_�[���~�ɂ��Ă��܂�
};

