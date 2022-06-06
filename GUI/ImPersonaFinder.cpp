#include "ImPersonaFinder.h"
#include "Fonts/IconsFontAwesome5.h"

ImPersonaFinder::ImPersonaFinder() :
	GD(nullptr),
	PersonaComboArr({
				    ("##PDR_P1"),
				    ("##PDR_P2"),
				    ("##PDR_P3"),
				    ("##PDR_P4"),
				    ("##PDR_P5"),
				    ("##PDR_P6"),
				    ("##PDR_P7"),
				    ("##PDR_P8"),
				    ("##PDR_P9"),
				    ("##PDR_P10"),
				    ("##PDR_P11"),
				    ("##PDR_P12"),
				    ("##PDR_P13"),
				    ("##PDR_P14"),
				    ("##PDR_P15"),
				    ("##PDR_P16")
			     }),
	ArcanaComboArr({
				    ("##PDR_A1"),
				    ("##PDR_A2"),
				    ("##PDR_A3"),
				    ("##PDR_A4"),
				    ("##PDR_A5"),
				    ("##PDR_A6"),
				    ("##PDR_A7"),
				    ("##PDR_A8"),
				    ("##PDR_A9"),
				    ("##PDR_A10"),
				    ("##PDR_A11"),
				    ("##PDR_A12"),
				    ("##PDR_A13"),
				    ("##PDR_A14"),
				    ("##PDR_A15"),
				    ("##PDR_A16")
				 })
{}

bool ImPersonaFinder::InitGameData(const Gamedata* g)
{
	GD = g;
	
	if (GD == nullptr || GD->Arcana_List.empty() || GD->Persona_List.empty() || GD->Personas.empty())
		return false;

	if (!RFinder.InitResultFinder(GD))
		return false;

	ResetAll();
	return true;
}

void ImPersonaFinder::DestroyList()
{
	m_ClearList();
}

void ImPersonaFinder::ResetAll()
{
	m_ClearList();
	m_ClearFilters();
	m_ResetCombo();
	m_ResetPreview();
	m_ResetCheckBox();
	m_ResetFilterFlags();
	m_ResetPersonaPairs();
}

void ImPersonaFinder::m_ClearList()
{
	PComboVec().swap(ResultList);
}

void ImPersonaFinder::m_ClearFilters()
{
	P1Filter.Clear();
	P2Filter.Clear();
	P3Filter.Clear();
	RPFilter.Clear();
	RAFilter.Clear();
}

void ImPersonaFinder::m_ResetCombo()
{
	static StrVec ATemp;
	ATemp = { "All Arcanas" };
	ATemp.insert(ATemp.end(), GD->Arcana_List.begin(), GD->Arcana_List.end());

	for (auto& A : ArcanaComboArr)
		A.Reset(&ATemp);

	for (auto& P : PersonaComboArr)
		P.Reset(GD->Persona_List);
}

void ImPersonaFinder::m_ResetPreview()
{
	ComboPreviewArr = {
						 "Set Persona 1" , "Set Persona 2" , "Set Persona 3",
						 "Set Persona 4" , "Set Persona 5" , "Set Persona 6",
						 "Set Persona 7" , "Set Persona 8" , "Set Persona 9",
						 "Set Persona 10", "Set Persona 11", "Set Persona 12",
						 "Set Persona 13", "Set Persona 14", "Set Persona 15",
						 "Set Persona 16"
	};
}

void ImPersonaFinder::m_ResetCheckBox()
{
	EnableComboArr = {
						 true,  true,  true,   // <* P1,  P2,  P3
						 false, false, false,  // <* P4,  P5,  P6
						 false, false, false,  // <* P7,  P8,  P9
						 false, false, false,  // <* P10, P11, P12
						 false, false, false,  // <* P13, P14, P15 
						 false				   // <* P16
	};
}

void ImPersonaFinder::m_ResetFilterFlags()
{
	for (auto& Flags : FilterFlagsArr)
		Flags = GetPersonaFlag_None;
}

void ImPersonaFinder::m_ResetPersonaPairs()
{
	for (auto& P : PersonaLevelPairArr)
		P.Reset();
}

