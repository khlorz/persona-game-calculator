#include "MainFunction.h"
#include "imgui_markdown.h"

ImMainFunc::ImMainFunc() :
	GamesList({
						"Persona 3 FES",
						"Persona 3 Portable",
						"Persona 4",
						"Persona 4 Golden",
						"Persona 5"
			 }),
	FusionDataPaths({
						"PersonaData/p3fes_fusion.dat",
						"PersonaData/p3p_fusion.dat",
						"PersonaData/p4_fusion.dat",
						"PersonaData/p4g_fusion.dat",
						"PersonaData/p5_fusion.dat"
				   }),
#if defined PCALC_DEBUG
	PersonaDataPaths({
						"PersonaData/p3fes_persona.dat",
						"PersonaData/p3p_persona.dat",
						"PersonaData/p4_persona.dat",
						"PersonaData/p4g_persona.dat",
						"PersonaData/p5_persona.dat"
					}),
	PersonaSkillPaths({
						"PersonaData/p3fes_skill.dat",
						"PersonaData/p3p_skill.dat",
						"PersonaData/p4_skill.dat",
						"PersonaData/p4g_skill.dat",
						"PersonaData/p5_skill.dat"
					 }),
#endif
	SelectedGame(-1),
	OpenHelpWindow(false),
	ErrorMessage(ErrorMessage_NoSelectedTitle)
{
	ResetWindows();
}

ImMainFunc::~ImMainFunc()
{
	ClearMemory();
}

void ImMainFunc::InitOnlyOnce()
{
	LoadElemIcons();
	MainPersonaData.SetSkillElementIcons(ElemIcons);
	MainPersonaData.LoadSkillCardTexture();
	MainSkillWindow.SetSkillElementIcons(ElemIcons);
}

void ImMainFunc::ResetWindows()
{
	InitSuccess           = false;
	CloseMainWindow       = false;
	OpenArcanaFusionList  = false;
	OpenPersonaCalcWindow = false;
	OpenPersonaDataWindow = false;
	OpenSkillListWindow   = false;
}

// Initialize the class
void ImMainFunc::InitGameData()
{
	// No refresh needed if there is no selected game
	if (SelectedGame == -1)
		return;

	// Destroy previous Gamedata
	ResetAll();
	if (!GD.InitializeGameData(1 << SelectedGame))
	{
		InitSuccess = false;
		ErrorMessage = ErrorMessage_NonExistentTitle;
		return;
	}

	//---------------------------------------------------
	// INITIALIZE FUSION DATA
	//---------------------------------------------------
	if (!GD.LoadFusionData(FusionDataPaths[SelectedGame]))
	{
		InitSuccess = false;
		ErrorMessage = ErrorMessage_NoFusionData;
		return;
	}
	else
	{
		// Initialize Persona Calculator
		if (!MainCalc.InitGameData(&GD) || !MainRecipe.InitGameData(&GD) || !MainPersonaFinder.InitGameData(&GD))
		{
			InitSuccess = false;
			ErrorMessage = ErrorMessage_MissingFusionData;
			return;
		}

		MainFusionWindow.InitializeWindow(&GD);
		InitSuccess  = true;
	}

#if defined PCALC_DEBUG
	InitSkill       = GD.LoadSkillData(PersonaSkillPaths[SelectedGame])  ? MainSkillWindow.InitGameData(&GD)    : false;
	InitPersonaData = GD.LoadPersonaData(PersonaDataPaths[SelectedGame]) ? MainPersonaData.InitPersonaData(&GD) : false;

#elif defined PCALC_RELEASE
	InitSkill             = false;
	InitPersonaData       = false;
	OpenPersonaDataWindow = false;
	OpenSkillListWindow   = false;

#endif // DEBUG
}

bool ImMainFunc::CloseWindow()
{
	return CloseMainWindow;
}

void ImMainFunc::LoadElemIcons()
{
	ElemIcons[PersonaElement_Slash].    LoadTextures("Images/Elemental Icons/sword.png");
	ElemIcons[PersonaElement_Strike].   LoadTextures("Images/Elemental Icons/phys.png");
	ElemIcons[PersonaElement_Pierce].   LoadTextures("Images/Elemental Icons/gun.png");
	ElemIcons[PersonaElement_Fire].     LoadTextures("Images/Elemental Icons/fire.png");
	ElemIcons[PersonaElement_Ice].      LoadTextures("Images/Elemental Icons/ice.png");
	ElemIcons[PersonaElement_Elec].     LoadTextures("Images/Elemental Icons/elec.png");
	ElemIcons[PersonaElement_Wind].     LoadTextures("Images/Elemental Icons/wind.png");
	ElemIcons[PersonaElement_Light].    LoadTextures("Images/Elemental Icons/bless.png");
	ElemIcons[PersonaElement_Dark].     LoadTextures("Images/Elemental Icons/curse.png");
	ElemIcons[PersonaElement_Almighty]. LoadTextures("Images/Elemental Icons/almighty.png");
	ElemIcons[PersonaElement_Ailment].  LoadTextures("Images/Elemental Icons/ailment.png");
	ElemIcons[PersonaElement_Healing].  LoadTextures("Images/Elemental Icons/healing.png");
	ElemIcons[PersonaElement_Support].  LoadTextures("Images/Elemental Icons/support.png");
	ElemIcons[PersonaElement_Passive].  LoadTextures("Images/Elemental Icons/passive.png");
	ElemIcons[PersonaElement_Nuke].     LoadTextures("Images/Elemental Icons/nuke.png");
	ElemIcons[PersonaElement_Undefined].LoadTextures("Images/Elemental Icons/undefined.png");
}

