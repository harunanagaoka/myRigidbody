//
// Text.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "..\ResourceManager\FontManager.h"
#include "UIBase.h"

class Text : public UIBase {
public:
	Text(SimpleMath::Vector2 initpos, wstring text) :UIBase(initpos, text) {};

	void Render(FontManager& fm) override;
};