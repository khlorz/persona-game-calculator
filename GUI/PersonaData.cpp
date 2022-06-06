#include "PersonaData.h"
#include "Fonts/IconsFontAwesome5.h"

ImPersonaData::ImPersonaData() :
	GD(nullptr),
	PD(nullptr),
	PopUpSkill(nullptr),
	DataExist(true),
	ElemAff     ( { "---", "Wk", "Str", "Abs", "Rpl" } ), 
	PersonaCombo( "##PComboPD" ),
	ArcanaCombo ( "##AComboPD" )
{
}

ImPersonaData::~ImPersonaData()
{
	ClearTextures();
}

bool ImPersonaData::InitPersonaData(const Gamedata* data)
{
	GD = data;
	
	if (GD == nullptr || GD->Personas.empty() || GD->Arcana_List.empty() || GD->Persona_List.empty() || GD->MainElements.empty())
		return false;

	ResetAll();
	return true;
}

void ImPersonaData::ResetAll()
{
	ResetCombos();
	SetPersonaData(0); // Set the initial persona data with the first persona in the list (which is usually the first persona you get to use in the game)
}

bool ImPersonaData::LoadSkillCardTexture()
{
	SkillCardTexture.LoadTextures("Images/Elemental Icons/skillcard.png");

	if (SkillCardTexture.GetTexture() == 0)
		return false;
	
	return true;
}

void ImPersonaData::ClearTextures()
{
	SkillCardTexture.ClearTextures();
	for (auto& T : SkillElementIcons)
		T->ClearTextures();
}

void ImPersonaData::ResetCombos()
{
	StrVec Temp = { "All Arcanas" };
	Temp.insert(Temp.end(), GD->Arcana_List.begin(), GD->Arcana_List.end());

	ArcanaCombo.Reset(Temp);
	PersonaCombo.Reset(GD->Persona_List);
}


void ImPersonaData::MainWindow(bool *w_open)
{
	ImGui::SetNextWindowSize(ImVec2(600.0f, 400.0f), ImGuiCond_Once);
	if (!ImGui::Begin("Persona Main Data", w_open, ImGuiWindowFlags_NoResize))
	{
		ImGui::End();
		return;
	}

	ArcanaPersonaComboFilters();

	ImGui::Separator(ImVec2(1.0f, 5.0f));

	if (!ImGui::BeginChild("PersonaDataEtc", ImVec2(ImGui::GetContentRegionAvail().x / 1.80f, ImGui::GetContentRegionAvail().y)))
	{
		ImGui::EndChild();
		return;
	}

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::Text("%s", PD->Name.c_str());
	ImGui::SameLine();
	ImGui::Text("%s", PD->Arcana.c_str());
	ImGui::SameLine();
	ImGui::Text("%d", PD->Level);
	ImGui::PopFont();

	// Early exit if there is no existing data from the chosen persona
	if (!DataExist)
	{
		ImGui::EndChild();
		ImGui::EndChild();
		return;
	}
	
	if (ImGui::BeginTable("Elemental Weaknesses", GD->P_Elements.size(), ImGuiTableFlags_Borders))
	{
		ImGui::TableNextRow();
		for(const auto& E : PD->Elements.Element)
		{
			ImGui::TableNextColumn();
			ImGui::Image((void*)(intptr_t)SkillElementIcons[E]->GetTexture(), ImVec2(20.0f, 15.0f));
			HoverElemIcons(E);
		}

		ImGui::TableNextRow();
		for (const auto& A : PD->Elements.Affinity)
		{
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(ElemAff[A]);
		}
		ImGui::EndTable();
	}

	PersonaGameExclusive();

	ImGui::EndChild();

	ImGui::SameLine();

	RightSideSize  = ImGui::GetContentRegionAvail();
	if(!ImGui::BeginChild("RightSide##PData", RightSideSize))
	{
		ImGui::EndChild();
	}
	else
	{
		PersonaStats();
		PersonaSkillTable();

		ImGui::EndChild();
	}

	ImGui::End();
}

