//
// UIManager.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "..\ResourceManager\FontManager.h"
#include "UIBase.h"
#include "UIFactory.h"
#include <unordered_map>

using namespace std;

class UIManager {
public:

	void UpdateAll();
	void CreateUI();
	void RenderAll();

private:
	vector<UIBase*> m_UIContainar;

};