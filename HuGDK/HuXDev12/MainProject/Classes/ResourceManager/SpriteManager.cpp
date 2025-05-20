//
// SpriteManager.cpp
//

#include "SpriteManager.h"

using namespace SimpleMath;

//SpriteInfo.def�̃f�[�^���g���A�ǂݍ��݂��ꊇ�ōs���֐��B
void SpriteManager::CreatSpriteInfo(ResourceUploadBatch& resourceUpload,
	std::unique_ptr<DirectX::DescriptorHeap>& resourceDescriptors)
{
	for (int i = 0; i < SpriteName::DescriptorCount; i++)
	{
		//�t�@�C���p�X�̕����`����ϊ����Ă��܂�
		string FilePath = m_spritePath[i];
		wstring wstringPath(FilePath.begin(), FilePath.end());
		const wchar_t* spriteFilePath = wstringPath.c_str();

		SpriteData spriteData;

		spriteData.m_sprite = DirectXTK::CreateSpriteSRV(
			DXTK->Device,
			spriteFilePath,
			resourceUpload,
			resourceDescriptors,
			static_cast<int>(static_cast<SpriteName>(i)));

		//�`��p�f�[�^��spriteArray�ɂ܂Ƃ߂Ċi�[���Ă��܂��B
			spriteArray.push_back(std::move(spriteData));
	}
}

//enum�������ɐݒ肷��ƁA�Y���̃X�v���C�g�f�[�^��Ԃ��܂��B
struct SpriteData SpriteManager::GetSprite(int SpriteNum)//SpriteName�Ŏ󂯎��
{
	return spriteArray[SpriteNum];
}

void SpriteManager::Render(SpriteName spriteName, SimpleMath::Vector2 position)
{
	SpriteData spd = GetSprite(spriteName);
	float centerX = spd.m_sprite.size.x / 2;
	float centerY = spd.m_sprite.size.y / 2;
	FXMVECTOR origin = XMVectorSet(centerX, centerY, 0.0f, 0.0f);

	m_spriteBatch->Draw(
		spd.m_sprite.handle,
		spd.m_sprite.size,
		position,
		nullptr,//�摜�S�̂�\��
		XMVectorSet(1.0, 1.0, 1.0, 1.0),//color
		0.0f,
		origin,//���S
		XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		DirectX::DX12::SpriteEffects_None,
		0.0f
	);
}

void SpriteManager::Render(SpriteName spriteName, SimpleMath::Vector2 position, float rotation) {
	SpriteData spd = GetSprite(spriteName);
	float centerX = spd.m_sprite.size.x / 2;
	float centerY = spd.m_sprite.size.y / 2;

	FXMVECTOR origin = XMVectorSet(centerX, centerY, 0.0f, 0.0f);
	m_spriteBatch->Draw(
		spd.m_sprite.handle,
		spd.m_sprite.size,
		position,
		nullptr,//�摜�S�̂�\��
		XMVectorSet(1.0, 1.0, 1.0, 1.0),//color
		rotation,
		origin,//���S
		XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		DirectX::DX12::SpriteEffects_None,
		0.0f
	);

}

void SpriteManager::Render(SpriteName spriteName, SimpleMath::Vector2 position, float rotation, SimpleMath::Vector2 scale) {
	SpriteData spd = GetSprite(spriteName);
	float centerX = spd.m_sprite.size.x / 2;
	float centerY = spd.m_sprite.size.y / 2;
	FXMVECTOR origin = XMVectorSet(centerX, centerY, 0.0f, 0.0f);

	m_spriteBatch->Draw(
		spd.m_sprite.handle,
		spd.m_sprite.size,
		position,
		nullptr,//�摜�S�̂�\��
		XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f),//color
		rotation,
		origin,//���S
		XMVectorSet(scale.x, scale.y, 1.0, 0.0f),//�X�P�[��
		DirectX::DX12::SpriteEffects_None,
		0.0f
	);
}