void ImPersonaData::ErrorWindow(bool* w_open)
{
	ImGui::SetNextWindowSize(ImVec2(500.0f, 80.0f), ImGuiCond_Appearing);
	if (!ImGui::Begin("No Loaded Persona Data", w_open, ImGuiWindowFlags_NoResize))
	{
		ImGui::End();
		return;
	}

	ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);
#ifdef PCALC_DEBUG
	ImGui::TextUnformatted("No persona data loaded. Check for the location/name/extension of the file. "
		                   "It must be the game's acronym + _persona.dat ( i.e. Persona 4 Golden -> p4g_persona.dat ) file in a PersonaData folder");
#elif PCALC_RELEASE
ImGui::TextUnformatted("Still experimental. Consider using the debug .exe to be able to access this feature");
#endif // DEBUG
	ImGui::PopTextWrapPos();

	ImGui::End();
}

void ImPersonaData::SetSkillElementIcons(ImgTextures img[])
{
	for (int i = 0; i < SkillElementIcons.size(); i++)
	{
		SkillElementIcons[i] = &(img[i]);
	}
}

void ImPersonaData::PersonaStats() const
{
	if (ImGui::BeginTable("##StatTable", 1, ImGuiTableFlags_PadOuterX))
	{
		ImGui::TableSetupColumn("Persona Stats:");
		ImGui::TableHeadersRow();

		ImGui::EndTable();
	}

	static float StatBarHeight = ImGui::GetTextLineHeight();
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 3.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.378f, 0.394f, 0.729f, 0.155f));
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.173f, 0.753f, 1.00f, 0.706f));
	ImGui::AlignTextToFramePadding(); ImGui::TextUnformatted("Str");  ImGui::SameLine(33.0f);   ImGui::ProgressBar((float)PD->Stats.Strength    / 100.0f, ImVec2(RightSideSize.x - 55.0f, StatBarHeight), ""); ImGui::SameLine(0, 2.50f); ImGui::Text("%d", PD->Stats.Strength);
	ImGui::AlignTextToFramePadding(); ImGui::TextUnformatted("Mag");  ImGui::SameLine(33.0f);	ImGui::ProgressBar((float)PD->Stats.Magic	    / 100.0f, ImVec2(RightSideSize.x - 55.0f, StatBarHeight), ""); ImGui::SameLine(0, 2.50f); ImGui::Text("%d", PD->Stats.Magic);
	ImGui::AlignTextToFramePadding(); ImGui::TextUnformatted("Agi");  ImGui::SameLine(33.0f);	ImGui::ProgressBar((float)PD->Stats.Agility     / 100.0f, ImVec2(RightSideSize.x - 55.0f, StatBarHeight), ""); ImGui::SameLine(0, 2.50f); ImGui::Text("%d", PD->Stats.Agility);
	ImGui::AlignTextToFramePadding(); ImGui::TextUnformatted("Endu"); ImGui::SameLine(33.0f);	ImGui::ProgressBar((float)PD->Stats.Endurance   / 100.0f, ImVec2(RightSideSize.x - 55.0f, StatBarHeight), ""); ImGui::SameLine(0, 2.50f); ImGui::Text("%d", PD->Stats.Endurance);
	ImGui::AlignTextToFramePadding(); ImGui::TextUnformatted("Lck");  ImGui::SameLine(33.0f);	ImGui::ProgressBar((float)PD->Stats.Luck	    / 100.0f, ImVec2(RightSideSize.x - 55.0f, StatBarHeight), ""); ImGui::SameLine(0, 2.50f); ImGui::Text("%d", PD->Stats.Luck);
	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar(3);
}

