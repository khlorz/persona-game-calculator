#pragma once

#ifndef _ImPersonaCalculator_
#define _ImPersonaCalculator_

#include "imgui.h"
#include "ImFunks.h"
#include "calc.h"
#include <array>

class ImCalc
{
public:
	ImCalc();
	~ImCalc();

	bool InitGameData(const Gamedata* data);
	// Resets the filters and results
	void ResetAll();
	// Render the main body
	void CalculatorBody(bool *w_open);
	// Set the dimension of the whole calculator body
	void SetBodyDimensions( float _w = 0, float _h = 0);
	void DestroyList();

private:
	const Gamedata*                 GD;             // The game data pointer
	float                           MainBodyHeight; // Height of the Calculator
	float                           MainBodyWidth;  // Width of the Calculator
	bool                            SameLevel;
	int                             FusionMode;
	int                             ThirdPersona;
	int                             ResultantID; //
	PersonaCalc                     TheCalculator;  // Calculator class object
	std::array<char[100], 3>        ComboPreviewArr;
	std::array<int, 3>              ChosenIDArr;
	std::array<ComboFilterFunc, 3>  PComboArr;
	std::array<ComboFilterFunc, 3>  AComboArr;
	std::array<GetPersonaFilter, 3> FilterFlagsArr;
	

	void m_ResetCombo();
	void m_ResetValues();
	void m_ResultTable();
	void m_ChoicesTable();
	void m_ComboHelper(const char* label, const float& size, const int& arr_idx);
};

#endif