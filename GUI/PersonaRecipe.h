#pragma once

#ifndef _PersonaRecipe_
#define _PersonaRecipe_

#include "ReverseCalc.h"
#include "imgui.h"
#include "ImFunks.h"
#include <array>

class ImRcp
{
public:
	ImRcp();
	~ImRcp();

	bool InitGameData(const Gamedata* data);
	void MainBody();
	void ResetAll();
	void StopProcess();
	void DestroyList();
private:
	const Gamedata* GD;                // <* Main game data
	int             RecipeMode;        // <* Mode of recipe
	int             FilterFlags;
	int             ColumnNum;         // <* The number of columns in the result table
	int             ChosenID;
	int             FusionState;       // <* The state of making recipes (Normal or Triangle fusion)
	int             NumOfItems;        // <* The number of recipes that should be shown in the table
	int             StartingNum;       // <* The starting index for showing the recipes
	int             EndingNum;         // <* The ending index for showing the recipes
	bool            PageChanged;
	bool            SpOnly;
	bool            MaxOnly;
	bool            InitMakeRcp;       // <* Flag for initializing the variables for the table if the reverse fusion started
	bool            ShowPersona3;      // <* State for showing the third persona and arcana
	const char*     ComboPreview;
	PComboVec       ResultList;        // <* The recipe list. Contains the indices of the personas
	SpecialData     SpecialResult;     // <* The recipe list for special personas
	ReverseCalc     RCalculator;       // <* The reverse calculator object. 
	ComboFilterFunc PCombo;            // <* The combo filter for choosing a persona for reverse calculation
	ComboFilterPtr  ACombo;
	ImGuiTextFilter P1F, A1F,          // <* The filters for the recipe results
					P2F, A2F,
					P3F, A3F;

	void m_Buttons();
	void m_Filters();
	void m_Table();
	void m_RcpProcess();
	void m_ComboHelper(const float& size);

	void m_ResetCombo();
	void m_ResetValues();
	void m_ClearFilters();
	void m_ClearList();

	//-------------------------------------------------------------------------
	// UNUSED VARIABLES
	// - Due to various changes and updates
	// - No longer needed
	//-------------------------------------------------------------------------
	//bool            o_NoProcess;      // <* For status bar. State of making recipes, no ongoing process
	//bool            o_ForcedStop;     // <* For status bar. State of makign recipes, the process is forced to stop
};



#endif
