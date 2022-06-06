#include "MainFunction.h"
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Fonts/font.h"

constexpr int width = 1160;
constexpr int height = 465;

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Persona Calculator/Recipe", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	gladLoadGL();

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int win_x, win_y;
	glfwGetMonitorWorkarea(monitor, NULL, NULL, &win_x, &win_y);

	GLFWimage images[1];
	images[0].pixels = stbi_load("Pcalc.png", &images[0].width, &images[0].height, 0, 4);
	glfwSetWindowIcon(window, 1, images);
	stbi_image_free(images[0].pixels);

	glViewport(0, 0, width, height);

	glfwSetWindowPos(window, ( (win_x - width) / 2 ) + 10, ((win_y - height) / 2) + 20);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io       = ImGui::GetIO(); (void)io;
	io.ConfigFlags   |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
	//io.ConfigFlags   |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
	//io.ConfigFlags   |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 clear_color(0.07f, 0.13f, 0.17f, 0.5f);
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.25f);
	}

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	//-------------------------------------------------------
	// SETUP FONTS
	//-------------------------------------------------------

	std::string dir = ((std::filesystem::current_path()).string()) + "\\Fonts\\";
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config;
	ImFontConfig CustomFont;
	CustomFont.FontDataOwnedByAtlas = false;

	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.OversampleH = 2;
	icons_config.OversampleV = 2;

	io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 14.0f, &CustomFont);
	io.Fonts->AddFontFromFileTTF((dir + FONT_ICON_FILE_NAME_FAS).c_str(), 14.0f, &icons_config, icons_ranges);
	io.Fonts->AddFontFromFileTTF((dir + FONT_ICON_FILE_NAME_FAR).c_str(), 14.0f, &icons_config, icons_ranges);
	io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 15.5f, &CustomFont);
	io.Fonts->AddFontFromFileTTF((dir + FONT_ICON_FILE_NAME_FAS).c_str(), 15.5f, &icons_config, icons_ranges);
	io.Fonts->AddFontFromFileTTF((dir + FONT_ICON_FILE_NAME_FAR).c_str(), 15.5f, &icons_config, icons_ranges);
	io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 13.0f, &CustomFont);
	io.Fonts->AddFontDefault();

	glfwSwapInterval(1); // 60fps usually

	ImMainFunc MainFunction;

	MainFunction.InitOnlyOnce();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//ImGui::ShowDemoWindow();

		MainFunction.MainWindow();
		if (MainFunction.CloseWindow())
		{
			glfwSetWindowShouldClose(window, 1);
		}

		// Renders the ImGUI elements
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize (window, &display_w, &display_h);
		glViewport             (0, 0, display_w, display_h);
		glClearColor           (clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear                (GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(window);
	}

	// Clear most dynamic memories and textures
	MainFunction.ClearMemory();

	// Deletes all ImGUI instances
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window); // Delete window before ending the program
	glfwTerminate(); // Terminate GLFW before ending the program
	return 0;
}

