#include "ImSkillWindow.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Fonts/IconsFontAwesome5.h"

static void HoverToolTip(const char* desc);

ImSkillWindow::ImSkillWindow() : 
	GD(nullptr),
	IsSkillListReset(false)
{}

bool ImSkillWindow::InitGameData(const Gamedata* gd)
{
	GD = gd;

	if (GD == nullptr || GD->GameSkills.empty())
		return false;

	ResetAll();
	return true;
}

void ImSkillWindow::ResetAll()
{
	m_ResetValues();
	m_ResetFilters();
	m_ResetSkillList();
}

void ImSkillWindow::MainWindow(bool* w_open)
{
	ImGui::SetNextWindowSize(ImVec2(1000.0f, 370.0f), ImGuiCond_Appearing);
	if (!ImGui::Begin("Skill List##window", w_open, ImGuiWindowFlags_NoResize))
	{
		ImGui::End();
		return;
	}

	m_SkillElementalTypeBody();

	ImGui::SameLine();
	if (!ImGui::BeginChild("TableChild##cw3", ImGui::GetContentRegionAvail()))
	{
		ImGui::EndChild();
	}
	else
	{
		m_FilterBody();
		m_MainTableBody();

		ImGui::EndChild();
	}

	ImGui::End();
}

void ImSkillWindow::ErrorWindow(bool* w_open)
{
	ImGui::SetNextWindowSize(ImVec2(500.0f, 80.0f), ImGuiCond_Appearing);
	if (!ImGui::Begin("No Loaded Skill Data", w_open, ImGuiWindowFlags_NoResize))
	{
		ImGui::End();
		return;
	}

	ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);
#ifdef PCALC_DEBUG
	ImGui::TextUnformatted("No skill data loaded. Check for the location/name/extension of the file. "
						   "It must be the game's acronym + _skill.dat ( i.e. Persona 4 Golden -> p4g_skill.dat ) file in a PersonaData folder");
#elif PCALC_RELEASE
	ImGui::TextUnformatted("Still experimental. Consider using the debug .exe to be able to access this feature.");
#endif // PCALC_DEBUG
	ImGui::PopTextWrapPos();


	ImGui::End();
}

//---------------------------------------------------------------------------------------
// TEXTURE FUNCTIONS
//---------------------------------------------------------------------------------------

void ImSkillWindow::SetSkillElementIcons(ImgTextures imgtex[])
{
	for (int i = 0; i < SkillIcons.size(); i++)
	{
		SkillIcons[i] = &(imgtex[i]);
	}
}

void ImSkillWindow::ClearTextures()
{
	for (auto& T : SkillIcons)
		T->ClearTextures();
}


//---------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
//---------------------------------------------------------------------------------------

void ImSkillWindow::m_SkillElementalTypeBody()
{
	if (!ImGui::BeginChild("SkillType##cw1", ImVec2(90.0f, ImGui::GetContentRegionAvail().y)))
	{
		ImGui::EndChild();
		return;
	}

	ImGui::TextUnformatted("Skill Types");

	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 5.00f);
	if (!ImGui::BeginChild("SkillType##cw2", ImGui::GetContentRegionAvail(), true)) // Selectable window
	{
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}
	else
	{
		if (ImGui::Selectable("All", SkillElementIdx == -1))
		{
			if (SkillElementIdx != -1)
			{
				SkillElementIdx = -1;
				m_ResetSkillList();
			}
		}
		ImGui::Separator();
		for(const auto& E : GD->MainElements)
		{
			if (ImGui::Selectable(GD->MainElementsChar[E], SkillElementIdx == E))
			{
				if (SkillElementIdx != E)
				{
					SkillElementIdx = E;
					m_ResetSkillList();
				}
			}

			ImGui::Separator();
		}

		ImGui::EndChild();
	}
	ImGui::PopStyleVar();

	ImGui::EndChild();
}

void ImSkillWindow::m_SetArcanaSkillFilterFlag()
{
	GetSkillFlag = ArcanaFilter == nullptr ? GetSkillListFlag_None : GetSkillListFlag_PersonaOfArcana;
}

