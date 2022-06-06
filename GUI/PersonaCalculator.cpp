#include "PersonaCalculator.h"
#include "Fonts/IconsFontAwesome5.h"

enum ThirdPersona_
{
	ThirdPersona_P1,
	ThirdPersona_P2,
	ThirdPersona_P3
};

// Shows a message when hovered onto something
static void HoverToolTip(const char* desc);

ImCalc::ImCalc() :
	GD(nullptr),
	ResultantID(CalcError_NoResult),
	AComboArr({
				("##AC1Rcp"),
				("##AC2Rcp"),
				("##AC3Rcp")
			 }),
	PComboArr({
				("##PC1Rcp"),
				("##PC2Rcp"),
				("##PC3Rcp"),
			 })
{
}

ImCalc::~ImCalc()
{
	DestroyList();
}

void ImCalc::SetBodyDimensions( float _w, float _h )
{
	MainBodyWidth  = _w;
	MainBodyHeight = _h;
}

bool ImCalc::InitGameData(const Gamedata* data)
{
	GD = data;

	if (GD == nullptr || GD->Personas.empty() || GD->Arcana_List.empty() || GD->Persona_List.empty())
		return false;

	if (!TheCalculator.InitCalc(GD))
		return false;

	ResetAll();
	return true;
}

void ImCalc::ResetAll()
{
	m_ResetValues();
	m_ResetCombo();
}

void ImCalc::m_ResetCombo()
{
	StrVec ATemp = { "All Arcanas" };
	ATemp.insert(ATemp.end(), GD->Arcana_List.begin(), GD->Arcana_List.end());

	for (auto& A : AComboArr)
		A.Reset(ATemp);

	for (auto& P : PComboArr)
		P.Reset(GD->Persona_List);
}

void ImCalc::m_ResetValues()
{
	SameLevel       = false;
	ResultantID     = CalcError_NoResult;
	ThirdPersona    = ThirdPersona_P3;
	FusionMode      = FusionMode_Normal;
	ChosenIDArr     = { -1, -1, -1 };
	ComboPreviewArr = { "Set Persona 1", "Set Persona 2", "Set Persona 3" };
	FilterFlagsArr  = { GetPersonaFlag_None, GetPersonaFlag_None, GetPersonaFlag_None };
}

void ImCalc::DestroyList()
{
	
}

void ImCalc::CalculatorBody(bool *w_open)
{
	ImGui::SetNextWindowSize(ImVec2(500.0f, 155.0f));
	if (!ImGui::Begin("Persona Calculator##pcalc", w_open, ImGuiWindowFlags_NoResize))
	{
		ImGui::End();
		return;
	}

	//------------------------
	// CHOICES TABLE
	//------------------------
	m_ChoicesTable();

	ImGui::SameLine();

	//------------------------
	// RESULT TABLE
	//------------------------
	m_ResultTable();

	ImGui::End();
}

