//
// Camera.cpp
//

#include "Camera.h"

void Camera::Update() {

	if (m_followingObjId != -1)
	{
		m_cameraPosition = m_transforms[m_followingObjId]->GetPosition();
	}
	UpdateScreenPos();
}


void Camera::UpdateScreenPos() {
	
	for (auto& pair : m_transforms) {

		if (pair.second != nullptr) {
			SimpleMath::Vector2 pos = pair.second->GetPosition();//worldPos‚ðŽæ“¾
			SimpleMath::Vector2 screenPos = pos - m_cameraPosition + m_screenCenter;
			pair.second->SetScreenPosition(screenPos);//screenpos‚ðƒZƒbƒg
		}
	}
}

const SimpleMath::Vector2 Camera::GetScreenPos(int id) {
	if (m_transforms[id] != nullptr)
	{
		UpdateScreenPos();
	}
	return SimpleMath::Vector2::Zero;
}