void ImMainFunc::ClearIconTextures()
{
	for (int i = 0; i < 16; i++)
		ElemIcons[i].ClearTextures();
}

void ImMainFunc::ClearMemory()
{
	ClearIconTextures();
	MainPersonaData.ClearTextures();
	MainSkillWindow.ClearTextures();
}

void ImMainFunc::ResetAll()
{
	ResetWindows();
	GD.DestroyAll();
	MainRecipe.DestroyList();
	MainPersonaFinder.DestroyList();
}

void ImMainFunc::MainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::BeginMenu("Game"))
			{
				for (int n = 0; n < GamesList.size(); n++)
				{
					if (ImGui::MenuItem(GamesList[n], NULL, SelectedGame == n))
					{
						if (SelectedGame != n)
						{
							Refresh      = true;
							SelectedGame = n;
							InitGameData();
						}
					}
				}
				ImGui::EndMenu();
			}
			ImGui::MenuItem("Exit", "Alt + F4", &CloseMainWindow);
			ImGui::EndMenu();
		}
		if (SelectedGame >= 0 && InitSuccess)
		{
			if (ImGui::BeginMenu(GamesList[SelectedGame]))
			{
				ImGui::MenuItem("Persona Calculator",  "Ctrl + T", &OpenPersonaCalcWindow);
				ImGui::MenuItem("Persona Data",        "Ctrl + D", &OpenPersonaDataWindow);
				ImGui::MenuItem("Arcana Fusions",      "Ctrl + F", &OpenArcanaFusionList );
				ImGui::MenuItem("Skill List",          "Ctrl + G", &OpenSkillListWindow  );

				ImGui::EndMenu();
			}
		}
		if(ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("View Help \xef\x81\x99", nullptr, &OpenHelpWindow);
			ImGui::EndMenu();
		}

#if defined  PCALC_DEBUG
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("Framerate: %.1f FPS", io.Framerate);
		ImGui::EndMainMenuBar();
#endif //  PCALC_DEBUG

	}
}

void ImMainFunc::MainWindow()
{
// Only for debug mode for now
#if defined PCALC_DEBUG
	if (ImGui::IsKeyPressed(294, false))
		Refresh = true;
	if (Refresh)
		InitGameData();
#endif

	MainMenuBar();

	if (OpenHelpWindow) HelpWindow(&OpenHelpWindow);

	if (!InitSuccess)
	{
		NoLoadedGameData();

		if (Refresh)
			Refresh = false;

		return;
	}

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_Always);
	if (!ImGui::Begin("Main Body [Persona Recipe/ Persona Result Finder]", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize ))
	{
		ImGui::End();
		return;
	}

	MiniWindows();
	MainBody();

	if (Refresh)
		Refresh = false;

	ImGui::End();
}

void ImMainFunc::MiniWindows()
{
	const ImGuiIO& io = ImGui::GetIO();

	if (io.KeyCtrl && ImGui::IsKeyPressed(84, false))   OpenPersonaCalcWindow = !OpenPersonaCalcWindow;  // Ctrl + T
	if (io.KeyCtrl && ImGui::IsKeyPressed(68, false))   OpenPersonaDataWindow = !OpenPersonaDataWindow;  // Ctrl + D
	if (io.KeyCtrl && ImGui::IsKeyPressed(70, false))   OpenArcanaFusionList  = !OpenArcanaFusionList;   // Ctrl + F
	if (io.KeyCtrl && ImGui::IsKeyPressed(71, false))   OpenSkillListWindow   = !OpenSkillListWindow;    // Ctrl + G

	if (OpenPersonaCalcWindow)   MainCalc.CalculatorBody(&OpenPersonaCalcWindow);
	if (OpenArcanaFusionList)    MainFusionWindow.MainWindow(&OpenArcanaFusionList);
	if (OpenSkillListWindow) 
	{
		if (InitSkill)
			MainSkillWindow.MainWindow(&OpenSkillListWindow);
		else
			MainSkillWindow.ErrorWindow(&OpenSkillListWindow);
	}
	if (OpenPersonaDataWindow)
	{
		if (InitPersonaData)
			MainPersonaData.MainWindow(&OpenPersonaDataWindow);
		else
			MainPersonaData.ErrorWindow(&OpenPersonaDataWindow);
	}
}