void ImPersonaFinder::MainBody()
{
	ImGui::BeginGroup();
	m_Buttons();
	m_Combos();
	ImGui::EndGroup();
	ImGui::SameLine();
	ImGui::BeginGroup();
	m_Filters();
	m_Table();
	ImGui::EndGroup();
}

void ImPersonaFinder::m_Buttons()
{
	ImGuiIO& io = ImGui::GetIO();
	static bool error_tooltip = false;
	static float time = 0;

	if (ImGui::Button(ICON_FA_CHECK_DOUBLE" Get Results!", ImVec2(111.0f, 0)))
	{
		static constexpr int array_size = 16;
		PLvlPairVec InputPersonaList;
		InputPersonaList.reserve(array_size);

		for (int i = 0; i < array_size; i++)
			if (EnableComboArr[i] && PersonaLevelPairArr[i].Persona != -1)
				InputPersonaList.emplace_back(PersonaLevelPairArr[i]);

		if (InputPersonaList.size() < 2)
		{
			error_tooltip = true;
			time = 1.80f;
		}
		else if (RFinder.SetPersonaList(InputPersonaList))
		{
			PComboVec().swap(ResultList);
			RFinder.GetResults(ResultList);
		}
	}

	if (error_tooltip)
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(250.0f);
		ImGui::TextUnformatted(
			"Not enough number of set personas!\nMake sure you set a persona after you enable a box, "
			"or there is at least 2 personas set."
		);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();

		time -= io.DeltaTime;
		if (time <= 0)
			error_tooltip = false;
	}

	ImGui::SameLine(0, 2.50f);
	if (ImGui::Button(ICON_FA_UNDO" Reset", ImVec2(77.0f, 0)))
	{
		m_ResetCheckBox();
		m_ResetCombo();
		m_ResetPreview();
		m_ResetPersonaPairs();
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(250.0f);
		ImGui::TextUnformatted("Reset the combos and check boxes to its initial values");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	ImGui::SameLine(0, 2.50f);
	if (ImGui::Button(ICON_FA_ERASER" Clear", ImVec2(79.0f, 0)))
	{
		m_ClearList();
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(250.0f);
		ImGui::TextUnformatted("Clear the current results");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void ImPersonaFinder::m_Filters()
{
	ImGui::PushItemWidth(171.0f);
	P1Filter.DrawWithHint(ICON_FA_FILTER" Persona 1");

	ImGui::SameLine(0, 2.50f);
	ImGui::PushItemWidth(171.0f);
	P2Filter.DrawWithHint(ICON_FA_FILTER" Persona 2");

	ImGui::SameLine(0, 2.50f);
	ImGui::PushItemWidth(171.0f);
	P3Filter.DrawWithHint(ICON_FA_FILTER" Persona 3");

	ImGui::SameLine(0, 2.50f);
	ImGui::PushItemWidth(171.0f);
	RPFilter.DrawWithHint(ICON_FA_FILTER" Resultant Persona");

	ImGui::SameLine(0, 2.50f);
	ImGui::PushItemWidth(171.0f);
	RAFilter.DrawWithHint(ICON_FA_FILTER" Resultant Arcana");
}

void ImPersonaFinder::m_Table()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 10.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(3.00f, 4.00f));
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 10.0f);
	if (ImGui::BeginTable("pdr_Table##T", 5, ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInner | ImGuiTableFlags_RowBg | ImGuiTableFlags_PadOuterX, ImGui::GetContentRegionAvail()))
	{
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableNextColumn();
		ImGui::TableHeader("Persona 1");
		ImGui::TableNextColumn();
		ImGui::TableHeader("Persona 2");
		ImGui::TableNextColumn();
		ImGui::TableHeader("Persona 3");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			{
				ImGui::PushTextWrapPos(250.0f);
				ImGui::TextUnformatted("Persona 3 is the third persona. The persona with the highest current level of the three.");
				ImGui::PopTextWrapPos();
			}
			ImGui::EndTooltip();
		}
		ImGui::TableNextColumn();
		ImGui::TableHeader("Resultant Persona");
		ImGui::TableNextColumn();
		ImGui::TableHeader("Resultant Arcana");

		const  int& r_size = ResultList.size();
		static bool NoPersona3 = false;
		static bool NoResult = false;
		static const char* P3, *RP, *RA;

		for (int i = 0; i < r_size; i++)
		{
			NoPersona3 = ResultList[i].P3 < 0;
			NoResult   = ResultList[i].Result < 0;
			P3         = NoPersona3 ? "N/A" : GD->Personas[ResultList[i].P3].Name.c_str();
			RP         = NoResult   ? "N/A" : GD->Personas[ResultList[i].Result].Name.c_str();
			RA         = NoResult   ? "N/A" : GD->Personas[ResultList[i].Result].Arcana.c_str();

			if ( 
				 P1Filter.PassFilter(GD->Personas[ResultList[i].P1].Name.c_str()) &&
				 P2Filter.PassFilter(GD->Personas[ResultList[i].P2].Name.c_str()) &&
				 P3Filter.PassFilter(P3) &&
				 RPFilter.PassFilter(RP) && 
				 RAFilter.PassFilter(RA)
			   )
			{
				ImGui::TableNextRow(ImGuiTableRowFlags_None, 18.0f);
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(GD->Personas[ResultList[i].P1].Name.c_str());

				ImGui::TableNextColumn();
				ImGui::TextUnformatted(GD->Personas[ResultList[i].P2].Name.c_str());

				ImGui::TableNextColumn();
				ImGui::PushStyleColor(ImGuiCol_Text, NoPersona3 ? ImVec4(1.00f, 0.10f, 0.10f, 1.0f) : ImVec4(1.00f, 1.00f, 1.00f, 1.0f));
				ImGui::TextUnformatted(NoPersona3 ? "N/A" : GD->Personas[ResultList[i].P3].Name.c_str());
				ImGui::PopStyleColor();

				ImGui::TableNextColumn();
				ImGui::PushStyleColor(ImGuiCol_Text, NoResult ? ImVec4(1.00f, 0.10f, 0.10f, 1.0f) : ImVec4(1.00f, 1.00f, 1.00f, 1.0f));
				ImGui::TextUnformatted(NoResult ? "N/A" : GD->Personas[ResultList[i].Result].Name.c_str());
				ImGui::PopStyleColor();

				ImGui::TableNextColumn();
				ImGui::PushStyleColor(ImGuiCol_Text, NoResult ? ImVec4(1.00f, 0.10f, 0.10f, 1.0f) : ImVec4(1.00f, 1.00f, 1.00f, 1.0f));
				ImGui::TextUnformatted(NoResult ? "N/A" : GD->Personas[ResultList[i].Result].Arcana.c_str());
				ImGui::PopStyleColor();
			}
		}

		ImGui::EndTable();
	}
	ImGui::PopStyleVar(3);
}

