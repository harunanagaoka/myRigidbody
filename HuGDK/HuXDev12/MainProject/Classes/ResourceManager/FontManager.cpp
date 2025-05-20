//
// FontManager.cpp
//

#include "FontManager.h"

using namespace SimpleMath;

void FontManager::CreatFontsInfo(ResourceUploadBatch& resourceUpload,
	std::unique_ptr<DirectX::DescriptorHeap>& resourceDescriptors)
{
	for (int i = 0; i < FontName::FontsCount; i++)
	{
		string FilePath = m_spritePath[i];
		wstring wstringPath(FilePath.begin(), FilePath.end());
		const wchar_t* spriteFilePath = wstringPath.c_str();//Œ^•ÏŠ·

		auto fontData = std::make_unique<FontData>();

		fontData->m_font = DirectXTK::CreateSpriteFont(
			DXTK->Device,
			spriteFilePath,
			resourceUpload,
			resourceDescriptors,
			SpriteName::DescriptorCount + i);
		fontArray.push_back(std::move(fontData));
	}
	

}

struct std::unique_ptr<FontData>& FontManager::GetFont(int FontNum)
{
	return fontArray[FontNum];
}

void FontManager::Render(SimpleMath::Vector2 position,wstring text)
{
	struct std::unique_ptr<FontData>& font = GetFont(FontName::msgothic);
	font.get()->m_font.get()->DrawString(
		m_spriteBatch,
		text.c_str(),
		position,
		DirectX::Colors::Black,
		0.0f,
		XMFLOAT2(0.0f, 0.0f),
		1.0f
	);
}





