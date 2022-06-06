# Persona Calculator

A calculator for persona games for finding certain persona result and combinations.

Useful when looking for fusion results to know what persona to acquire in-game instead of experimenting in-game, which could probably be time-consuming and tedious. This application is made for offline use so it can be portable.

## Persona Calculator Features

### Persona Calculator
Calculates the resulting persona based on the input personas.
Only for Normal and Triangle Fusion.

### Persona Recipe / Reverse Calculator
Creates a list of recipes of a persona.
Because there will be a lot of recipes, especially for Triangle Fusion recipes, you can navigate and filter the list of recipes to find the recipe you want or need.

### Persona Result Finder
Creates a list of results from the combination from the list of given personas. This is the same as the "Fusion Search" feature from Persona 4 Golden, but probably... better.

### Fusion Table
A simplified table of arcana fusion for easier time of finding the combination or results you want.
Also includes some info about fusion in general in Persona games.

### Skill Table (Experimental)
Only available in debug version.
Shows the list of skills of the chosen game.
This feature is still experimental and there are some bugs that I haven't fixed yet. Use at your own risk.
You need to provide your own image for elemental icons in the Images/Elemental Icons folder.
The file names should be the following:
  * sword.png - For slash and sword icons
  * phys.png - For strike and physical icons
  * gun.png - For pierce and gun icons
  * fire.png - For fire element icons
  * ice.png - For ice element icons
  * elec.png - For elec element icons
  * wind.png - For wind element icons
  * bless.png - For light and bless icons
  * curse.png - For dark and curse icons
  * almighty.png - For almighty element icons
  * ailment.png - For ailment element icons
  * healing.png - For healing element icons
  * support.png - For support element icons
  * passive.png - For passive skill icons
  * nuke.png - For nuke element icons
  * undefined.png - For undefined icons (needed for skill errors)
NOTE: Make sure the image size is small and not exceeding 100 x 100 pixels because the image textures uses a lot of memory especially when big and I still have not work around compressing image textures to reduce memory usage.

### Persona Data (Experimental)
Only available in debug version.
Show the data of a certain persona.
This feature is still experimental and incomplete. Use at your own risk.
You need to provide your own image for the skill card icon. The file name should be 'skillcard.png' and has small size.

## Other Important Things

### Supported Games
Supported games are:
  * Persona 3 FES
  * Persona 3 Portable
  * Persona 4
  * Persona 4 Golden
  * Persona 5
Addition support for older and newer persona games will be available in the future.

### Persona Files
The persona .dat files are just txt file with a fancy extension. Do not edit those (yet)! Well, you can edit it but the syntax is really messy. It can create some bugs in the application when you do! The plan is to make an editable data file, however, the process of filestreaming is still messy and a simple syntax mistake can make the program go haywire or the calculator create wrong results. Will fix in the future.

### Font Files
Always put the Font folder in the same directory as the application file.

### Images
Still not an official part of the application, but can be included when using the debug version of the application. Put the Images folder in the same directory as the application.

## Libraries Used
  * GLFW and OpenGl
  * Dear ImGui by Omar Cornut
  * stb_image
  * imgui_markdown by Juliette Foucaut & Doug Binks
  * imgui_combofilter by kovewnikov
  * Icon Font Awesome 5
  
## Future Plans
  * Support for older and newer persona games
  * Support for other Shin Megami Tensei games
  * Better algorithm for filestreaming
  * Fixing the Skill Data feature
  * Completion of Persona data feature
  * A feature for creating custom algorithm for something similar to persona fusion (Kinda like creating your own game feature that involves fusing something)
  
## Why did I make this?
  * I don't always have internet to search up the fusions or use the other persona calculators that requires internet
  * I like Persona and SMT games
