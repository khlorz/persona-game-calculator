#include "ResultFinder.h"

ResultFinder::ResultFinder()
{
    GD = nullptr;
    NormalCombinations.reserve(120);
    TriangleCombinations.reserve(220);
}

bool ResultFinder::InitResultFinder(const Gamedata* gd)
{
    return InitCalc(gd);
}

bool ResultFinder::SetPersonaList(const PLvlPairVec& p_list, const int& fusion_mode)
{
    if (!CalcInitialized)
        return false;

    FusionMode = fusion_mode;
    NormalCombinations.clear();
    TriangleCombinations.clear();
    const int& p_size = p_list.size();

    if (FusionMode < FusionMode_None || FusionMode > FusionMode_Triangle)
        return false;

    if(FusionMode & (FusionMode_None | FusionMode_Normal))
        for (int i = 0; i < p_size; i++)
            for (int j = i + 1; j < p_size; j++)
                NormalCombinations.emplace_back( p_list[i], p_list[j] );

    if(FusionMode & (FusionMode_None | FusionMode_Triangle))
    {
        for (int i = 0; i + 2 < p_size; i++)
            for (int j = i + 1; j < p_size; j++)
                for (int k = j + 1; k < p_size; k++)
                    TriangleCombinations.emplace_back( p_list[i], p_list[j], p_list[k] );

        for(auto& T : TriangleCombinations)
            T.Rearrange();
    }

    return true;
}

void ResultFinder::GetResults(PComboVec& outVec)
{
    int result = CalcError_NoResult;

    if(FusionMode & (FusionMode_None | FusionMode_Normal))
    {
        outVec.reserve(NormalCombinations.size());

        for (const auto& N : NormalCombinations)
        {
            result = CalculatePersona(FusionMode_Normal, N.P1.Persona, N.P2.Persona);

            if(result >= 0)
                outVec.emplace_back( N.P1.Persona, N.P2.Persona, -1, result );
        }
    }

    if(FusionMode & (FusionMode_None | FusionMode_Triangle))
    {
        outVec.reserve(outVec.size() + TriangleCombinations.size());

        for (auto& T : TriangleCombinations)
        {
            result = CalculatePersona(FusionMode_Triangle, T.P1.Persona, T.P2.Persona, T.P3.Persona, T.SameCurrentLevels());

            if(result >= 0)
                outVec.emplace_back( T.P1.Persona, T.P2.Persona, T.P3.Persona, result);
        }
    }
}