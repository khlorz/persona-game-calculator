#pragma once

#include "ResultFinder.h"
#include "ImFunks.h"
#include <array>

class ImPersonaFinder
{
public:
	ImPersonaFinder();
	~ImPersonaFinder() = default;

	bool InitGameData(const Gamedata* g);
	void MainBody();
	void ResetAll();
	void DestroyList();

private:
	const Gamedata*                   GD;                // <* Main Game data
	ResultFinder                      RFinder;           // <* Main Result Finder
	PComboVec                         ResultList;
	ImGuiTextFilter                   P1Filter;
	ImGuiTextFilter                   P2Filter;
	ImGuiTextFilter                   P3Filter;
	ImGuiTextFilter                   RPFilter;          // <* Filter for the resultant personas
	ImGuiTextFilter                   RAFilter;          // <* Filter for the resultant arcana
	std::array<bool, 16>              EnableComboArr;
	std::array<char[150], 16>         ComboPreviewArr;
	std::array<ComboFilterFunc, 16>   PersonaComboArr;
	std::array<ComboFilterPtr, 16>    ArcanaComboArr;
	std::array<PersonaLevelPair, 16>  PersonaLevelPairArr;
	std::array<GetPersonaFilter, 16>  FilterFlagsArr;

	void m_Table();
	void m_Buttons();
	void m_Filters();
	void m_Combos();
	void m_ComboHelper(const char* combo_label, const char* intput_label, const int& arr_idx);

	void m_ResetCombo();
	void m_ResetPreview();
	void m_ResetCheckBox();
	void m_ResetPersonaPairs();
	void m_ResetFilterFlags();
	void m_ClearList();
	void m_ClearFilters();
};