void ImSkillWindow::m_SetInnateNonInnateSkillFilterFlag()
{
	switch (InnateNonInnateChoiceIdx)
	{
	case 1:
		GetSkillFlag = GetSkillListFlag_InnateOnlyOfArcana;
		break;

	case 2:
		GetSkillFlag = GetSkillListFlag_NonInnateOnlyOfArcana;
		break;

	default:
		GetSkillFlag = GetSkillListFlag_InnateNonInnateOfArcana;
		break;
	}
}

void ImSkillWindow::m_SetSkillCardFilterFlag()
{
	GetSkillFlag = GetSkillListFlag_SkillCardOfArcana;
}

void ImSkillWindow::m_FilterBody()
{
	//---------------------------------------------------------
	// Other Skill Filter Combo Box
	//---------------------------------------------------------
	static constexpr std::array<const char*, 3> OtherFilters = { "Basic Skill Data", "Personas With Skill", "Skill Card" };

	ImGui::PushItemWidth(170.0f);
	if (ImGui::BeginCombo("##OtherSkillFilter", OtherFilters[OtherFilterIdx]))
	{
		for (int i = 0; i < OtherFilters.size(); i++) {
			if (ImGui::Selectable(OtherFilters[i], OtherFilterIdx == i))
			{
				if (OtherFilterIdx != i)
				{
					OtherFilterIdx = i;

					switch (i)
					{
					case 0: 
						GetSkillFlag = GetSkillListFlag_None;    
						SkillListTableColumnNum = 8;
						break;

					case 1: 
						m_SetInnateNonInnateSkillFilterFlag(); 
						SkillListTableColumnNum = 4;
						break;

					case 2: 
						m_SetSkillCardFilterFlag();
						SkillListTableColumnNum = 4;
						break;

					default:
						break;
					}

					m_ResetSkillList();
				}
			}
		}

		ImGui::EndCombo();
	}
	ImGui::SameLine();
	ImGui::TextUnformatted(ICON_FA_QUESTION_CIRCLE);
	HoverToolTip(
				  "Basic Skill Data - Shows the basic data about the skill. Cost, Chance percentage (if there is any), etc.\n\n"
				  "Personas with Skill - Shows the skill and the personas that have the same skill.\n\n"
				  "Skill Card - Shows the skill and it's corresponding persona that holds the skill card."
				);

	ImGui::SameLine();

	//---------------------------------------------------------
	// Arcana List Combo Box
	//---------------------------------------------------------
	ImGui::PushDisabled((OtherFilterIdx & (1 | 2)) == 0);
	{
		ImGui::PushItemWidth(170.0f);
		if (ImGui::BeginCombo("##ArcanaSkillFilter", ArcanaChoiceList[ArcanaChoiceIdx].c_str()))
		{
			const int& sz = ArcanaChoiceList.size();

			for (int i = 0; i < sz; i++) {
				if (ImGui::Selectable(ArcanaChoiceList[i].c_str(), ArcanaChoiceIdx == i))
				{
					if (ArcanaChoiceIdx != i)
					{
						ArcanaChoiceIdx = i;
						ArcanaFilter = ArcanaChoiceIdx == 0 ? nullptr : GD->Arcana_List[ArcanaChoiceIdx - 1].c_str();

						if (OtherFilterIdx == 1)
							m_SetInnateNonInnateSkillFilterFlag();

						else if (OtherFilterIdx == 2)
							m_SetSkillCardFilterFlag();

						else
							m_SetArcanaSkillFilterFlag();

						m_ResetSkillList();
					}
				}
			}

			ImGui::EndCombo();
		}
	}
	ImGui::PopDisabled();
	ImGui::SameLine();
	ImGui::TextUnformatted(ICON_FA_QUESTION_CIRCLE);
	HoverToolTip(
				  "Choose an arcana to filter out the personas to your liking"
				);

	ImGui::SameLine();

	//---------------------------------------------------------
	// Innate and Non-Innate Combo Box
	//---------------------------------------------------------
	ImGui::PushDisabled((OtherFilterIdx & 1) == 0);
	{
		static constexpr std::array<const char*, 3> InnateNonInnateChoices = { "Innate and Non-Innate", "Innate Skills Only", "Non-Innate Skills Only" };

		ImGui::PushItemWidth(170.0f);
		if (ImGui::BeginCombo("##Non/InnateFilter", InnateNonInnateChoices[InnateNonInnateChoiceIdx]))
		{
			for (int i = 0; i < InnateNonInnateChoices.size(); i++) {
				if (ImGui::Selectable(InnateNonInnateChoices[i], InnateNonInnateChoiceIdx == i))
				{
					if (InnateNonInnateChoiceIdx != i)
					{
						InnateNonInnateChoiceIdx = i;
						m_SetInnateNonInnateSkillFilterFlag();
						m_ResetSkillList();
					}
				}
			}

			ImGui::EndCombo();
		}
		ImGui::SameLine();
		ImGui::TextUnformatted(ICON_FA_QUESTION_CIRCLE);
		HoverToolTip(
					  "Filter out the skills for either innate or non-innate skills or both.\n\n"
					  "Innate skills are skills learned as soon as you get the persona.\n"
					  "Non - innate skills need a certain level before you can learn that particular skill."
					);
	}
	ImGui::PopDisabled();
}

