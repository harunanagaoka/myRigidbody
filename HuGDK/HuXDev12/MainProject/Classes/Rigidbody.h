//
// Rigidbody.h
//

#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "Transform.h"

class Rigidbody{
public:
	Rigidbody() { m_transform = new Transform(); }
	
	//Old�ł���Ă������Ȍv�Z������Ȃ��Ȃ邩��
	void Update() { 
		//m_transform->SetPosition(SimpleMath::Vector3(aiu, 0, 0)); 
		}
	const SimpleMath::Matrix& Draw() { return m_transform->GetWorldMatrix(); }

private:
	Transform* m_transform;//�|�C���^�ɂ���K�v���������H
	float aiu = 0.1;
};


