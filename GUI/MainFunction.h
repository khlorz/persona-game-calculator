#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>

#include "ImFunks.h"
#include "ReverseCalc.h"
#include "Fonts/IconsFontAwesome5.h"
#include "ImPersonaFinder.h"
#include "PersonaCalculator.h"
#include "PersonaRecipe.h"
#include "PersonaData.h"
#include "ImageTextures.h"
#include "ImFusionWindow.h"
#include "ImSkillWindow.h"

template<size_t T>
using CharArray = std::array<const char*, T>;

class ImMainFunc
{
public:
    ImMainFunc ( );
    ~ImMainFunc( );

    // Renders the Main Window
    void MainWindow(); 
    // Close the Main Window
    bool CloseWindow();
    // Clear the memory
    void ClearMemory();
    // Initializations that only need to be done once
    void InitOnlyOnce();
private:
    enum ErrorMessage_
    {
        ErrorMessage_NoSelectedTitle   = 0,
        ErrorMessage_NonExistentTitle  = 1 << 0,
        ErrorMessage_NoFusionData      = 1 << 1,
        ErrorMessage_MissingFusionData = 1 << 2
    };

    // Main Constants
    Gamedata          GD;                 // <* Main game data
    ImCalc            MainCalc;           // <* Main Persona Calculator Object
    ImRcp             MainRecipe;         // <* Main Persona Recipe Object
    ImPersonaData     MainPersonaData;    // <* Main Persona Data Object
    ImPersonaFinder   MainPersonaFinder;  // <* Main Persona Finder Object
    ImFusionWindow    MainFusionWindow;   // <* Main Fusion Window Object
    ImSkillWindow     MainSkillWindow;    // <* Main Skill Window Object

    int               SelectedGame;          // Selected Game from the main menu bar
    int               ErrorMessage;          //
    bool              CloseMainWindow;       // Set to true to close the whole window
    bool              Refresh;               // Checks if the game changed
    bool              InitSkill;             // Checks if the initialization of skills succeed
    bool              InitSuccess;           // Checks if the initialization succeeds
    bool              InitPersonaData;       // Checks if the initialization of Persona Data is successful
    bool              OpenHelpWindow;        // Flag for Help window
    bool              OpenArcanaFusionList;  // Flag for Arcana Fusion window
    bool              OpenSkillListWindow;   // Flag for Skill List window
    bool              OpenPersonaCalcWindow; // Flag for Persona Calculator window
    bool              OpenPersonaDataWindow; // Flag for Persona Data window
    CharArray<5>      GamesList;             // Array of game lists
    CharArray<5>      FusionDataPaths;       // Array of the paths of the _fusion.data files
    CharArray<5>      PersonaDataPaths;      // Array of paths of the _persona.data files
    CharArray<5>      PersonaSkillPaths;     // Array of paths of the _skill.data files
    ImgTextures       ElemIcons[16];

    void ResetWindows();
    void LoadElemIcons();                    // Load the texture of the element icons
    void ClearIconTextures();                // Clear the textures that was loaded
    void InitGameData();                     // Initialize the game data
    void MainMenuBar();                      // The Main Menu Bar containing various options
    void ResetAll();                         // Destroy current list
    void MiniWindows();
    void HelpWindow(bool *w_open);
    void MainBody();

    // Utility functions
    void Tooltip(const std::string& desc);
    void NoLoadedGameData();
};

