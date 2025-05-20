//
// SpriteManager.h
//
//�摜�ǂݍ��ݗp�̃N���X�ł�

#pragma once

#include<vector>
#include <string>
#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "SpriteNameRegistry.h"

using namespace DirectX;
using namespace std;

struct SpriteData {//�摜�ǂݍ��݌�ɕۑ����Ă����\����
	DirectXTK::Sprite m_sprite; 
};

class SpriteManager  {
private:
	SpriteManager(){}
	
	std::vector<string> m_spritePath = {//SpriteInfo.def����摜�̃t�@�C���p�X�擾
	#define ITEM(name,adress)adress,
	#include "defineFile/SpriteInfo.def"
	#undef ITEM
	};

	std::vector<SpriteData> spriteArray; //�SSpriteData���Ǘ����Ă��܂��B

	DirectX::SpriteBatch* m_spriteBatch = nullptr;
public:
	//�O������擾�ł���悤�ɂ��Ă��܂�
	static SpriteManager& GetInstance() {
		static SpriteManager instance; 
		return instance;
	}

	//�R�s�[�֎~
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