#include "ImFusionWindow.h"
#include "imgui_internal.h"
#include "Fonts/IconsFontAwesome5.h"
#include "imgui_markdown.h"

ImFusionWindow::ImFusionWindow() :
	GD(nullptr),
	SelectedFusion(nullptr)
{}

bool ImFusionWindow::InitializeWindow(const Gamedata* gd)
{
	GD = gd;

	if (GD == nullptr)
		return false;

	ResetValues();
	return true;
}

void ImFusionWindow::MainWindow(bool* w_open)
{
	ImGui::SetNextWindowSize(ImVec2(500.0f, 350.0f), ImGuiCond_Appearing);
	if (!ImGui::Begin("Fusion Table Window##FW", w_open, ImGuiWindowFlags_NoResize))
	{
		ImGui::End();
		return;
	}

	m_TabBar();
	m_Body();

	ImGui::End();
}

void ImFusionWindow::ErrorWindow(bool* w_open)
{
	ImGui::SetNextWindowSize(ImVec2(200.0f, 50.0f), ImGuiCond_Appearing);
	if (!ImGui::Begin("No Loaded Fusion Data", w_open, ImGuiWindowFlags_NoResize))
	{
		ImGui::End();
		return;
	}

	ImGui::PushTextWrapPos(200.0f);
	ImGui::TextUnformatted("No fusion data loaded. Check for the location/name/extension of the file. "
		                   "It must be a [Game Abbreviation, i.e Persona 4 Golden -> p4g] + _fusion.dat file in a PersonaData folder");
	ImGui::PopTextWrapPos();

	ImGui::End();
}

void ImFusionWindow::ResetValues()
{
	TabItemChoice      = TabItemChoice_NormalFusion;
	SelectedFusion     = &(GD->NFusionData);
	FusionTableColumns = 3;
	A1TextFilter.Clear();
	A2TextFilter.Clear();
	ResultantTextFilter.Clear();
}

void ImFusionWindow::m_ResetFilters()
{
	A1TextFilter.Clear();
	A2TextFilter.Clear();
	ResultantTextFilter.Clear();
}