void ImPersonaFinder::m_Combos()
{
	static constexpr int arr_size = 16;
	static constexpr std::array<const char*, arr_size> IntPutLabel = {
															  "##P1IntPut",  "##P2IntPut",  "##P3IntPut",
															  "##P4IntPut",  "##P5IntPut",  "##P6IntPut",
															  "##P7IntPut",  "##P8IntPut",  "##P9IntPut",
															  "##P10IntPut", "##P11IntPut", "##P12IntPut",
															  "##P13IntPut", "##P14IntPut", "##P15IntPut",
															  "##P16IntPut"
															   };
	static constexpr std::array<const char*, arr_size> ComboLabel = {
															 "##P1_mCombo",  "##P2_mCombo",  "##P3_mCombo",
															 "##P4_mCombo",  "##P5_mCombo",  "##P6_mCombo",
															 "##P7_mCombo",  "##P8_mCombo",  "##P9_mCombo",
															 "##P10_mCombo", "##P11_mCombo", "##P12_mCombo",
															 "##P13_mCombo", "##P14_mCombo", "##P15_mCombo",
															 "##P16_mCombo"
															  };
	static constexpr std::array<const char*, arr_size> CheckBoxLabel = {
																"##P1CBox",  "##P2CBox",  "##P3CBox",
																"##P4CBox",  "##P5CBox",  "##P6CBox",
																"##P7CBox",  "##P8CBox",  "##P9CBox",
																"##P10CBox", "##P11CBox", "##P12CBox",
																"##P13CBox", "##P14CBox", "##P15CBox",
																"##P16CBox"
																 };

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));
	for (int i = 0; i < arr_size; i++)
	{
		ImGui::Checkbox(CheckBoxLabel[i], &EnableComboArr[i]);

		ImGui::SameLine(0, 2.50f);
		m_ComboHelper( ComboLabel[i], IntPutLabel[i], i	);
	}
	ImGui::PopStyleVar();
}

