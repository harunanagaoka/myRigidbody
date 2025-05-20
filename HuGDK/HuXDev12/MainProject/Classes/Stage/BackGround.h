//
// BackGround.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "..\ResourceManager/SpriteManager.h"
#include "..\Camera.h"
#include "..\Object/Object/Transform.h"

class BackGround {
public:
	BackGround(Camera& camera):m_camera(camera),m_transform(new Transform()){}
	~BackGround(){
		delete m_transform;
	}
	void Initialize();
	void Update();
	void Render(SpriteManager& sp);

	
private:

	Transform* m_transform;
	Camera& m_camera;
};