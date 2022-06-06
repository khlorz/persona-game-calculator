#pragma once

#ifndef _PersonaData_
#define _PersonaData_

#include "ImFunks.h"
#include "Gamedata.h"
#include "ImageTextures.h"
#include <array>

class ImPersonaData
{
public:
	ImPersonaData();
	~ImPersonaData();

	void    MainWindow(bool *w_open);
	void    ErrorWindow(bool *w_open);
	void    ResetAll();
	bool    InitPersonaData(const Gamedata* data);
	void    SetSkillElementIcons(ImgTextures img[]);
	bool    LoadSkillCardTexture();
	void    ClearTextures();

private:
	typedef int HoveredElemIcon_;

	const Gamedata*    GD;                // <* Main game data
	const PersonaData* PD;                // <* Chosen persona data
	const SkillData*   PopUpSkill;
	bool               OpenPopUpSkill;
	bool               ClickedPopUpSkill;
	int                SelectedSkillLevel;
	bool               DataExist;
	ImVec2             RightSideSize;
	CharVec            ElemAff;
	ComboFilterFunc    ArcanaCombo;
	ComboFilterFunc    PersonaCombo;
	ImgTextures        SkillCardTexture;
	TextureArray<16>   SkillElementIcons;

	void ResetCombos();
	void PersonaStats() const;
	void PersonaSkillTable();
	void ArcanaPersonaComboFilters();
	void PopUpSkillWindow();

	// Utility Functions
	void SetPersonaData(const int& ID);
	void HoverElemIcons(const HoveredElemIcon_& icon);

	// Needs to be fixed/worked on
	void PersonaGameExclusive();
	void RenderSkillCard(const SkillCardData& scard);
	void RenderHeartItem();
	void RenderFusionSkill();
};

#endif