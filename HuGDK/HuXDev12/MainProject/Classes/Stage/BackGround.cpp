//
// BackGround.cpp
//

#include "BackGround.h"

void BackGround::Initialize() {};


void BackGround::Update() {

	static float time;
	time += DXTK->Time.deltaTime; // �o�ߎ���
	float wobbleX = sin(time * 0.5f) * 2.0f; // 0.5Hz�̂�����肵���h��A�}2px
	float wobbleY = cos(time * 0.3f) * 2.0f; // �ʂ̎��g���ŃY����

	//float finalOffsetX = baseOffsetX + wobbleX;
	//float finalOffsetY = baseOffsetY + wobbleY;

	//drawTile(-finalOffsetX, -finalOffsetY);
}

void BackGround::Render(SpriteManager& sp) {
	//sp.Render()
}