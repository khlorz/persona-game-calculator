#include "PersonaRecipe.h"
#include "Fonts/IconsFontAwesome5.h"

static void HoverToolTip(const char* desc);

ImRcp::ImRcp() :
	GD(nullptr),
	PCombo("##PComboPRcp"),
	ACombo("##AComboPRcp")
{
}

ImRcp::~ImRcp()
{

}

bool ImRcp::InitGameData(const Gamedata* data)
{
	// Stop any ongoing process
	StopProcess();
	GD = data;

	if (GD == nullptr || GD->Personas.empty() || GD->Arcana_List.empty() || GD->Persona_List.empty())
		return false;

	if (!RCalculator.InitReverseCalc(data))
		return false;

	ResetAll();
	return true;
}

void ImRcp::ResetAll()
{
	m_ClearFilters();
	m_ResetValues();
	m_ResetCombo();
	m_ClearList();
}

void ImRcp::DestroyList()
{
	m_ClearList();
}

void ImRcp::StopProcess()
{
	RCalculator.SetNoProcess();
}

void ImRcp::MainBody()
{
	m_Buttons();
	m_RcpProcess();
	m_Table();
}


//-----------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
// RECIPE BODY AND FUNCTIONS SECTION
//-----------------------------------------------------------------------------------------------------

void ImRcp::m_ResetCombo()
{
	// Create the list with "All Arcanas" + Arcanas
	static StrVec ArcanaChoices = { "All Arcanas" };
	ArcanaChoices.insert(ArcanaChoices.end(), GD->Arcana_List.begin(), GD->Arcana_List.end());

	ACombo.Reset(&ArcanaChoices);
	PCombo.Reset(GD->Persona_List);
}

void ImRcp::m_ResetValues()
{
	FilterFlags    = GetPersonaFlag_None;
	PageChanged    = false;
	ChosenID       = -1;
	ColumnNum      = 4;
	StartingNum    = 0;
	EndingNum      = 0;
	NumOfItems     = 100;
	ShowPersona3   = false;
	FusionState    = FusionMode_Normal;
	ComboPreview   = "Set Persona";
}

void ImRcp::m_ClearFilters()
{
	P1F.Clear();
	P2F.Clear();
	P3F.Clear();
	A1F.Clear();
	A2F.Clear();
	A3F.Clear();
}

void ImRcp::m_ClearList()
{
	PComboVec().swap(ResultList);
}

