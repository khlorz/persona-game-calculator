#include "calc.h"

//-----------------------------------------------------
// MAIN CALCULATOR FUNCTIONS (PUBLIC & PRIVATE)
//-----------------------------------------------------

bool PersonaCalc::InitCalc(const Gamedata* data)
{
    GD = data;

    if (GD == nullptr || GD->Personas.empty() || GD->Arcana_List.empty())
    {
        CalcInitialized = false;
        return false;
    }

    GD->GetNormalSpecials(SpecialNormals);
    GD->GetTriangleSpecials(SpecialTriangles);
    CalcInitialized = true;

    return true;
}

int PersonaCalc::CalculatePersona(const int& fusion_mode, const std::string& p1, const std::string& p2, const std::string& p3, bool sameLvl)
{
    if (p1 == p2 || p2 == p3 || p1 == p3 || !CalcInitialized)
        return -1;

    CalcFusionMode = fusion_mode;
    ID_1 = GD->GetPersonaIndex(p1);
    ID_2 = GD->GetPersonaIndex(p2);
    if (CalcFusionMode == FusionMode_Triangle)
        ID_3 = GD->GetPersonaIndex(p3);
    else
        ID_3 = -1;

    return CalculatePersona(CalcFusionMode, ID_1, ID_2, ID_3, sameLvl);
}

int PersonaCalc::CalculatePersona(const int& fusion_mode, const int& p1, const int& p2, const int& p3, bool sameLvl)
{
    if (!CalcInitialized || p1 < 0 || p2 < 0 || p1 == p2 || p2 == p3 || p1 == p3)
        return CalcError_Invalid;

    ID_1 = p1;
    ID_2 = p2;
    ID_3 = p3;
    CalcFusionMode = fusion_mode;

    switch (CalcFusionMode)
    {
        case FusionMode_Normal:   
            return NormalFusion();

        case FusionMode_Triangle: 
            if (p3 < 0)
                return CalcError_Invalid;

            return TriangleFusion(sameLvl);

        default:
            return CalcError_Invalid;
    }
}

int PersonaCalc::ResultantPersonaIndex() const
{
    int i = GD->GetArcanaIndex(ResultantArcana);
    if (i < 0)
        return CalcError_Invalid;

    const int& p_size = GD->Personas.size();

    for ( ; i < p_size && GD->Personas[i].Arcana == ResultantArcana; i++)
    {
        if (p_Lvl <= GD->Personas[i].Level) // The if condition must always be a persona base level higher or equal than the pLvl
        {
            if (!GD->Personas[i].Special) // If the resulting persona is not a special persona
                return i;

            do // Find the next persona that is not special
            {
                i++;
            
                if (GD->Personas[i].Arcana != ResultantArcana) // In case there are no more available persona, the loop will go down until the persona is no longer special
                {                                              // Example:
                    do                                         // If the supposed resulting persona is Alice,
                    {                                          // the resulting persona will be White Rider instead since after Alice
                        i--;                                   // is Mahakala but it's the highest possible persona yet a special persona
                    } while (GD->Personas[i].Special);         // already for the Death arcana, and the persona before Alice is White Rider

                    return i;                                  // Replace return with "return GD->Personas[i].Arcana == ResultantArcana ? i : CalcError_NoResult" in case the whole arcana only have special personas
                }
            } while (GD->Personas[i].Special);

            return i;
        }
    }

    // Find the previous persona in case p_Lvl is higher than the highest persona level in the particular arcana
    // Decrement i to point to the previous iteration that has the same arcana as the resultant arcana
    i--;

    while (i > 0 && GD->Personas[i].Special)
        i--;
    //if(GD->Personas[i].Arcana == ResultantArcana) // Uncomment this part in case the whole arcana only have special personas
    return i;

    return CalcError_NoResult;
}

//------------------------------------------------------------
// SPECIAL FUSION FUNCTIONS
//------------------------------------------------------------

// Finds the resultant persona from special fusion
// Returns the resultant persona
int PersonaCalc::SpecialFusionResult() const
{
    PersonaCombos CurrentCombo(ID_1, ID_2);

    for (const auto& SP : (CalcFusionMode == FusionMode_Normal ? SpecialNormals : SpecialTriangles))
    {
        if (CurrentCombo == SP)
            return SP.Result;
    }

    return CalcError_NoResult;
}

//----------------------------------------------------------
// NORMAL FUSION FUNCTIONS
//----------------------------------------------------------

int PersonaCalc::NormalFusion()
{
    // In case the input persona is a special persona
    int isSpecial = SpecialFusionResult();
    if ( isSpecial != CalcError_NoResult )
        return isSpecial;

    const int& P_Lvl1 = GD->Personas[ID_1].Level;
    const int& P_Lvl2 = GD->Personas[ID_2].Level;
    p_Lvl = ( ( P_Lvl1  +  P_Lvl2 ) / 2 ) + 1 ;

    // If both persona are of the same arcana
    if (GD->Personas[ID_1].Arcana == GD->Personas[ID_2].Arcana)
    {
        ResultantArcana = GD->Personas[ID_1].Arcana;
        return NormalSameArcana();
    }
    
    ResultantArcana = NormalResultantArcana(GD->Personas[ID_1].Arcana, GD->Personas[ID_2].Arcana);
    return ResultantArcana == "N/A" ? CalcError_NoResult : ResultantPersonaIndex();
}