void ImSkillWindow::m_MainTableBody()
{
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.0f, 5.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 10.0f);
	if (!ImGui::BeginTable("SkillListTable##t1", SkillListTableColumnNum, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX, ImGui::GetContentRegionAvail()))
	{
		ImGui::PopStyleVar(2);
		return;
	}

	ImGui::TableSetupScrollFreeze(3, 1);
	if (IsSkillListReset)
	{
		ImGui::SetScrollX(0.00f);
		ImGui::SetScrollY(0.00f);
		IsSkillListReset = false;
	}

	// Table for skill basic info
	if (SkillListTableColumnNum == 8)
	{
		ImGui::TableSetupColumn("Type"        , ImGuiTableColumnFlags_WidthFixed, 25.0f);
		ImGui::TableSetupColumn("Skill Name"  , ImGuiTableColumnFlags_WidthFixed, 120.0f);
		ImGui::TableSetupColumn("Description" , 0, 320.0f);
		ImGui::TableSetupColumn("Cost"        , 0, 40.0f);
		ImGui::TableSetupColumn("Chance/Crit" , 0, 30.0f);
		ImGui::TableSetupColumn("Power"       , 0, 30.0f);
		ImGui::TableSetupColumn("Accuracy"    , 0, 30.0f);
		ImGui::TableSetupColumn("Inherit"     , 0, 30.0f);
		ImGui::TableHeadersRow();

		ImGuiListClipper clip;
		clip.Begin(FilteredSkillList.size());
		while (clip.Step())
		{
			for (int i = clip.DisplayStart; i < clip.DisplayEnd; i++)
			{
				const auto& CurrentSkill = GD->GameSkills[FilteredSkillList[i]];

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Image((void*)(intptr_t)SkillIcons[CurrentSkill.Element]->GetTexture(), ImVec2(22.0f, 17.0f));
			
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(CurrentSkill.Name.c_str());

				ImGui::TableNextColumn();
				ImGui::TextUnformatted(CurrentSkill.Description.c_str());

				ImGui::TableNextColumn();
				if (CurrentSkill.CostType == "HP")
					CurrentSkill.Cost ? ImGui::Text("HP: %d%%", CurrentSkill.Cost) : ImGui::TextUnformatted("-----");
				else
					CurrentSkill.Cost ? ImGui::Text("SP: %d", CurrentSkill.Cost) : ImGui::TextUnformatted("-----");

				ImGui::TableNextColumn();
				CurrentSkill.ChanceValue > 0 ? ImGui::Text("%d%%", CurrentSkill.ChanceValue) : ImGui::TextUnformatted("-----");

				ImGui::TableNextColumn();
				CurrentSkill.Power > 0 ? ImGui::Text("%d", CurrentSkill.Power) : ImGui::TextUnformatted("-----");

				ImGui::TableNextColumn();
				CurrentSkill.Accuracy > 0 ? ImGui::Text("%d%%", CurrentSkill.Accuracy) : ImGui::TextUnformatted("-----");

				ImGui::TableNextColumn();
				ImGui::PushStyleColor(ImGuiCol_Text, CurrentSkill.Inherit ? ImVec4(0.40f, 1.00f, 0.30f, 1.0f) : ImVec4(1.00f, 0.10f, 0.10f, 1.0f));
				ImGui::TextUnformatted(CurrentSkill.Inherit ? "Yes" : "No");
				ImGui::PopStyleColor();
			}
		}

		ImGui::EndTable();
		ImGui::PopStyleVar(2);

		return;
	}

	// Table for skill card and persona with skills
	ImGui::TableSetupColumn("Type"           , ImGuiTableColumnFlags_WidthFixed, 25.0f);
	ImGui::TableSetupColumn("Skill Name"     , ImGuiTableColumnFlags_WidthFixed, 120.0f);
	ImGui::TableSetupColumn("Inherit"        , ImGuiTableColumnFlags_WidthFixed, 40.0f);
	ImGui::TableSetupColumn("Persona (Level)", ImGuiTableColumnFlags_WidthFixed | ImGuiColumnsFlags_GrowParentContentsSize);
	ImGui::TableHeadersRow();

	ImGuiListClipper clip;
	clip.Begin(FilteredSkillList.size());
	while (clip.Step())
	{
		for (int i = clip.DisplayStart; i < clip.DisplayEnd; i++)
		{
			const auto& CurrentSkill = GD->GameSkills[FilteredSkillList[i]];

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Image((void*)(intptr_t)SkillIcons[CurrentSkill.Element]->GetTexture(), ImVec2(22.0f, 17.0f));

			ImGui::TableNextColumn();
			ImGui::TextUnformatted(CurrentSkill.Name.c_str());

			ImGui::TableNextColumn();
			ImGui::PushStyleColor(ImGuiCol_Text, CurrentSkill.Inherit ? ImVec4(0.40f, 1.00f, 0.30f, 1.0f) : ImVec4(1.00f, 0.10f, 0.10f, 1.0f));
			ImGui::TextUnformatted(CurrentSkill.Inherit ? "Yes" : "No");
			ImGui::PopStyleColor();

			ImGui::TableNextColumn();
			static std::string ListOfPersona;
			ListOfPersona.clear();

			// Because the GetSkillList is still not a good way to filter out the skills that have certain personas and other things, 
			// this is where we filter out the left-over things that needs to be filtered out.
			for (auto& P : CurrentSkill.Personas)
			{
				if (m_FilterPersonaWithSkill(P))
				{
					if (P.SkillLevel == 0)
					{
						ListOfPersona += P.Persona->Name + "  ,  ";
					}
					else
					{
						char buf[128];
						sprintf_s(buf, 128, "%s ( %d )  ,  ", P.Persona->Name.c_str(), P.SkillLevel);
						ListOfPersona += buf;
					}
				}
			}
			ListOfPersona.resize(ListOfPersona.length() - 3);
			ImGui::TextUnformatted(ListOfPersona.c_str());
		}
	}
	
	ImGui::EndTable();
	ImGui::PopStyleVar(2);
}