void ImFusionWindow::m_TabBar()
{
	static constexpr const char* TabItems[] = { "Normal Fusion", "Triangle Fusion", "Special Fusion", "More Info" };
	static constexpr int TabItemSize = 4;

	if (ImGui::BeginTabBar("FusionTabBar"))
	{
		for (int i = 0; i < TabItemSize; i++)
		{
			if (ImGui::BeginTabItem(TabItems[i]) )
			{
				if (TabItemChoice != i)
				{
					TabItemChoice = i;
					switch (TabItemChoice)
					{
					case TabItemChoice_NormalFusion:
						m_ResetFilters();
						SelectedFusion     = &(GD->NFusionData);
						FusionTableColumns = 3;
						break;

					case TabItemChoice_TriangleFusion:
						m_ResetFilters();
						SelectedFusion     = &(GD->TFusionData);
						FusionTableColumns = 3;
						break;

					case TabItemChoice_SpecialFusion:
						FusionTableColumns = 2;
						break;

					default:
						break;
					}

					TabChanged = true;
				}

				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}
}

void ImFusionWindow::m_Body()
{
	if (TabItemChoice == TabItemChoice_MoreInfo)
		m_MoreInfoBody();

	else
		m_FusionTable();
}

void ImFusionWindow::m_FusionTable()
{
	if (TabItemChoice != TabItemChoice_SpecialFusion)
	{
		ImGui::PushItemWidth(155.0f);
		A1TextFilter.DrawWithHint(ICON_FA_FILTER" Arcana 1");

		ImGui::SameLine(0, 2.50f);
		ImGui::PushItemWidth(159.0f);
		A2TextFilter.DrawWithHint(ICON_FA_FILTER" Arcana 2");

		ImGui::SameLine(0, 2.50f);
		ImGui::PushItemWidth(165.0f);
		ResultantTextFilter.DrawWithHint(ICON_FA_FILTER" Resultant Arcana");
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 10.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(3.5f, 4.0f));
	if (ImGui::BeginTable("FusionTable", FusionTableColumns, ImGuiTableFlags_PadOuterX | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInner | ImGuiTableFlags_ScrollY, ImGui::GetContentRegionAvail()))
	{
		if (TabChanged)
		{
			ImGui::SetScrollY(0.0f);
			TabChanged = false;
		}

		ImGui::TableSetupScrollFreeze(0, 1);
		if (TabItemChoice == TabItemChoice_SpecialFusion)
		{
			ImGui::TableSetupColumn("Special Persona", ImGuiTableColumnFlags_None, 30.0f);
			ImGui::TableSetupColumn("Persona Recipe",  ImGuiTableColumnFlags_None, 70.0f);
			ImGui::TableHeadersRow();

			for (auto& SP : GD->SpFusionData)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(SP.ResultantPersona.c_str());
				ImGui::TableNextColumn();
				if (ImGui::BeginTable("SpecialCombos", 2, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_RowBg))
				{
					ImGui::TableNextRow();
					for (auto& P : SP.PersonaCombos)
					{
						ImGui::TableNextColumn();
						ImGui::TextUnformatted(GD->Personas[P].Name.c_str());
						ImGui::TableNextColumn();
						ImGui::TextUnformatted(GD->Personas[P].Arcana.c_str());
					}

					ImGui::EndTable();
				}
			}
		}
		else
		{
			ImGui::TableSetupColumn("Arcana 1");
			ImGui::TableSetupColumn("Arcana 2");
			ImGui::TableSetupColumn("Resultant Arcana");
			ImGui::TableHeadersRow();

			for (auto& Fusion : *SelectedFusion)
			{
				if (A1TextFilter.PassFilter(Fusion.Arcana1.c_str()) && A2TextFilter.PassFilter(Fusion.Arcana2.c_str()) && ResultantTextFilter.PassFilter(Fusion.ResultantArcana.c_str()))
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextUnformatted(Fusion.Arcana1.c_str());
					ImGui::TableNextColumn();
					ImGui::TextUnformatted(Fusion.Arcana2.c_str());
					ImGui::TableNextColumn();
					ImGui::TextUnformatted(Fusion.ResultantArcana.c_str());
				}
			}
		}

		ImGui::EndTable();
	}

	ImGui::PopStyleVar(2);
}

