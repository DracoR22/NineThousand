#include "EditorUI.h"

namespace EditorUI {
	void Init() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);          // Background
		colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);            // Title Bar
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);      // Active Title
		colors[ImGuiCol_Button] = ImVec4(0.0f, 0.5f, 0.8f, 1.0f);             // Buttons
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.6f, 0.9f, 1.0f);      // Button Hover
		colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.4f, 0.7f, 1.0f);       // Button Click
		colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);         // Input Box Background
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);     // Input Hover
		colors[ImGuiCol_Border] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);             // Panel Borders

		ImGui_ImplGlfw_InitForOpenGL(Window::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	void Update() {
		Player& player = Game::GetPLayerByIndex(0);
		float gamma = OpenGLRenderer::GetGammaValue();
		RendererCommon::PostProcessMode postProcessMode = OpenGLRenderer::GetPostProcessMode();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Game Settings");
		ImGui::Text("FPS: %d", Window::GetFPSCount());
		ImGui::Text("Player Position: (%.2f, %.2f, %.2f)", player.getPosition().x, player.getPosition().y, player.getPosition().z);

		if (ImGui::SliderFloat("Gamma", &gamma, 0.1f, 5.0f, "%.2f")) {
			OpenGLRenderer::ChangeGammaValue(gamma);
		}

		if (ImGui::BeginCombo("Post Processing Mode", postProcessMode == RendererCommon::PostProcessMode::NONE ? "None" : "Sharpen")) {
			if (ImGui::Selectable("None", postProcessMode == RendererCommon::PostProcessMode::NONE)) {
				OpenGLRenderer::ChangePostProcessMode(RendererCommon::PostProcessMode::NONE);
			}
			if (ImGui::Selectable("Sharpen", postProcessMode == RendererCommon::PostProcessMode::SHARPEN)) {
				OpenGLRenderer::ChangePostProcessMode(RendererCommon::PostProcessMode::SHARPEN);
			}
			ImGui::EndCombo();
		}

		ImGui::End();
	}

	void Render() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void Cleanup() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}