void ImRcp::m_Buttons()
{
	// The Combo Filter
	m_ComboHelper(245.0f);
	
	ImGui::SameLine(0, 3.0f);
	ImGui::BeginGroup();
	{
		if (ImGui::Button("Normal##PRcp", ImVec2(70, 0)))
			FusionState = FusionMode_Normal;

		ImGui::SameLine(0, 3.0f);
		if (ImGui::Button("Triangle##PRcp", ImVec2(70, 0)))
			FusionState = FusionMode_Triangle;
	}
	ImGui::EndGroup();
	if (ImGui::IsItemDeactivated())
	{
		PComboVec().swap(ResultList);

		if (!RCalculator.SetReversePersona(ChosenID, FusionState))
			InitMakeRcp = false;
		else
			InitMakeRcp = true;
	}

	ImGui::SameLine(0.0f, 3.0f);
	{
		static bool error_tooltip = false;
		if (ImGui::Button(ICON_FA_ERASER" Clear List##PRcp", ImVec2(100, 0)))
		{
			if (RCalculator.IsOngoing())
				error_tooltip = true;
			else
			{
				EndingNum   = 0;
				StartingNum = 0;
				PComboVec().swap(ResultList);
			}
		}
		if (ImGui::IsItemHovered() && error_tooltip)
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos();
			ImGui::TextUnformatted("You must first stop the process before clearing the list!");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	ImGui::SameLine(0.0f, 3.0f);
	ImGui::PushItemWidth(240.0f);
	if (ImGui::SliderInt("##Number of items", &NumOfItems, 1, 1000, "Number of items: %d", ImGuiSliderFlags_AlwaysClamp))
	{
		if (ResultList.size() > 0)
		{
			EndingNum = NumOfItems + StartingNum;
			if (EndingNum > ResultList.size())
			{
				EndingNum = ResultList.size();
			}
		}
	}

	ImGui::SameLine(0.0f, 3.0f);
	ImGui::PushButtonRepeat(true); // Previous Page and Next Page buttons
	{
		if (ImGui::Button(ICON_FA_ARROW_CIRCLE_LEFT"##PreviousPage", ImVec2(50, 0)))
		{
			if (StartingNum > 0)
			{
				if (EndingNum - StartingNum != NumOfItems)
				{
					const size_t& rcp_sz = ResultList.size();
					StartingNum -= NumOfItems;
					EndingNum = StartingNum + NumOfItems;

					if (StartingNum < 0)
						StartingNum = 0;

					if (NumOfItems >= rcp_sz)
						EndingNum = rcp_sz;
					else if ((NumOfItems < rcp_sz && NumOfItems > EndingNum))
						EndingNum = NumOfItems;
				}
				else
				{
					StartingNum -= NumOfItems;
					EndingNum -= NumOfItems;
					if (EndingNum < NumOfItems)
					{
						StartingNum = 0;
						EndingNum = NumOfItems;
					}
				}
			}

			PageChanged = true;
		}
		HoverToolTip("Previous set of recipes");

		ImGui::SameLine(0.0f, 3.0f);
		if (ImGui::Button(ICON_FA_ARROW_CIRCLE_RIGHT"##NextPage", ImVec2(50, 0)))
		{
			const int& rcp_size = ResultList.size();

			if (EndingNum < rcp_size)
			{
				StartingNum += NumOfItems;
				EndingNum += NumOfItems;
				if (EndingNum > rcp_size)
				{
					EndingNum = rcp_size;
					if (EndingNum < StartingNum)
						StartingNum -= NumOfItems;
				}

				PageChanged = true;
			}
		}
		HoverToolTip("Next set of recipes");
	}
	ImGui::PopButtonRepeat(); // Previous Page and Next Page buttons

	ImGui::SameLine(0, 3.0f);
	ImGui::Button(ICON_FA_COGS, ImVec2(40.0f, 0));

	{
		ImGui::SameLine(898.0f);
		ImGui::Text("Recipes: %d", (ResultList.size() > 0 || RCalculator.IsSpecial()) ? StartingNum + 1 : 0);
		ImGui::SameLine(1000.0f);
		ImGui::Text("to %d", RCalculator.IsSpecial() ? 1 : EndingNum);
		ImGui::SameLine(1067.0f);
		ImGui::Text("Total: %d", RCalculator.IsSpecial() ? 1 : ResultList.size());
	}
}

void ImRcp::m_RcpProcess()
{
	// The main process of creating recipes
	// Only true if there is a process ongoing, false if paused, done or force stopped
	if (!RCalculator.ReverseFusion(ResultList, SpecialResult))
		return;

	// Initialize the important values (number of columns, starting iteration, current items)
	static bool non_sp = true;
	if (InitMakeRcp)
	{
		StartingNum = 0;
		InitMakeRcp = false;
		if (RCalculator.IsSpecial())
		{
			non_sp = false;
			ColumnNum = 3;
			EndingNum = 1;
		}
		else
		{
			non_sp = true;
			ShowPersona3 = FusionState == FusionMode_Triangle;
			ColumnNum = ShowPersona3 ? 6 : 4;
		}
	}
	if (non_sp)
	{
		const int& rcp_size = ResultList.size();
		if (rcp_size <= NumOfItems)
		{
			EndingNum = rcp_size;
		}
		else
		{
			EndingNum = NumOfItems + StartingNum;
			if (EndingNum > rcp_size)
			{
				EndingNum = rcp_size;
			}
		}
	}
}

void ImRcp::m_Filters()
{
	//------------------------------------
	// Persona 1 Filter
	//------------------------------------
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / (ShowPersona3 ? 6.0f : 4.0f)); // 6.0f :  4.0f  3.850f : 6.0f
	P1F.DrawWithHint(ICON_FA_FILTER" Persona 1");

	//------------------------------------
	// Arcana 1 Filter
	//------------------------------------
	ImGui::SameLine(0, 1.5f);
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / (ShowPersona3 ? 5.1f : 3.02f)); //  5.10f : 3.02f  3.22 : 5.04f
	A1F.DrawWithHint(ICON_FA_FILTER" Arcana 1");

	//------------------------------------
	// Persona 2 Filter
	//------------------------------------
	ImGui::SameLine(0, 1.5f);
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / (ShowPersona3 ? 4.03f : 2.01f)); //  4.03f : 2.01f  1.920 : 4.02f
	P2F.DrawWithHint(ICON_FA_FILTER" Persona 2");

	//------------------------------------
	// Arcana 2 Filter
	//------------------------------------
	ImGui::SameLine(0, 1.5f);
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / (ShowPersona3 ? 3.03f : 1.0f)); // 3.03f : 1 : 3.04f
	A2F.DrawWithHint(ICON_FA_FILTER" Arcana 2");

	if (ShowPersona3)
	{
		//------------------------------------
		// Persona 3 Filter
		//------------------------------------
		ImGui::SameLine(0, 1.5f);
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 2.02f); // 2.02f
		P3F.DrawWithHint(ICON_FA_FILTER" Persona 3");

		//------------------------------------
		// Arcana 3 Filter
		//------------------------------------
		ImGui::SameLine(0, 1.5f);
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		A3F.DrawWithHint(ICON_FA_FILTER" Arcana 3");
	}
}

