#include "EditorPanel.h"

namespace EditorPanel {
	bool g_showCreateButtonPanel = false;
	std::string g_selectedObjectName = "None";

	void Init() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		colors[ImGuiCol_Text] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);

		ImGui_ImplGlfw_InitForOpenGL(Window::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	void Update() {
		Player& player = Game::GetPLayerByIndex(0);

		float gamma = OpenGLRenderer::GetGammaValue();
		float exposure = OpenGLRenderer::GetExposure();

		RendererCommon::PostProcessMode postProcessMode = OpenGLRenderer::GetPostProcessMode();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (Game::GetGameState() == Game::GameState::EDITOR) {
			ImGui::Begin("Game Editor");
			ImGui::Text("FPS: %d", Window::GetFPSCount());
			ImGui::Text("Player Position: (%.2f, %.2f, %.2f)", player.getPosition().x, player.getPosition().y, player.getPosition().z);

			if (ImGui::CollapsingHeader("Post Process")) {
				if (ImGui::SliderFloat("Gamma", &gamma, 0.1f, 5.0f, "%.2f")) {
					OpenGLRenderer::ChangeGammaValue(gamma);
				}

				if (ImGui::SliderFloat("Exposure", &exposure, 0.01f, 10.0f, "%.3f")) {
					OpenGLRenderer::SetExposureValue(exposure);
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

			}

			if (ImGui::CollapsingHeader("Lights")) {
				std::vector<LightCreateInfo>& lights = OpenGLRenderer::GetSceneLights();
				for (int i = 0; i < lights.size(); ++i) {
					ImGui::PushID(i);

					ImGui::Text("Light %d", i + 1);
					if (ImGui::SliderFloat("Strength", &lights[i].strength, 0.0f, 100.0f)) {
						OpenGLRenderer::UpdateLightStrength(i, lights[i].strength);
					}

					if (ImGui::SliderFloat("Radius", &lights[i].radius, 0.0f, 100.0f)) {
						OpenGLRenderer::UpdateLightRadius(i, lights[i].radius);
					}

					if (ImGui::InputFloat("Position X", &lights[i].position.x, 1.0f, 10.0f, "%.2f")) {
						OpenGLRenderer::UpdateLightPosition(i, lights[i].position);
					}

					if (ImGui::InputFloat("Position Y", &lights[i].position.y, 1.0f, 10.0f, "%.2f")) {
						OpenGLRenderer::UpdateLightPosition(i, lights[i].position);
					}

					if (ImGui::InputFloat("Position Z", &lights[i].position.z, 1.0f, 10.0f, "%.2f")) {
						OpenGLRenderer::UpdateLightPosition(i, lights[i].position);
					}

					ImGui::Separator();
					ImGui::PopID();
				}
			}

			if (ImGui::CollapsingHeader("Objects")) {
				int index = 1000;

				std::vector<GameObject>& objects = Scene::GetGameObjects();

				for (GameObject& gameObject : objects) {
					if (gameObject.IsSelected()) {
						g_selectedObjectName = gameObject.GetName();
						break;
					}
				}

				if (ImGui::BeginCombo("Select Object", g_selectedObjectName.c_str())) {
					if (ImGui::Selectable("None", g_selectedObjectName == "None")) {
						for (GameObject& obj : objects)
							obj.SetSelected(false); // Deselect all

						g_selectedObjectName = "None";
					}
					for (GameObject& gameObject : objects) {
						bool isSelected = (g_selectedObjectName == gameObject.GetName()); // get only the first one selected

						if (ImGui::Selectable(gameObject.GetName().c_str(), isSelected)) {
							for (GameObject& obj : objects)
								obj.SetSelected(false); // Deselect all

							gameObject.SetSelected(true);
							g_selectedObjectName = gameObject.GetName();
						}

						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}	
					}

					ImGui::EndCombo();
				}

				for (GameObject& gameObject : Scene::GetGameObjects()) {
					if (!gameObject.IsSelected())
						continue; 

					ImGui::PushID(index++);

					ImGui::Text("%s", gameObject.GetName().c_str());

					glm::vec3 objectSize = gameObject.GetSize();
					if (ImGui::SliderFloat("Size XYZ", &objectSize.x, 0.0f, 100.0f)) {
						gameObject.SetSize(glm::vec3(objectSize.x, objectSize.x, objectSize.x));
					}
					
					if (ImGui::SliderFloat("Size X", &objectSize.x, 0.0f, 100.0f)) {
						gameObject.SetSize(glm::vec3(objectSize.x, objectSize.y, objectSize.z));
					}

				    if (ImGui::SliderFloat("Size Y", &objectSize.y, 0.0f, 100.0f)) {
							gameObject.SetSize(glm::vec3(objectSize.x, objectSize.y, objectSize.z));
					}

					if (ImGui::SliderFloat("Size Z", &objectSize.z, 0.0f, 100.0f)) {
							gameObject.SetSize(glm::vec3(objectSize.x, objectSize.y, objectSize.z));
					}
				

					glm::vec3 objectPosition = gameObject.GetPosition();
					if (ImGui::InputFloat("Position X", &objectPosition.x, 1.0f, 10.0f, "%.2f")) {
						gameObject.SetPosition(objectPosition);
					}

					if (ImGui::InputFloat("Position Y", &objectPosition.y, 1.0f, 10.0f, "%.2f")) {
						gameObject.SetPosition(objectPosition);
					}

					if (ImGui::InputFloat("Position Z", &objectPosition.z, 1.0f, 10.0f, "%.2f")) {
						gameObject.SetPosition(objectPosition);
					}

					glm::vec3 objectEulerRotation = gameObject.GetRotationEuler();
					if (ImGui::SliderFloat3("Rotation", &objectEulerRotation.x, -180.0f, 180.0f, "%.2f")) {
						
						gameObject.SetRotationEuler(objectEulerRotation);
					}

					float textureScale = gameObject.GetTextureScale();
					if (ImGui::SliderFloat("Texture Scale", &textureScale, 0.1f, 10.0f, "%.2f")) {
						gameObject.SetTextureScale(textureScale);
					}


					ImGui::Separator();
					ImGui::PopID();
				}

				if (ImGui::Button("Create Plane")) {
					GameObjectCreateInfo newPlaneObject;
					newPlaneObject.modelName = "Plane";
					newPlaneObject.name = "Plane" + std::to_string(Scene::GetGameObjects().size());

					Scene::AddGameObject(newPlaneObject);
				}
				if (ImGui::Button("Create Cube")) {
					GameObjectCreateInfo newPlaneObject;
					newPlaneObject.modelName = "Cube";
					newPlaneObject.name = "Cube" + std::to_string(Scene::GetGameObjects().size());

					Scene::AddGameObject(newPlaneObject);
				}
			}

			ImGui::End();
		}

		if (Keyboard::KeyJustPressed(GLFW_KEY_F2)) {
			g_showCreateButtonPanel = !g_showCreateButtonPanel;
		}

		if (g_showCreateButtonPanel) {
			ImVec2 fixedSize(300, 200);
			ImGui::SetNextWindowSize(fixedSize, ImGuiCond_Always);

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;

			ImGui::Begin("Save Current Level", &g_showCreateButtonPanel, windowFlags);

			if (ImGui::Button("Save")) {
				std::vector<GameObject> gameObjects = Scene::GetGameObjects();
				LevelCreateInfo levelCreateInfo;
				levelCreateInfo.name = "test";
				for (GameObject& object : gameObjects) {
					levelCreateInfo.gameObjects.emplace_back(object.GetLatestCreateInfo());
				}
				
				
				JSON::SaveLevel("resources/levels/" + levelCreateInfo.name + ".json", levelCreateInfo);
				JSON::SaveLevel("../../../resources/levels/" + levelCreateInfo.name + ".json", levelCreateInfo);
			}

			ImGui::End();
		}
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