void ImFusionWindow::m_MoreInfoBody()
{
	if (!ImGui::BeginChild("InfoBody", ImGui::GetContentRegionAvail()))
	{
		ImGui::EndChild();
		return;
	}

	static ImGui::MarkdownConfig mdconfig;

	if (ImGui::CollapsingHeader("Normal Fusion##MInfo", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Framed))
	{
		static const std::string nfusion_info = 
			"Normal Fusion is the simplest fusion that requires only two personas to perform.\n"
			"To find the resultant persona, you must first find the resulting arcana and approximate resulting level.\n"
			"From the Normal Fusion table, find the resultant arcana based on the arcana of the two input personas.\n"
			"After that, find the calculate for the approximate resulting level with :\n"
			"    Approximate Level = [(P1's base level + P2's base level) / 2] + 1\n"
			"Based on the resultant arcana, find the persona which has an EQUAL or HIGHER base level than the approximate level, and that will be the resulting persona.\n\n"
			"Example : ( Based on Persona 4 Golden ) \n"
			"___\n"
			"  * Persona 1 : Matador [ Arcana:Death ] [ Base Level : 24 ]\n"
			"  * Persona 2 : Jack Frost [ Arcana:Magician ] [ Base Level : 16 ]\n"
			"  * Resultant Arcana : Emperor [ Magician + Death ]\n"
			"  * Approximate Level = [ (24 + 16) / 2 ] + 1 = 21\n"
			"  * From the Emperor Arcana, the persona which is equal or higher than the approximate level is King Frost. Therefore, the resulting persona is King Frost of Emperor Arcana.\n";

		ImGui::Markdown(nfusion_info.c_str(), nfusion_info.length(), mdconfig);
	}

	if (ImGui::CollapsingHeader("Triangle Fusion##MInfo", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Framed))
	{
		static const std::string tfusion_info = 
			"Triangle Fusion requires three personas to perform like what the fusion name suggests.\n"
			"Finding the resultant persona is almost the same as normal fusion, find the resulting arcana and approximate resulting level, but with some extra steps.\n"
			"  * First, find the \"Third Persona\".The Third Persona is the persona that has the highest CURRENT level of the three.\n"
			"  * Next is to find the resultant arcana of the two remaining persona with Normal Fusion.\n"
			"  * Finally, find the resultant persona from the Triangle Fusion table with the arcana of the Third Personaand the resulting arcana of the two other persona.\n"
			"After that, calculate for the approximate resulting level with :\n"
			"  Approx Level = [(P1's base level + P2's base level + P3's base level ) / 3 ] + 5\n"
			"Based on the resultant arcana, find the persona which has an EQUAL or HIGHER base level than the approximate level, and that will be the resulting persona.\n\n"
			"Example : ( Based on Persona 4 Golden )\n"
			"___\n"
			"  * Persona 1 : Matador [ Arcana:Death ] [ Current Level : 27 ]\n"
			"  * Persona 2 : Jack Frost [ Arcana:Magician] [ Current Level : 20 ]\n"
			"  * Persona 3 : Ara Mitama [ Arcana:Chariot] [ Current Level : 22 ]\n"
			"  * Resultant Arcana :\n"
			"    * Third Persona : Matador ( Death )\n"
			"    * Resultant Arcana of the other two = Magician + Chariot = Temperance\n"
			"    * Final Resultant Arcana = Death + Temperance = Jester\n"
			"  * Approximate Level = [(24 + 16 + 18) / 3] + 5 = 24\n"
			"  * From the Jester Arcana, the persona which is equal or higher than the approximate level is Take-Minakata. Therefore, the resulting persona is Take-Minakata of Jester Arcana.\n";

		ImGui::Markdown(tfusion_info.c_str(), tfusion_info.length(), mdconfig);
	}

	if (ImGui::CollapsingHeader("Special Fusion##MInfo", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Framed))
	{
		static const std::string spfusion_info =
			"Special Fusion is a fusion that would result to a Special Persona which only has one recipe/combination to make/fuse.\n"
			"Special Fusion involves Normal and Triangle Fusion, which are hidden in the game, whereas the Star(4) Fusion, Pentagonal(5) Fusion, and Hexagonal(6) Fusion combinations are already given in the game.\n";

		ImGui::Markdown(spfusion_info.c_str(), spfusion_info.length(), mdconfig);
	}

	if (ImGui::CollapsingHeader("Other Info##MInfo", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Framed))
	{
		static const std::string otherinfo =
			"Same Arcana Fusion\n"
			"____\n"
			"As the name suggest, it's a fusion of same arcanas. Only possible in Normal and Triangle Fusion.\n"
			"  * Normal Fusion:\n"
			"   - Also called as \"Fusion Down\". The process is similar to the normal way, only that the resulting persona should be EQUAL or LOWER than the approximate level\n"
			"  * Triangle Fusion:\n"
			"   - Also called as \"Fusion Up\". The process is the same as the normal way. So the resulting persona would always result in a higher level persona of the same arcana, "
			"the reason why it is also called as \"Fusion Up\"\n"
			"Same Current Level Fusion\n"
			"___\n"
			"This is only possible in triangle fusion where the three personas have the same current level. The process has only one difference to the normal way "
			"and that is how the third persona is chosen.\n"
			"The third persona will be the persona with the lowest arcana (starting from Fool).";

		ImGui::Markdown(otherinfo.c_str(), otherinfo.length(), mdconfig);
	}

	ImGui::EndChild();
}