bool ImSkillWindow::m_FilterPersonaWithSkill(const SkillUser& _P)
{
	if (ArcanaFilter != nullptr && _P.Persona->Arcana != ArcanaFilter)
		return false;

	if (OtherFilterIdx == 1)
	{
		switch (InnateNonInnateChoiceIdx)
		{
		case 0:  return true;
		case 1:  return _P.SkillLevel == 0;
		case 2:  return _P.SkillLevel > 0;
		default: return false;
		}
	}

	if (OtherFilterIdx == 2)
		return _P.SkillCard;

	return false;
}

void ImSkillWindow::m_ResetValues()
{
	SkillListTableColumnNum  = 8;

	ArcanaChoiceList.emplace_back("All");
	ArcanaChoiceList.insert(ArcanaChoiceList.end(), GD->Arcana_List.begin(), GD->Arcana_List.end());
}

void ImSkillWindow::m_ResetSkillList()
{
	GD->GetSkillList(FilteredSkillList, GetSkillFlag, SkillElementIdx, ArcanaFilter);
	IsSkillListReset = true;
}

void ImSkillWindow::m_ResetFilters()
{
	GetSkillFlag             = GetSkillListFlag_None;
	ArcanaFilter             = nullptr;
	SkillElementIdx          = -1;
	OtherFilterIdx           = 0;
	ArcanaChoiceIdx          = 0;
	InnateNonInnateChoiceIdx = 0;
}

static void HoverToolTip(const char* desc)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(250.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}