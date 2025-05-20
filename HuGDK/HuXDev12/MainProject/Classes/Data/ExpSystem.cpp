//
// ExpSystem.cpp
//

#include "ExpSystem.h"

void ExpSystem::UpdateLevel() {
	while (m_TotalExp >= m_next) {
		m_level += 1;
		m_next += 5 * m_level;
	}
} 