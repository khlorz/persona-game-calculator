#pragma once

#ifndef CALC_CLASS_H
#define CALC_CLASS_H

#include "Gamedata.h"

enum FusionMode_
{
    FusionMode_None     = 1 << 0,
    FusionMode_Normal   = 1 << 1,
    FusionMode_Triangle = 1 << 2,
    FusionMode_Special  = 1 << 3
};

enum CalcError_ : int
{
    CalcError_Invalid  = -1,
    CalcError_NoResult = -2
};

class PersonaCalc
{
public:
    PersonaCalc() : 
        CalcInitialized(false)
    {
        GD = NULL;
    }
    //~PersonaCalc();
    
    //--------------------------------------------------
    // MAIN CALCULATOR FUNCTIONS (PUBLIC)
    //--------------------------------------------------
    
    // Initialize the Persona Calculator with the chosen game data. Initialize only once unless there is a change in gamedata!
    bool InitCalc( const Gamedata* data );
    /**
    *  Calculate the resulting persona from the given persona. Use the other overload that uses the persona's index as input if the persona's index is already known 
    *  
    *  \param fusion_mode  - The Fusion Mode of calculating the persona. Choose from FusionMode_Normal or FusionMode_Triangle
    *  \param p1           - The first persona
    *  \param p2           - The second persona
    *  \param p3           - The third persona. The third persona is the persona with the highest current level of the three input personas. Only for FusionMode_Triangle
    *  \param sameLvl      - By default, the sameLvl fusion is false. Set to true for same level triangle fusion. Only for FusionMode_Triangle
    * 
    *  \returns Returns the resultant persona index;
    *           -1 if input personas are invalid or the calculator was not initialized/failed;
    *           -2 if resultant persona is not found
    */
    int CalculatePersona( const int& fusion_mode, const std::string& p1, const std::string& p2, const std::string& p3 = "N/A", bool sameLvl = false );
    /**
    *  Calculate the resulting persona from the given persona.
    *  
    *  \param fusion_mode - The Fusion Mode of calculating the persona. Choose from FusionMode_Normal or FusionMode_Triangle
    *  \param p1          - The first persona's index
    *  \param p2          - The second persona's index
    *  \param p3          - The third persona's index. The third persona is the persona with the highest current level of the three input personas. Only for FusionMode_Triangle
    *  \param sameLvl     - By default, the sameLvl fusion is false. Set to true for same current level triangle fusion. Only for FusionMode_Triangle
    * 
    *  \returns Returns the resultant persona index;
    *           -1 if input personas are invalid or the calculator was not initialized/failed;
    *           -2 if resultant persona is not found
    */
    int CalculatePersona( const int& fusion_mode, const int& p1, const int& p2, const int& p3 = -1, bool sameLvl = false );

protected:
    const Gamedata*  GD;                        // <* Main game data
    bool             CalcInitialized;           // <* Initialization flag
    PComboVec        SpecialNormals;
    PComboVec        SpecialTriangles;

private:
    int          CalcFusionMode;            // <* Fusion mode
    int          p_Lvl;                     // <* Resulting average level of sort for finding the persona
    int          ID_1,   ID_2,   ID_3;      // <* Contains the ID of the input personas
    std::string  ResultantArcana;           // <* Resultant arcana of the calculation/fusion.
    //-----------------------------------------------------
    // MAIN CALCULATOR FUNCTIONS (PRIVATE)
    //-----------------------------------------------------

    int ResultantPersonaIndex () const;
    int SpecialFusionResult  () const;

    //---------------------------------
    // NORMAL FUSION FUNCTIONS
    //---------------------------------

    int              NormalFusion             ( );                                          
    int              NormalSameArcana         ( );                               
    std::string      NormalResultantArcana    ( const std::string& a1, const std::string& a2 ) const;

    //---------------------------------
    // TRIANGLE FUSION FUNCTIONS
    //---------------------------------

    int              TriangleFusion           ( bool sameLvl = false );
    int              TriangleSameArcana       ( );
    int              TriangleFusionAll        ( const std::string& a1, const std::string& a2, const std::string& a3 );
    std::string      TriangleResultantArcana  ( const std::string& a1, const std::string& a2 ) const; 
};

#endif