void ImPersonaData::PersonaSkillTable()
{	
	if (!ImGui::BeginTable("Skills", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_PadOuterX | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY, ImVec2(RightSideSize.x, ImGui::GetContentRegionAvail().y)))
		return;

	ImGui::TableSetupScrollFreeze(0, 1);
	char table_title[64];
	sprintf_s(table_title, 64, "%s's Skills: ", PD->Name.c_str());
	ImGui::TableSetupColumn(table_title);
	ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_NoHeaderWidth, ImGui::GetContentRegionAvail().x / 3.50f);
	ImGui::TableHeadersRow();

	ImGuiListClipper clip;
	clip.Begin(PD->Skills.size());
	while (clip.Step())
	{
		for (int i = clip.DisplayStart; i < clip.DisplayEnd; i++)
		{
			ImGui::TableNextRow(ImGuiTableRowFlags_None);
			ImGui::TableNextColumn();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.75f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.35f));

			if (PD->GameEx.SkillCard.ID == PD->Skills[i].SkillID)
			{
				ImDrawList* drawlist = ImGui::GetWindowDrawList();
				ImGuiWindow* window = ImGui::GetCurrentWindow();

				ImRect bb(window->DC.CursorPos + window->WindowPadding + ImVec2(0.0f, 2.0f), window->DC.CursorPos + ImVec2(12.0f, 15.0f) + window->WindowPadding + ImVec2(0.0f, 2.0f));

				drawlist->AddImage((void*)(intptr_t)SkillCardTexture.GetTexture(), bb.Min, bb.Max);
			}

			if (ImGui::Button(GD->GameSkills[PD->Skills[i].SkillID].Name.c_str(), ImVec2(ImGui::GetColumnWidth(), 18.0f)))
			{
				OpenPopUpSkill       = true;
				ClickedPopUpSkill    = true;
				PopUpSkill           = &(GD->GameSkills[PD->Skills[i].SkillID]);
				SelectedSkillLevel   = PD->Skills[i].Level;
			}
			ImGui::TableNextColumn();
			if (PD->Skills[i].Level == 0)
			{
				ImGui::Button("Innate", ImVec2(ImGui::GetColumnWidth(), 18.0f));
			}
			else
			{
				char buf[32];
				sprintf(buf, "%d", PD->Skills[i].Level);
				ImGui::Button(buf, ImVec2(ImGui::GetColumnWidth(), 18.0f));
			}
			ImGui::PopStyleColor(3);
		}
	}

	ImGui::EndTable();

	PopUpSkillWindow(); // Tooltip appears if one of the button is clicked
}

void ImPersonaData::ArcanaPersonaComboFilters()
{
	static int OffSetID = 0;
	ImGui::AlignTextToFramePadding();

	// Arcana Combo Filter
	ImGui::Text("Arcana");
	ImGui::SameLine(0, 3.5f);
	if (ArcanaCombo.Draw(ImGui::GetContentRegionAvail().x / 2.35f))
	{
		// Finds the first instance of arcana then set the index offset IdIterStart from that iteration
		// So that ID will still correspond to the correct persona when a name is chosen from the persona combo filter
		// For "All Arcanas", IdIterStart is just zero
		static StrVec FilteredList;
		const char* selected_arcana = ArcanaCombo.GetPreview();

		if (strcmp(selected_arcana, "All Arcanas") == 0)
		{
			OffSetID     = 0;
			FilteredList = GD->Persona_List;
		}
		else
		{
			OffSetID = GD->GetArcanaIndex(selected_arcana);
			GD->GetFilteredPersonaList(GetPersonaFlag_Arcana, FilteredList, selected_arcana);
		}

		PersonaCombo.Reset(FilteredList);
		FilteredList.clear();
	}

	// Persona Combo Filter
	ImGui::SameLine(0, 3.5f);
	ImGui::Text("Persona");
	ImGui::SameLine(0, 3.5f);
	if (PersonaCombo.Draw(ImGui::GetContentRegionAvail().x))
	{
		int ID = OffSetID + PersonaCombo.GetID();
		SetPersonaData(ID);
	}
}

