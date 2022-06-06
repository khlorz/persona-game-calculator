#include "ReverseCalc.h"
#include <algorithm>

ReverseCalc::ReverseCalc() :
    r_ComboSize(0),
    r_personaSet(false),
    r_stopMakeRcp(false),
    r_pauseMakeRcp(false),
    r_isPersonaSpecial(false)
{
    GD = nullptr;
    CalcInitialized = false;
}

ReverseCalc::~ReverseCalc()
{
    if (IsOngoing())
        Stop();
}

bool ReverseCalc::InitReverseCalc(const Gamedata* data)
{
    if (!InitCalc(data))
        return false;

    SetNoProcess();

    return true;
}

bool ReverseCalc::SetReversePersona(const std::string& inputPersona, const int& fusion_mode)
{
    int index = GD->GetPersonaIndex(inputPersona);
    return SetReversePersona(index, fusion_mode);
}

bool ReverseCalc::SetReversePersona(const int& p_index, const int& fusion_mode)
{
    if (p_index < 0 || p_index >= GD->Personas.size())
    {
        SetNoProcess();
        return false;
    }
    
    r_currentIteration  = 0;
    r_personaSet        = true;
    r_pauseMakeRcp      = false;
    r_stopMakeRcp       = false;
    r_Index             = p_index;
    r_fusionMode        = fusion_mode;
    r_Persona           = GD->Personas[p_index].Name.c_str();
    r_Arcana            = GD->Personas[p_index].Arcana.c_str();
    r_isPersonaSpecial  = GD->Personas[p_index].Special;

    if (r_isPersonaSpecial)
    {
        r_ComboSize = 1;
        return true;
    }

    SetUpperLowerLevel();

    switch (r_fusionMode)
    {
        case FusionMode_Normal:   GD->GetNormArcanaCombos(r_Arcana, r_mainCombos); r_ComboSize = r_mainCombos.size(); break;
        case FusionMode_Triangle: GD->GetTriArcanaCombos(r_Arcana, r_mainCombos);  r_ComboSize = r_mainCombos.size(); break;
        default: SetNoProcess(); return false;
    }

    if (r_ComboSize == 0)
        SetNoProcess();

    return true;
}

void ReverseCalc::SetUpperLowerLevel()
{
    const int& p_size = GD->Personas.size();

    //---------------------------
    // SET UPPER LEVEL
    //---------------------------
    if (r_Index == p_size - 1 || GD->Personas[r_Index + 1].Arcana != r_Arcana)
    {
        r_upLvl = 100;
    }
    else if (GD->Personas[r_Index + 1].Special)
    {
        int  i = r_Index + 1;
        bool max_sp = false;
        do
        {
            if (i == p_size - 1)
            {
                max_sp = GD->Personas[i].Special;
                break;
            }

            i++;
        } while (GD->Personas[i].Special && GD->Personas[i].Arcana == r_Arcana);

        r_upLvl = GD->Personas[i].Arcana == r_Arcana ? ( max_sp ? 100 : GD->Personas[r_Index].Level ) : 100;
    }
    else
    {
        r_upLvl = GD->Personas[r_Index].Level;
    }

    //---------------------------
    // SET LOWER LEVEL
    //---------------------------
    if (r_Index == 0 || GD->Personas[r_Index - 1].Arcana != GD->Personas[r_Index].Arcana)
    {
        r_lowLvl = 0;
    }
    else if (GD->Personas[r_Index - 1].Special)
    {
        int i = r_Index - 1;
        bool min_sp = false;
        do
        {
            if (i == 0)
            {
                min_sp = GD->Personas[i].Special;
                break;
            }

            i--;
        } while (GD->Personas[i].Special && GD->Personas[i].Arcana == GD->Personas[r_Index].Arcana);

        r_lowLvl = GD->Personas[i].Arcana == GD->Personas[r_Index].Arcana ? ( min_sp ? 0 : GD->Personas[i].Level ) : 0;
    }
    else
    {
        r_lowLvl = GD->Personas[r_Index - 1].Level;
    }
}