// Finds the resultant persona from same arcana fusion in normal fusion
// Returns the resultant persona
int PersonaCalc::NormalSameArcana()
{   
    int highNum ;
    int lowerNum;
    ID_1 > ID_2 ? ( highNum = ID_1, lowerNum = ID_2 ) : ( highNum = ID_2, lowerNum = ID_1 );

    for (int i = lowerNum; i < highNum + 1; i++)
    {
        if ( GD->Personas[i].Level == p_Lvl ) // If the resulting level is equal to a persona, it will be the same persona rather than going for a lesser one unless it's a special one
        {
            while ( GD->Personas[i].Special == true && GD->Personas[i].Arcana == ResultantArcana )
            {
                i--;
            }

            if (GD->Personas[i].Arcana != ResultantArcana)
                return CalcError_NoResult;

            return i;
        }

        if ( GD->Personas[i].Level > p_Lvl ) // Finding the persona that has a level lower than pLvl
        {
            bool stop = false;
            do
            {
                i--;
                if (i == 0)
                {
                    stop = true;
                }
                else if (i == lowerNum)
                {
                    i--;
                }
            } while (GD->Personas[i].Special && GD->Personas[i].Arcana == ResultantArcana && stop == false);

            if ( i == 0 && stop == true ) // For the first persona on the list (i.e. Izanagi on P4/P4G or Orpheus on P3/P3F/P3P) to result N/A if forced stop
                return CalcError_NoResult;

            if (GD->Personas[i].Arcana != ResultantArcana)
                return CalcError_NoResult;

            return i;
        }
    }
    return CalcError_NoResult;
}

// Finds the resultant arcana from normal fusion
// Returns the resultant arcana
std::string PersonaCalc::NormalResultantArcana( const std::string& a1, const std::string& a2 ) const
{
    FusionData inArcana(a1, a2);

    for(const FusionData& FusionCombo : GD->NFusionData)
        if ( inArcana == FusionCombo )
            return FusionCombo.ResultantArcana;

    return "N/A";
}

//-------------------------------------------------------------------------------------
// TRIANGLE FUSION FUNCTIONS
//-------------------------------------------------------------------------------------

int PersonaCalc::TriangleFusion(bool sameLvl)
{
    int isSpecial = SpecialFusionResult();
    if (isSpecial != CalcError_NoResult)
        return isSpecial;

    const int& P_Lvl1 = GD->Personas[ID_1].Level;
    const int& P_Lvl2 = GD->Personas[ID_2].Level;
    const int& P_Lvl3 = GD->Personas[ID_3].Level;
    p_Lvl = ((P_Lvl1 + P_Lvl2 + P_Lvl3) / 3) + 5;

    // First checks if it's the same arcana fusion
    if (GD->Personas[ID_1].Arcana == GD->Personas[ID_2].Arcana && GD->Personas[ID_2].Arcana == GD->Personas[ID_3].Arcana)
    {
        ResultantArcana = GD->Personas[ID_1].Arcana;
        return TriangleSameArcana();
    }

    if (sameLvl == true)
    {
        if (ID_1 < ID_2)
            std::swap(ID_1, ID_2);

        if (ID_2 < ID_3)
            std::swap(ID_2, ID_3);

        if (ID_3 > ID_1)
            std::swap(ID_3, ID_1);
    }

    return TriangleFusionAll(GD->Personas[ID_1].Arcana, GD->Personas[ID_2].Arcana, GD->Personas[ID_3].Arcana);
}

int PersonaCalc::TriangleSameArcana()
{
    if (ID_1 > ID_2)
        std::swap(ID_1, ID_2);

    if (ID_2 > ID_3)
        std::swap(ID_2, ID_3);

    if (ID_3 < ID_1)
        std::swap(ID_1, ID_3);

    const int& p_size = GD->Personas.size();

    for (int i = ID_1; i < p_size && GD->Personas[i].Arcana == ResultantArcana; i++)
    {
        if (p_Lvl > GD->Personas[i].Level)
            continue;

        if (i == ID_2)
            i++;

        if (i == ID_3)
            i++;

        while ( GD->Personas[i].Special && GD->Personas[i].Arcana == ResultantArcana )
            i++ ;
        
        if (GD->Personas[i].Arcana != ResultantArcana)
            return CalcError_NoResult;
            
        return i ;
    }

    return CalcError_NoResult;
}

// Performs both the normal fusion for Persona 1 and Persona 2 and the triangle fusion
// Returns the resultant persona name
int PersonaCalc::TriangleFusionAll(const std::string& a1, const std::string& a2, const std::string& a3)
{
    const std::string& norm_arcana = NormalResultantArcana(a1, a2);
    if (norm_arcana == "N/A")
        return CalcError_NoResult;

    ResultantArcana = TriangleResultantArcana(norm_arcana, a3);
    if (ResultantArcana == "N/A")
        return CalcError_NoResult;

    return ResultantPersonaIndex();
}

// Finds the resultant arcana from triangle fusion of the input arcanas
// Returns the resultant arcana
std::string PersonaCalc::TriangleResultantArcana(const std::string& a1, const std::string& a2) const
{
    FusionData inArcana(a1, a2);

    for(const FusionData& FusionCombo : GD->TFusionData)
        if ( inArcana == FusionCombo )
            return FusionCombo.ResultantArcana;

    return "N/A";
}