void ImPersonaData::PersonaGameExclusive()
{
	switch (GD->GetGameTitle())
	{
		case GameTitle_Persona3FES:
			break;
		case GameTitle_Persona3Portable:
			break;
		case GameTitle_Persona4Golden:
			//RenderSkillCard(PD->GetGameEx().SkillCard);
			break;
		case GameTitle_Persona5:
			break;
		default:
			break;
	}
}

void ImPersonaData::RenderSkillCard(const SkillCardData& scard)
{
	using namespace ImGui;

	PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
	if (!BeginChild("SkillCardData", ImVec2(200.0f, 50.0f), true))
	{
		EndChild();
		PopStyleVar();
		return;
	}


	EndChild();
	PopStyleVar();
}

void ImPersonaData::PopUpSkillWindow()
{
	if (OpenPopUpSkill)
	{
		static int duration = 180;
		static int timer;

		if (!ImGui::IsItemHovered())
		{
			OpenPopUpSkill = false;
			return;
		}

		if (ClickedPopUpSkill)
		{
			timer = duration;
			ClickedPopUpSkill = false;
		}

		ImGui::BeginTooltip();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::BeginGroup(); // Top Group Element Icon + Skill
		ImGui::Image((void*)(intptr_t)SkillElementIcons[PopUpSkill->Element]->GetTexture(), ImVec2(35.0f, 30.0f));

		ImGui::SameLine();
		ImGui::BeginGroup(); // Skill + Element Description

		ImGui::TextUnformatted(PopUpSkill->Name.c_str());
		ImGui::PushFont(io.Fonts->Fonts[2]);
		ImGui::TextUnformatted(GD->MainElementsChar[PopUpSkill->Element]);
		ImGui::SameLine();
		if (SelectedSkillLevel == 0)
			ImGui::TextUnformatted("Innate Skill");
		else
			ImGui::Text("Unlocked at: Lvl. %d", SelectedSkillLevel);
		ImGui::PopFont();

		ImGui::EndGroup();   // Skill + Element Description
		ImGui::EndGroup();   // Top Group Element Icon + Skill

		ImGui::PushTextWrapPos(250.0f);
		if (PopUpSkill->Element != PersonaElement_Passive && PopUpSkill->Element != PersonaElement_Undefined)
		{
			if(PopUpSkill->CostType == "HP")
				ImGui::Text("HP Cost: %d%%", PopUpSkill->Cost);
			else
				ImGui::Text("SP Cost: %d", PopUpSkill->Cost);

			if (PopUpSkill->Element != PersonaElement_Support)
			{
				ImGui::SameLine(90.0f);
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
				ImGui::SameLine();
				ImGui::Text("Crit Chance: %d%%", PopUpSkill->ChanceValue);
			}
		}
		ImGui::TextUnformatted(PopUpSkill->Description.c_str());
		ImGui::PopTextWrapPos();

		if (PD->GameEx.SkillCard.ID == PopUpSkill->ID)
		{
			ImVec2 imgsz = ImVec2(11.0f, 14.0f);
			ImGui::Image((void*)(intptr_t)SkillCardTexture.GetTexture(), imgsz);
			ImGui::SameLine(0, 3.50f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.22f, 0.45f, 0.70f, 0.95f));
			ImGui::TextUnformatted("Skill Card");
			ImGui::PopStyleColor();
		}

		timer--;
		if (timer == 0)
		{
			OpenPopUpSkill = false;
		}

		ImGui::EndTooltip();
	}
}

//------------------------------------------------------
// UTILITY FUNCTIONS
//------------------------------------------------------

void ImPersonaData::SetPersonaData(const int& _ID)
{
	PD = &(GD->Personas[_ID]);
}

void ImPersonaData::HoverElemIcons(const HoveredElemIcon_& icon)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(GD->MainElementsChar[icon]);
		ImGui::EndTooltip();
	}
}