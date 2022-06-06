#pragma once

#include "calc.h"

class ResultFinder : private PersonaCalc
{
public:
    ResultFinder();
    ~ResultFinder() = default;

    bool InitResultFinder(const Gamedata* gd);
    bool SetPersonaList(const PLvlPairVec& p_list, const int& fusion_mode = FusionMode_None);
    void GetResults(PComboVec& outVec);
private:
    struct PLvlPairCombo
    {
        PersonaLevelPair P1, P2, P3;

        PLvlPairCombo(const PersonaLevelPair& p1, const PersonaLevelPair& p2, const PersonaLevelPair& p3 = PersonaLevelPair()) :
            P1(p1), P2(p2), P3(p3) {}

        constexpr bool SameCurrentLevels()
        {
            return P1.Level == P2.Level && P2.Level == P3.Level;
        }

        constexpr void Rearrange()
        {
            if(P1.Level > P2.Level && P1.Level > P3.Level)
            {
                std::swap(P1, P3);
                return;
            }

            if(P2.Level > P3.Level)
                std::swap(P2, P3);
        }
    };

    using PPairComboVec = std::vector<PLvlPairCombo>;

    int             FusionMode;
    PPairComboVec   NormalCombinations;
    PPairComboVec   TriangleCombinations;
};