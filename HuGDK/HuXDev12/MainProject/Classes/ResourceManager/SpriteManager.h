//
// SpriteManager.h
//
//画像読み込み用のクラスです

#pragma once

#include<vector>
#include <string>
#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "SpriteNameRegistry.h"

using namespace DirectX;
using namespace std;

struct SpriteData {//画像読み込み後に保存しておく構造体
	DirectXTK::Sprite m_sprite; 
};

class SpriteManager  {
private:
	SpriteManager(){}
	
	std::vector<string> m_spritePath = {//SpriteInfo.defから画像のファイルパス取得
	#define ITEM(name,adress)adress,
	#include "defineFile/SpriteInfo.def"
	#undef ITEM
	};

	std::vector<SpriteData> spriteArray; //全SpriteDataを管理しています。

	DirectX::SpriteBatch* m_spriteBatch = nullptr;
public:
	//外部から取得できるようにしています
	static SpriteManager& GetInstance() {
		static SpriteManager instance; 
		return instance;
	}

	//コピー禁止
	SpriteManager(const SpriteManager&) = delete;
	SpriteManager& operator=(const SpriteManager&) = delete;


	void CreatSpriteInfo(ResourceUploadBatch& resourceUpload,
						 std::unique_ptr<DirectX::DescriptorHeap>& resourceDescriptors);

	struct SpriteData GetSprite(int SpriteNum);

	void Render(SpriteName spriteName, SimpleMath::Vector2 position);
	void Render(SpriteName spriteName, SimpleMath::Vector2 position,float rotation);
	void Render(SpriteName spriteName, SimpleMath::Vector2 position, float rotation,SimpleMath::Vector2 scale);

	void SetSpriteBatch(DirectX::SpriteBatch* sp) {
		m_spriteBatch = sp; 
	}

	DirectX::SpriteBatch* GetSpriteBatch() {
		return m_spriteBatch; 
	}


};