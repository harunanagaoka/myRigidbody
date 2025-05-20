//
// Camera.h
//

#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "Object/Object/Transform.h"
#include <unordered_map>

using namespace std;


class Camera {
public:
	Camera(){
		m_screenCenter = SimpleMath::Vector2(DXTK->SwapChain.Viewport.Width / 2,
						 DXTK->SwapChain.Viewport.Height / 2);
	}
	void Initialize(){}
	void Update();

	void SetCameraPos(const SimpleMath::Vector2& pos) { m_cameraPosition = pos; }
	const SimpleMath::Vector2 GetCameraPos() const { return m_cameraPosition; }

	const SimpleMath::Vector2 GetScreenPos(int id);
	void UpdateScreenPos();

	bool SetCamera(int id, Transform* transform) { m_transforms[id] = transform; return true; }
	bool SetFollowingCamera(int id) {/*もし登録があったら*/m_followingObjId = id; return true; }

private:
	SimpleMath::Vector2 m_cameraPosition = SimpleMath::Vector2::Zero;
	unordered_map<int, Transform*> m_transforms;
	int m_followingObjId = -1;
	SimpleMath::Vector2 m_screenCenter = SimpleMath::Vector2::Zero;
};
//SetCameraで各オブジェクトからTransformをセット
//毎フレームTransformを更新