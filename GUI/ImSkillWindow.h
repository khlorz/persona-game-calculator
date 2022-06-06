#pragma once

#include "Gamedata.h"
#include "ImageTextures.h"

class ImSkillWindow
{
public:
	ImSkillWindow();
	~ImSkillWindow() = default;

    bool InitGameData(const Gamedata* gd);
    void MainWindow(bool* w_open);
    void ErrorWindow(bool* w_open);
    void SetSkillElementIcons(ImgTextures imgtex[]);
    void ClearTextures();
    void ResetAll();
private:
    const Gamedata* GD;

    int               SkillElementIdx;    
    int               OtherFilterIdx;
    int               ArcanaChoiceIdx;
    int               InnateNonInnateChoiceIdx;

    const char*       ArcanaFilter;
    StrVec            ArcanaChoiceList;

    bool              IsSkillListReset;
    int               SkillListTableSize;
    int               SkillListTableColumnNum;

    SkillIDVec        FilteredSkillList;
    GetSkillListFlag  GetSkillFlag;
    TextureArray<16>  SkillIcons;

    // Main Functions
    void m_SkillElementalTypeBody();
    void m_FilterBody();
    void m_MainTableBody();
    void m_ResetSkillList();
    void m_ResetValues();
    void m_ResetFilters();

    // Functions for setting the GetSkillFlag
    void m_SetInnateNonInnateSkillFilterFlag();
    void m_SetSkillCardFilterFlag();
    void m_SetArcanaSkillFilterFlag();

    // Functions for filtering the persona in the skill table
    bool m_FilterPersonaWithSkill(const SkillUser& _P);
};