void ImRcp::m_Table()
{
	if (!RCalculator.IsSpecial())
		m_Filters();

	// Main Table
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(3.00f, 4.00f));
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 10.0f);
	if (!ImGui::BeginTable("Recipe Table##rcp", ColumnNum, ImGuiTableFlags_BordersInner | ImGuiTableFlags_RowBg | ImGuiTableFlags_PadOuterX | ImGuiTableFlags_Hideable | ImGuiTableFlags_ScrollY, ImGui::GetContentRegionAvail()))
	{
		ImGui::PopStyleVar(2);
		return;
	}
	
	ImGui::TableSetupScrollFreeze(0, 1);
	if (PageChanged)
	{
		ImGui::SetScrollY(0.0f);
		PageChanged = false;
	}

	if (RCalculator.IsSpecial())
	{
		ImGui::TableSetupColumn("Special Persona");
		ImGui::TableSetupColumn("Persona");
		ImGui::TableSetupColumn("Arcana");
		ImGui::TableHeadersRow();

		int i = 0;
		for (auto& SP : SpecialResult.PersonaCombos)
		{
			i++;
			ImGui::TableNextRow(ImGuiTableRowFlags_None, 18.0f);
			ImGui::TableNextColumn();
			ImGui::Text("Persona %d", i);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(GD->Personas[SP].Name.c_str());
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(GD->Personas[SP].Arcana.c_str());
		}

	}
	else
	{
		ImGui::TableSetupColumn("Persona 1");
		ImGui::TableSetupColumn("Arcana 1");
		ImGui::TableSetupColumn("Persona 2");
		ImGui::TableSetupColumn("Arcana 2");
		if (ShowPersona3)
		{
			ImGui::TableSetupColumn("Persona 3");
			ImGui::TableSetupColumn("Arcana 3");
		}
		ImGui::TableHeadersRow();

		for (int i = StartingNum; i < EndingNum; i++)
		{
			// Filters all the results
			if ( P1F.PassFilter(GD->Personas[ResultList[i].P1].Name.c_str()) && A1F.PassFilter(GD->Personas[ResultList[i].P1].Arcana.c_str()) && 
				 P2F.PassFilter(GD->Personas[ResultList[i].P2].Name.c_str()) && A2F.PassFilter(GD->Personas[ResultList[i].P2].Arcana.c_str()) && 
				(!ShowPersona3 || (P3F.PassFilter(GD->Personas[ResultList[i].P3].Name.c_str()) && A3F.PassFilter(GD->Personas[ResultList[i].P3].Arcana.c_str()))))
			{
				ImGui::TableNextRow(ImGuiTableRowFlags_None, 18.0f);
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(GD->Personas[ResultList[i].P1].Name.c_str());
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(GD->Personas[ResultList[i].P1].Arcana.c_str());
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(GD->Personas[ResultList[i].P2].Name.c_str());
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(GD->Personas[ResultList[i].P2].Arcana.c_str());
				if (ShowPersona3)
				{
					ImGui::TableNextColumn();
					ImGui::TextUnformatted(GD->Personas[ResultList[i].P3].Name.c_str());
					ImGui::TableNextColumn();
					ImGui::TextUnformatted(GD->Personas[ResultList[i].P3].Arcana.c_str());
				}
			}
		}
	}

	ImGui::EndTable();
	ImGui::PopStyleVar(2);
}

void ImRcp::m_ComboHelper(const float& size)
{
	ImGui::PushItemWidth(size);
	if(ImGui::BeginCombo("##RcpCombo", ComboPreview))
	{
		static StrVec FilteredList;

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Arcana:");
		ImGui::SameLine(53.0f);

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
				ChosenID     = -1;
				ComboPreview = "N/A";
			}
			else
			{
				ChosenID     = GD->GetPersonaIndex(PCombo.GetPreview());
				ComboPreview = GD->Personas[ChosenID].Name.c_str();
			}
		}

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Persona:");
		ImGui::SameLine(53.0f);
		if (PCombo.Draw(220.0f))
		{
			ChosenID     = GD->GetPersonaIndex(PCombo.GetPreview());
			ComboPreview = GD->Personas[ChosenID].Name.c_str();
		}

		if(ImGui::Button("Close##RCPC"))
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
				ComboPreview = "N/A";
			}
			else
			{
				ChosenID = GD->GetPersonaIndex(PCombo.GetPreview());
				ComboPreview = GD->Personas[ChosenID].Name.c_str();
			}
		}

		ImGui::EndCombo();
	}
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
