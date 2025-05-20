//
// BonusSystem.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"

using namespace std;

class BonusSystem {
public:


private:
    struct BonusOption {
        wstring name;
        function<void()> applyEffect;
    };

    std::vector<BonusOption> m_allBonuses;
};

/*std::vector<BonusOption> allBonuses = {
    { "HP +20", []() { player.hp += 20; } },
    { "�ړ����x +10%", []() { player.speed *= 1.1f; } },
    { "�U���� +5", []() { player.attack += 5; } },//�ώ��ɂ�����
};
*/
/*std::vector<BonusOption> GetRandomBonuses(int count) {�����_���Ő��I��
    std::vector<BonusOption> shuffled = allBonuses;
    std::shuffle(shuffled.begin(), shuffled.end(), std::mt19937{std::random_device{}()});
    shuffled.resize(count);
    return shuffled;
}

*/