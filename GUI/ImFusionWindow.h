#pragma once

#include "Gamedata.h"
#include "imgui.h"
#include <array>

class ImFusionWindow
{
public:
	ImFusionWindow();

	bool InitializeWindow(const Gamedata* gd);
	void MainWindow(bool* w_open);
	void ErrorWindow(bool* w_open);
	void ResetValues();
private:
	enum TabItemChoice_
	{
		TabItemChoice_NormalFusion   = 0,
		TabItemChoice_TriangleFusion = 1,
		TabItemChoice_SpecialFusion  = 2,
		TabItemChoice_MoreInfo       = 3
	};

	const Gamedata*     GD;
	int                 TabItemChoice;
	int                 FusionTableColumns;
	bool                TabChanged;
	const FusionVec*    SelectedFusion;
	ImGuiTextFilter     A1TextFilter;
	ImGuiTextFilter     A2TextFilter;
	ImGuiTextFilter     ResultantTextFilter;

	void m_TabBar();
	void m_Body();
	void m_FusionTable();
	void m_MoreInfoBody();
	void m_ResetFilters();
};