void ImCalc::m_ChoicesTable()
{

	static constexpr std::array<const char*, 3> ComboLabel     = { "##PCalcP1C", "##PCalcP2C", "##PCalcP3C" };
	static constexpr std::array<const char*, 3> RadioLabel     = { "##RadioP1", "##RadioP2", "##RadioP3" };

	if (!ImGui::BeginChild("##ChoicesRcp", ImVec2(260.0f, ImGui::GetContentRegionAvail().y)))
	{
		ImGui::EndChild();
		return;
	}

	ImGui::RadioButton("Normal Fusion", &FusionMode, FusionMode_Normal);
	ImGui::SameLine();
	ImGui::RadioButton("Triangle Fusion", &FusionMode, FusionMode_Triangle);

	for (int i = 0; i < 3; i++)
	{
		ImGui::PushDisabled(i == 2 && FusionMode != FusionMode_Triangle);
		{
			m_ComboHelper(ComboLabel[i], 230.0f, i);

			ImGui::PushDisabled(FusionMode != FusionMode_Triangle || SameLevel);
			{
				ImGui::SameLine();
				ImGui::RadioButton(RadioLabel[i], &ThirdPersona, i);
				if (ImGui::IsItemHovered())
				{
					static char buf[50];
					sprintf(buf, "Set Persona %d as the third persona", i + 1);
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(250.0f);
					ImGui::TextUnformatted(buf);
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
			}
			ImGui::PopDisabled();
		}
		ImGui::PopDisabled();
	}

	if (ImGui::Button("Get Resultant Persona!", ImVec2(230.0f, 0)))
	{
		switch (FusionMode)
		{
		case FusionMode_Normal:
			ResultantID = TheCalculator.CalculatePersona(FusionMode_Normal, ChosenIDArr[0], ChosenIDArr[1]);
			break;

		case FusionMode_Triangle:
		{
			const int& P1 = ChosenIDArr[0];
			const int& P2 = ChosenIDArr[1];
			const int& P3 = ChosenIDArr[2];

			switch (ThirdPersona)
			{
			case ThirdPersona_P1: ResultantID = TheCalculator.CalculatePersona(FusionMode_Triangle, P3, P2, P1, SameLevel); break;
			case ThirdPersona_P2: ResultantID = TheCalculator.CalculatePersona(FusionMode_Triangle, P1, P3, P2, SameLevel); break;
			case ThirdPersona_P3: ResultantID = TheCalculator.CalculatePersona(FusionMode_Triangle, P1, P2, P3, SameLevel); break;
			default:			  ResultantID = CalcError_NoResult; break;
			}

			break;
		}

		default:
			ResultantID = CalcError_NoResult;
			break;
		}
	}

	ImGui::SameLine(0, 3.0f);
	ImGui::PushDisabled(FusionMode != FusionMode_Triangle);
	{
		ImGui::SameLine();
		ImGui::Checkbox("##SameLvlPRcp", &SameLevel);
		HoverToolTip("Apply same level fusion for triangle fusion");
	}
	ImGui::PopDisabled();

	ImGui::EndChild();
}

void ImCalc::m_ComboHelper(const char* label, const float& size, const int& arr_idx)
{
	ImGui::PushItemWidth(size);
	if (ImGui::BeginCombo(label, ComboPreviewArr[arr_idx]))
	{
		static StrVec FilteredList;
		int& ChosenID                 = ChosenIDArr[arr_idx];
		char(&preview)[100]           = ComboPreviewArr[arr_idx];
		ComboFilterFunc& ACombo       = AComboArr[arr_idx];
		ComboFilterFunc& PCombo       = PComboArr[arr_idx];
		GetPersonaFilter& FilterFlags = FilterFlagsArr[arr_idx];

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Arcana");
		ImGui::SameLine(53.0f);
		if (ACombo.Draw(200.0f))
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
				ChosenID = -1;
				memcpy(preview, "N/A", strlen("N/A") + 1);
			}
			else
			{
				ChosenID = GD->GetPersonaIndex(PCombo.GetPreview());
				sprintf(preview, "%s: %s", GD->Personas[ChosenID].Arcana.c_str(), GD->Personas[ChosenID].Name.c_str());
			}
		}

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Persona:");
		ImGui::SameLine(53.0f);
		if (PCombo.Draw(200.0f))
		{
			ChosenID = GD->GetPersonaIndex(PCombo.GetPreview());
			sprintf_s(preview, "%s: %s", GD->Personas[ChosenID].Arcana.c_str(), GD->Personas[ChosenID].Name.c_str());
		}

		if (ImGui::Button("Close##PCalc"))
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
				ChosenID = -1;
				memcpy(preview, "N/A", strlen("N/A") + 1);
			}
			else
			{
				ChosenID = GD->GetPersonaIndex(PCombo.GetPreview());
				sprintf_s(preview, "%s: %s", GD->Personas[ChosenID].Arcana.c_str(), GD->Personas[ChosenID].Name.c_str());
			}
		}

		ImGui::EndCombo();
	}
}

void ImCalc::m_ResultTable()
{
	ImGui::BeginGroup();
	if (ImGui::BeginTable("PName##PCalc", 1, ImGuiTableFlags_PadOuterX))
	{
		ImGui::TableSetupColumn("Resultant Persona");
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(ResultantID >= 0 ? GD->Personas[ResultantID].Name.c_str() : (ResultantID == CalcError_Invalid ? "INVALID" : "N/A"));

		ImGui::EndTable();
	}
	if (ImGui::BeginTable("PArcana##PCalc", 1, ImGuiTableFlags_PadOuterX))
	{
		ImGui::TableSetupColumn("Arcana");
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(ResultantID >= 0 ? GD->Personas[ResultantID].Arcana.c_str() : "N/A");

		ImGui::EndTable();
	}
	if (ImGui::BeginTable("LvlMaxSp##PCalc", 3, ImGuiTableFlags_PadOuterX | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV))
	{
		ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_None, 25.0f);
		ImGui::TableSetupColumn("Max",   ImGuiTableColumnFlags_None, 25.0f);
		ImGui::TableSetupColumn("Special Persona", ImGuiTableColumnFlags_None, 50.0f);
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text("%d", ResultantID >= 0 ? GD->Personas[ResultantID].Level : 0);

		ImGui::TableNextColumn();
		const bool& MaxPersona = ResultantID >= 0 && GD->Personas[ResultantID].Max;
		ImGui::PushStyleColor(ImGuiCol_Text, MaxPersona ? ImVec4(0.50f, 0.50f, 8.00f, 1.0f) : ImVec4(1.00f, 0.10f, 0.10f, 1.0f));
		ImGui::TextUnformatted(MaxPersona ? "YES" : "NO");
		ImGui::PopStyleColor();

		ImGui::TableNextColumn();
		const bool& SpPersona = ResultantID >= 0 && GD->Personas[ResultantID].Special;
		ImGui::PushStyleColor(ImGuiCol_Text, SpPersona ? ImVec4(0.50f, 0.50f, 8.00f, 1.0f) : ImVec4(1.00f, 0.10f, 0.10f, 1.0f));
		ImGui::TextUnformatted(SpPersona ? "YES" : "NO");
		ImGui::PopStyleColor();

		ImGui::EndTable();
	}
	ImGui::EndGroup();
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


