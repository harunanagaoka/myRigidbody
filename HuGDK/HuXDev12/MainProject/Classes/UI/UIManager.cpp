//
// UIManager.h
//

#include "UIManager.h"

void UIManager::UpdateAll(){}

void UIManager::CreateUI(){
	UIBase* ui = UIFactory::CreateUI(SimpleMath::Vector2(100, 100), L"‚ ‚¢‚¤‚¦‚¨");
	m_UIContainar.push_back(ui);
}
void UIManager::RenderAll(){
	FontManager& fm = FontManager::GetInstance();
	for (int i = 0; i < m_UIContainar.size(); i++)
	{
		m_UIContainar[i]->Render(fm);
	}
}