bool ReverseCalc::ReverseFusion(PComboVec& n_result, SpecialData& sp_result)
{
    if (!CalcInitialized)
        return false;

    if (r_stopMakeRcp || r_pauseMakeRcp || !r_personaSet)
        return false;

    if (r_isPersonaSpecial)
    {
        ReverseSpecialFusion(sp_result);
        Stop();
        return true;
    }

    switch (r_fusionMode)
    {
        case FusionMode_Normal:   ReverseNormalFusion(n_result);   break;
        case FusionMode_Triangle: ReverseTriangleFusion(n_result); break;
        default: return false;
    }

    r_currentIteration++;
    if (r_currentIteration == r_ComboSize)
        Stop();

    return true;
}

void ReverseCalc::ReverseSpecialFusion(SpecialData& outVec) const
{
    for (auto& SpFusion : GD->SpFusionData)
    {
        if (r_Persona == SpFusion.ResultantPersona)
        {
            outVec.PersonaCombos = SpFusion.PersonaCombos;
            break;
        }
    }
}

//---------------------------------------------------------------------------------------------
// REVERSE NORMAL FUSION
//---------------------------------------------------------------------------------------------

void ReverseCalc::ReverseNormalFusion(PComboVec& outVec)
{
    const size_t& p_size = GD->Personas.size();
    // -----------------------------------------------------------------------------------------------------------------
    // SAME ARCANA FUSION
    //  - The loop starts at the first instance of the arcana of the reverse persona. Filter out the indices that are
    //  the same as the reverse persona's index. The main reason for inheriting the PersonaCalc would be for this
    //  reason, to calculate the same persona fusion because it would still be the same algorithm for finding
    //  the correct combinations for the reverse persona.
    //  - In normal fusion, there is no combination where one of the persona's arcana is the same as the output persona
    //  unless it's the same arcana fusion. That's why we early exit after the loop is done.
    // -----------------------------------------------------------------------------------------------------------------

    if (r_mainCombos[r_currentIteration].Arcana1 == r_mainCombos[r_currentIteration].Arcana2)
    {
        const int& starting_point = GD->GetArcanaIndex(r_Arcana);

        for (int p1 = starting_point; p1 < p_size && GD->Personas[p1].Arcana == r_Arcana; p1++)
        {
            if (p1 != r_Index)
            {
                for (int p2 = p1 + 1; p2 < p_size && GD->Personas[p2].Arcana == r_Arcana; p2++)
                {
                    if (p2 != r_Index)
                        if (r_Index == CalculatePersona(FusionMode_Normal, p1, p2))
                                outVec.emplace_back( p1 , p2 );
                }
            }
        }

        return;
    }

    //------------------------------------------------------------------------------------------------------------------------------------
    // CREATING THE POSSIBLE COMBINATIONS:
    //  - Create a vector of possible combinations based on the arcana, regardless if it's the correct combination or not.
    //  Filter out combinations if the two indices are the same or at least one of them has the same index as the input persona's index
    //------------------------------------------------------------------------------------------------------------------------------------

    PComboVec PossibleCombo; PossibleCombo.reserve(144);
    const std::string& A1 = r_mainCombos[r_currentIteration].Arcana1;
    const std::string& A2 = r_mainCombos[r_currentIteration].Arcana2;
    const int& p1_start   = GD->GetArcanaIndex(A1);
    const int& p2_start   = GD->GetArcanaIndex(A2);

    for (int p1 = p1_start; p1 < p_size && GD->Personas[p1].Arcana == A1; p1++)
    {
        if (p1 != r_Index)
        {
            for (int p2 = p2_start; p2 < p_size && GD->Personas[p2].Arcana == A2; p2++)
            {
                if (p2 != r_Index && p1 != p2)
                    PossibleCombo.emplace_back(p1, p2);
            }
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------------------
    // FILTERING FOR THE CORRECT COMBINATIONS:
    //  - This part is where the loop inserts the correct combination to the outVec by solving the resulting possible level for
    //  Normal Fusion. It should be higher than the computed r_lowLvl and lower or equal to the r_upLvl.
    //  - Another thing to filter out would be if the combination is a combination for a special persona since we did filter that
    //  earlier as that's more time consuming if done earlier
    //-------------------------------------------------------------------------------------------------------------------------------------

    for (const auto& P : PossibleCombo)
    {
        if (((GD->Personas[P.P1].Level + GD->Personas[P.P2].Level) / 2) + 1 > r_lowLvl && ((GD->Personas[P.P1].Level + GD->Personas[P.P2].Level) / 2) + 1 <= r_upLvl)
        {
            if (std::find(SpecialNormals.begin(), SpecialNormals.end(), P) == SpecialNormals.end())
                outVec.emplace_back(P);
        }
    }
}


//-------------------------------------------------------------------------------------------------
// REVERSE TRIANGLE FUSION
//-------------------------------------------------------------------------------------------------

void ReverseCalc::ReverseTriangleFusion(PComboVec& outVec)
{
    const size_t& p_size = GD->Personas.size();
    // -----------------------------------------------------------------------------------------------------------------
    // SAME ARCANA FUSION
    //  - The process here would be the same as the normal fusion one except for one thing: no early exit.
    //  In triangle fusion, there can only be one persona that has the same arcana as the output persona due to the
    //  nature of having the "Third Persona". The reason
    // -----------------------------------------------------------------------------------------------------------------

    if (r_mainCombos[r_currentIteration].Arcana1 == r_mainCombos[r_currentIteration].Arcana2)
    {
        int  start = GD->GetArcanaIndex(r_Arcana);
        for (int p1 = start; p1 < p_size && GD->Personas[p1].Arcana == r_Arcana; p1++)
        {
            if (p1 != r_Index)
            {
                for (int p2 = p1 + 1; p2 < p_size && GD->Personas[p2].Arcana == r_Arcana; p2++)
                {
                    if (p2 != r_Index)
                    {
                        for (int p3 = p2 + 1; p3 < p_size && GD->Personas[p3].Arcana == r_Arcana; p3++)
                        {
                            if (p3 != r_Index)
                                if (r_Index == CalculatePersona(FusionMode_Triangle, p1, p2, p3))
                                    outVec.emplace_back( p1, p2, p3 );
                        }
                    }
                }
            }

        }
    }

    //---------------------------------------------------------------------------------------------------------
    // CREATING POSSIBLE COMBINATIONS: [TriPossibleCombos]
    //  - Compared to creating the possible combinations in normal fusion, this has an extra step of doing
    //  the loop twice because the two arcana combinations are not combinations but permutations so A1 + A2
    //  is not the same as A2 + A1 when it comes to triangle fusion.
    //---------------------------------------------------------------------------------------------------------

    // <* The normal arcana results from normal fusion for the first two personas
    FusionVec arc1; GD->GetNormArcanaCombos(r_mainCombos[r_currentIteration].Arcana1.c_str(), arc1);
    FusionVec arc2; GD->GetNormArcanaCombos(r_mainCombos[r_currentIteration].Arcana2.c_str(), arc2);

    // <* Vector of indices that contains the normal fusion arcana combos of Arcana 1
    IntVec parr1_1; parr1_1.reserve(144);
    IntVec parr2_1; parr2_1.reserve(144);

    // <* Vector of indices that contains the normal fusion arcana combos of Arcana 2
    IntVec parr1_2; parr1_2.reserve(144);
    IntVec parr2_2; parr2_2.reserve(144);  

    TriPossibleCombos(arc1, parr1_1, parr2_1);
    TriPossibleCombos(arc2, parr1_2, parr2_2);

    //------------------------------------------------------------------------------------------------------
    // FILTERING FOR THE CORRECT COMBINATIONS: [TriCorrectCombos]
    //  - Almost the same as the normal fusion but with the additional third persona. 
    //  - Skip the second loop after the first loop if the current combo is the same arcana, because it
    //  would just create a needless duplicate.
    //------------------------------------------------------------------------------------------------------

    TriCorrectCombos(parr1_1, parr2_1, r_mainCombos[r_currentIteration].Arcana2, outVec);

    if (r_mainCombos[r_currentIteration].Arcana2 == r_mainCombos[r_currentIteration].Arcana1)
        return;

    TriCorrectCombos(parr1_2, parr2_2, r_mainCombos[r_currentIteration].Arcana1, outVec);
}

void ReverseCalc::TriPossibleCombos(const FusionVec& _A, IntVec& outVec1, IntVec& outVec2) const
{
    const int& pSize = GD->Personas.size();

    for (const FusionData& A_Combo : _A)
    {
        if (A_Combo.Arcana1 != r_Arcana || A_Combo.Arcana2 != r_Arcana)
        {
            int start_1 = GD->GetArcanaIndex(A_Combo.Arcana1);
            int start_2 = GD->GetArcanaIndex(A_Combo.Arcana2);

            

            for (int j = start_1; j < pSize && GD->Personas[j].Arcana == A_Combo.Arcana1; j++)
            {
                if (j != r_Index)
                {
                    if (A_Combo.Arcana1 == A_Combo.Arcana2)
                        start_2 = j + 1;

                    for (int k = start_2; k < pSize && GD->Personas[k].Arcana == A_Combo.Arcana2; k++)
                    {
                        if (k != j && k != r_Index)
                        {
                            outVec1.emplace_back(j);
                            outVec2.emplace_back(k);
                        }
                    }
                }
            }
        }
    }
}

void ReverseCalc::TriCorrectCombos(const IntVec& inVec1, const IntVec& inVec2, const std::string& thirdArcana, PComboVec& outVec) const
{
    const int& start  = GD->GetArcanaIndex(thirdArcana);
    const int& p_size = GD->Personas.size();

    for (int i = 0; i < inVec1.size(); i++)
    {
        for (int p3 = start; p3 < p_size && GD->Personas[p3].Arcana == thirdArcana; p3++)
        {
            if (GD->Personas[p3].Arcana == thirdArcana && p3 != r_Index && p3 != inVec1[i] && p3 != inVec2[i] &&
                (((GD->Personas[inVec1[i]].Level + GD->Personas[inVec2[i]].Level + GD->Personas[p3].Level) / 3) + 5 > r_lowLvl) &&
                (((GD->Personas[inVec1[i]].Level + GD->Personas[inVec2[i]].Level + GD->Personas[p3].Level) / 3) + 5 <= r_upLvl))
            {
                outVec.emplace_back( inVec1[i], inVec2[i], p3 );
            }
        }
    }
}


//-----------------------------------------------------------------------------------------
// QUERY FUNCTIONS
//-----------------------------------------------------------------------------------------

int ReverseCalc::ComboSize()
{
    if (!CalcInitialized)
        return 0;

    return r_ComboSize;
}

void ReverseCalc::Pause()
{
    // Only if there is an ongoing operation
    if (IsOngoing())
        r_pauseMakeRcp = true;
}

void ReverseCalc::Resume()
{
    // Only if it's paused
    if (r_pauseMakeRcp)
        r_pauseMakeRcp  = false;
}

void ReverseCalc::Stop()
{
    r_personaSet       = false;
    r_stopMakeRcp      = true;
    r_pauseMakeRcp     = false;
    r_currentIteration = 0;
}

void ReverseCalc::SetNoProcess()
{
    r_ComboSize        = 0;
    r_personaSet       = false;
    r_stopMakeRcp      = false;
    r_pauseMakeRcp     = false;
    r_isPersonaSpecial = false;
}

bool ReverseCalc::IsOngoing() const
{
    if (r_personaSet && !r_stopMakeRcp)
        return true;

    return false;
}

bool ReverseCalc::IsDone() const
{
    return r_stopMakeRcp;
}

bool ReverseCalc::IsPaused() const
{
    return r_pauseMakeRcp;
}

bool ReverseCalc::IsSpecial() const
{
    return r_isPersonaSpecial;
}

