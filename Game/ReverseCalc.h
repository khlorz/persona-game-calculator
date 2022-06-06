#pragma once

#ifndef REVERSE_CALC_H
#define REVERSE_CALC_H

#include "calc.h"

class ReverseCalc : private PersonaCalc
{
public:
    ReverseCalc  ( );
    ~ReverseCalc ( );

    //------------------------------------------------------
    // MAIN REVERSE CALCULATOR FUNCTIONS
    //------------------------------------------------------

    /**
    *  Initialize the reverse calculator gamedata
    * 
    *  @param pdata - The game data needed to perform reverse calculation
    * 
    *  @returns False if at least one of the important gamedata is empty, otherwise true
    */
    bool InitReverseCalc( const Gamedata* data );
    /**
    *  Set the persona for reverse calculation using the persona's string
    * 
    *  @param inputPersona - The persona needed for reverse calculation
    *  @param fusion_mode  - The fusion mode for performing reverse calculation. @see FusionMode_
    * 
    *  @returns False if the object is not initialized or inputPersona does not exist, otherwise true
    */
    bool SetReversePersona( const std::string& inputPersona, const int& fusion_mode );        
    /**
    *  Set the persona for reverse calculation using the persona index
    * 
    *  @param p_index      - The index of the persona needed for reverse calculation
    *  @param fusion_mode  - The fusion mode for performing reverse calculation. @see FusionMode_
    * 
    *  @returns False if at least one of the important gamedata is empty, otherwise true
    */
    bool SetReversePersona( const int& p_index, const int& fusion_mode );
    /**
    *  Performs the reverse calculation of the set persona
    *
    *  @param n_result  - The output combo vector. Empty if the set persona is special
    *  @param sp_result - The output special combo vector. Empty if the set persona is not special
    *
    *  @returns True while the operation is ongoing. False if it is stopped (forced stop or done) or paused
    */
    bool ReverseFusion( PComboVec& n_result, SpecialData& sp_result );


    //-------------------------------
    // QUERY FUNCTIONS
    //-------------------------------

    int    ComboSize();        // <* Returns the size of the combination of the chosen fusion mode
    void   Pause();            // <* Set the pause the current process
    void   Resume();           // <* Resume the current process if it was paused earlier
    void   Stop();             // <* Stop the Current process
    void   SetNoProcess();     // <* Set the proccess bools to false 
    bool   IsPaused()  const;  // <* Returns true if the process was paused 
    bool   IsSpecial() const;  // <* Returns true if persona is special 
    bool   IsDone()    const;  // <* Returns true if the process is already done 
    bool   IsOngoing() const;  // <* Returns true if the process is still ongoing 

private:


    int           r_upLvl;              // The Upper Level needed for finding recipes
    int           r_lowLvl;             // The Lower Level needed for finding recipes
    int           r_fusionMode;         // 0 for Normal Fusion, 1 for Triangle Fusion
    int           r_currentIteration;   // Current iteration of making recipe
    int           r_Index;              // The persona's index for reverse fusion
    int           r_ComboSize;          // The current combo size of the reverse calculation
    bool          r_personaSet;         // Flag if the persona is set or not
    bool          r_isPersonaSpecial;   // Flag for checking if special persona
    bool          r_stopMakeRcp;        // Force stop making persona recipes
    bool          r_pauseMakeRcp;       // Pause state of the current operation
    IntVec        r_pList;              //
    PComboVec     r_spNormalCombos;     // Contains the special combos from normal fusion
    FusionVec     r_mainCombos;         // Contains the triangle combinations of an arcana
    const char*   r_Persona;            // Reverse Persona Name
    const char*   r_Arcana;             // Reverse Persona's Arcana

    void          SetUpperLowerLevel    ();
    void          ReverseSpecialFusion  (SpecialData& outVec) const;
    void          ReverseNormalFusion   (PComboVec& outVec);
    void          ReverseTriangleFusion (PComboVec& outVec);
    void          TriPossibleCombos     (const FusionVec& _A, IntVec& pArr1, IntVec& pArr2) const ;
    void          TriCorrectCombos      (const IntVec& inVec1, const IntVec& inVec2, const std::string& thirdArcana, PComboVec& outVec) const;
};

#endif