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
	
	//Oldでやってたいろんな計算がいらなくなるかも
	void Update() { 
		//m_transform->SetPosition(SimpleMath::Vector3(aiu, 0, 0)); 
		}
	const SimpleMath::Matrix& Draw() { return m_transform->GetWorldMatrix(); }

private:
	Transform* m_transform;//ポインタにする必要無いかも？
	float aiu = 0.1;
};


