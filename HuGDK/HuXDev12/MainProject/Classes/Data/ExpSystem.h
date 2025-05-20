//
// ExpSystem.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"

class ExpSystem {
public:
	void AddExperience(int points) { m_TotalExp += points; }
	void UpdateLevel();

private:
	int m_level = 1;
	int m_TotalExp = 0;
	int m_next = 5;
};