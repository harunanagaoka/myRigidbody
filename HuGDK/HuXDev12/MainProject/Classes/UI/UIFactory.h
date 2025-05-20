//
// UIFactory.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "UIBase.h"
#include "Text.h"

class UIFactory final {
public:
	static UIBase* CreateUI( const SimpleMath::Vector2 initPos,const wstring text)
	{
		UIBase* ui = nullptr;
		ui = new Text(initPos,text);
		return ui;
	}
};
