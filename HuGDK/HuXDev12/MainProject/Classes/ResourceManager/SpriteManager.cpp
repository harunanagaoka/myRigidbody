//
// SpriteManager.cpp
//

#include "SpriteManager.h"

using namespace SimpleMath;

//SpriteInfo.defのデータを使い、読み込みを一括で行う関数。
void SpriteManager::CreatSpriteInfo(ResourceUploadBatch& resourceUpload,
	std::unique_ptr<DirectX::DescriptorHeap>& resourceDescriptors)
{
	for (int i = 0; i < SpriteName::DescriptorCount; i++)
	{
		//ファイルパスの文字形式を変換しています
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

		//描画用データをspriteArrayにまとめて格納しています。
			spriteArray.push_back(std::move(spriteData));
	}
}

//enumを引数に設定すると、該当のスプライトデータを返します。
struct SpriteData SpriteManager::GetSprite(int SpriteNum)//SpriteNameで受け取る
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
		nullptr,//画像全体を表示
		XMVectorSet(1.0, 1.0, 1.0, 1.0),//color
		0.0f,
		origin,//中心
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
		nullptr,//画像全体を表示
		XMVectorSet(1.0, 1.0, 1.0, 1.0),//color
		rotation,
		origin,//中心
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
		nullptr,//画像全体を表示
		XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f),//color
		rotation,
		origin,//中心
		XMVectorSet(scale.x, scale.y, 1.0, 0.0f),//スケール
		DirectX::DX12::SpriteEffects_None,
		0.0f
	);
}