void ImMainFunc::MainBody()
{
	if (ImGui::BeginTabBar("##MainBodyTabs"))
	{
		if (ImGui::BeginTabItem("Persona Recipe / Reverse Fusion", NULL, Refresh ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None))
		{
			MainRecipe.MainBody();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Persona Result Finder"))
		{
			MainPersonaFinder.MainBody();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void ImMainFunc::HelpWindow(bool *w_open)
{
	const ImVec2 center = ImGui::GetWindowViewport()->GetWorkCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(500.0f, 350.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 10.0f);
	if (!ImGui::Begin(ICON_FA_QUESTION_CIRCLE" Help##Window", w_open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse ))
	{
		ImGui::End();
		ImGui::PopStyleVar();

		return;
	}

	static ImGui::MarkdownConfig mdconfig;

	static const std::string help1 =
		"Errors: \n"
		"___\n"
		"  * No Loaded Data\n"
		"  - Make sure you have the PersonaData folder containing the .dat files in the same directory as the application. "
		"If it is there, then make sure the .dat files exist with the proper file names.\n"
		"  The file names should be the following:\n"
		"    * Persona 3 FES\n"
		"    - p3fes_fusion.dat\n"
		"    - p3fes_skill.dat (for debug version)\n"
		"    - p3fes_persona.dat (for debug version)\n"
		"    * Persona 3 Portable\n"
		"    - p3p_fusion.dat\n"
		"    - p3p_skill.dat (for debug version)\n"
		"    - p3p_persona.dat (for debug version)\n"
		"    * Persona 4\n"
		"    - p4_fusion.dat\n"
		"    - p4_skill.dat (for debug version)\n"
		"    - p4_persona.dat (for debug version)\n"
		"    * Persona 4 Golden\n"
		"    - p4g_fusion.dat\n"
		"    - p4g_skill.dat (for debug version)\n"
		"    - p4g_persona.dat (for debug version)\n"
		"    * Persona 5\n"
		"    - p5_fusion.dat\n"
		"    - p5_skill.dat (for debug version)\n"
		"    - p5_persona.dat (for debug version)\n"
		"  * Characters with question mark\n"
		"  - Make sure the Font folder containing the .ttf file is in the same directory as the application\n"
		"Navigation: \n"
		"___\n"
		"  * Ctrl + Tab - Navigate through windows\n"
		"  * Space - To activate most widgets and buttons. Otherwise, use Enter\n"
		"  * Escape - To exit most widgets and pop-ups\n"
		"Widgets: \n"
		"___\n"
		"Text Filter: \n"
		"  * Use '-' ( dash ) to exclude more items to filter\n"
		"  * Use ',' ( comma ) to include more items to filter\n"
		"  * The filter is not case sensitive\n"
		"  * Items that passed the filter cannot be excluded by '-', and items excluded by '-' cannot pass the filter.";

	ImGui::Markdown(help1.c_str(), help1.length(), mdconfig);

	static ImGuiTextFilter filter;
	filter.DrawWithHint(ICON_FA_FILTER" Filter");
	static constexpr std::array<const char*, 5> Items = { 
														  "AAABBBCCC",
														  "DDDEEEFFF",
														  "GGGHHHIII",
														  "JJJKKKLLL",
														  "MMMNNNOOO"
														};
	if(!ImGui::BeginChild("##HelpTextFilter", ImVec2(ImGui::GetContentRegionAvail().x, 100.0f)))
	{
		ImGui::EndChild();
	}
	else
	{
		for (auto& I : Items)
			if (filter.PassFilter(I))
				ImGui::BulletText(I);

		ImGui::EndChild();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

void ImMainFunc::NoLoadedGameData()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_Always);
	if (!ImGui::Begin("No Loaded Game Data", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::End();
		return;
	}

	switch (ErrorMessage)
	{
	case ErrorMessage_NoSelectedTitle:
		ImGui::TextUnformatted("No selected title. Please select a game in Menu -> Game");
		break;

	case ErrorMessage_NonExistentTitle:
		ImGui::TextUnformatted("The current title does not exist in the application! Check for spelling errors!");
		break;

	case ErrorMessage_NoFusionData:
		ImGui::TextUnformatted("No fusion data is loaded! Check for the .dat file or filename errors!");
		break;

	case ErrorMessage_MissingFusionData:
		ImGui::TextUnformatted("Fusion data is loaded properly, but important fusion data is missing!");
		break;

	}

	ImGui::End();
}

void ImMainFunc::Tooltip(const std::string& desc)
{
	if(ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(250.0f);
		ImGui::TextUnformatted(desc.c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

