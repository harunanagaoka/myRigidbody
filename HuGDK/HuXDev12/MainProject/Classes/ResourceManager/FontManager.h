//
// FontManager.h
//

#pragma once

#include<vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "SpriteNameRegistry.h"

using namespace DirectX;
using namespace std;

struct FontData {
	DirectXTK::SpriteFont m_font; 
	SimpleMath::Vector2 m_position;
};

class FontManager {
private:
	FontManager() {}
	std::vector<std::unique_ptr<FontData>> fontArray; //spriteÇÃäeéÌèÓïÒÇä«óù
	DirectX::SpriteBatch* m_spriteBatch = nullptr;
public:
	static FontManager& GetInstance() {
		static FontManager instance;
		return instance;
	}

	FontManager(const FontManager&) = delete;
	FontManager& operator=(const FontManager&) = delete;

	//CreateSpriteSRVÅ®SpriteDataÇ…äiî[
	void CreatFontsInfo(ResourceUploadBatch& resourceUpload,
		std::unique_ptr<DirectX::DescriptorHeap>& resourceDescriptors);

	struct std::unique_ptr<FontData>& GetFont(int FontNum);

	void Render(SimpleMath::Vector2 position,wstring text);
	void SetSpriteBatch(DirectX::SpriteBatch* sp) { m_spriteBatch = sp; }
	DirectX::SpriteBatch* GetSpriteBatch() { return m_spriteBatch; }
protected:
	std::vector<string> m_spritePath = {
	#define ITEM(name,adress)adress,
	#include "defineFile/FontInfo.def"
	#undef ITEM
	};
};