void ImPersonaFinder::m_ComboHelper(const char* combo_label, const char* intput_label, const int& arr_idx)
{
	ImGui::PushDisabled(!EnableComboArr[arr_idx]);

	ImGui::PushItemWidth(250.0f);
	if (ImGui::BeginCombo(combo_label, ComboPreviewArr[arr_idx]))
	{
		static StrVec FilteredList;
		char(&preview)[150]            = ComboPreviewArr[arr_idx];
		PersonaLevelPair& PL           = PersonaLevelPairArr[arr_idx];
		ComboFilterPtr& ACombo         = ArcanaComboArr[arr_idx];
		ComboFilterFunc& PCombo        = PersonaComboArr[arr_idx];
		GetPersonaFilter& FilterFlags  = FilterFlagsArr[arr_idx];
								       
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Arcana: ");
		ImGui::SameLine(55.0f);
		if (ACombo.Draw(220.0f))
		{
			if (ACombo.GetID() == 0)
				FilterFlags ^= GetPersonaFlag_Arcana;
			else
				FilterFlags |= GetPersonaFlag_Arcana;

			GD->GetFilteredPersonaList(FilterFlags, FilteredList, ACombo.GetPreview());
			PCombo.Reset(FilteredList);
			FilteredList.clear();

			if (PCombo.size == 0)
			{
				PL.Persona = -1;
				PL.Level = 0;
				memcpy(preview, "N/A", strlen("N/A") + 1);
			}
			else
			{
				PL.Persona = GD->GetPersonaIndex(PCombo.GetPreview());
				PL.Level = GD->Personas[PL.Persona].Level;
				sprintf(preview, "%d %s || %s", PL.Level, GD->Personas[PL.Persona].Name.c_str(), GD->Personas[PL.Persona].Arcana.c_str());
			}
		}

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Persona: ");
		ImGui::SameLine(55.0f);
		if (PCombo.Draw(220.0f))
		{
			PL.Persona = GD->GetPersonaIndex(PCombo.GetPreview());
			PL.Level   = GD->Personas[PL.Persona].Level;
			sprintf(preview, "%d %s || %s", PL.Level, GD->Personas[PL.Persona].Name.c_str(), GD->Personas[PL.Persona].Arcana.c_str());
		}

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Level: ");
		ImGui::SameLine(55.0f);
		ImGui::PushItemWidth(60.0f);
		if (ImGui::InputInt(intput_label, &PL.Level, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			if (PL.Level < GD->Personas[PL.Persona].Level)
				PL.Level = GD->Personas[PL.Persona].Level;

			if (PL.Level > 100)
				PL.Level = 100;

			sprintf(preview, "%d %s || %s", PL.Level, GD->Personas[PL.Persona].Name.c_str(), GD->Personas[PL.Persona].Arcana.c_str());
		}

		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();

		ImGui::SameLine(0, 3.0f);
		ImGui::BeginGroup();
		{
			ImGui::CheckboxFlags("Special Personas##PRcp", &FilterFlags, GetPersonaFlag_Special);
			ImGui::SameLine(0, 3.0f);
			ImGui::CheckboxFlags("Max Personas##PRcp", &FilterFlags, GetPersonaFlag_Max);
		}
		ImGui::EndGroup();
		if (ImGui::IsItemDeactivated())
		{
			GD->GetFilteredPersonaList(FilterFlags, FilteredList, ACombo.GetPreview());
			PCombo.Reset(FilteredList);
			FilteredList.clear();

			if (PCombo.size == 0)
			{
				PL.Persona = -1;
				PL.Level = 0;
				memcpy(preview, "N/A", strlen("N/A") + 1);
			}
			else
			{
				PL.Persona = GD->GetPersonaIndex(PCombo.GetPreview());
				PL.Level = GD->Personas[PL.Persona].Level;
				sprintf(preview, "%d %s || %s", PL.Level, GD->Personas[PL.Persona].Name.c_str(), GD->Personas[PL.Persona].Arcana.c_str());
			}
		}

		ImGui::EndCombo();
	}

	ImGui::PopDisabled();
}