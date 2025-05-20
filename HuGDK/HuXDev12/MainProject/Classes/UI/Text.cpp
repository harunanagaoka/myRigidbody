//
// Text.h
//

#include "Text.h"

void Text::Render(FontManager& fm) {
	fm.Render(m_transform->GetPosition(), m_text);
}