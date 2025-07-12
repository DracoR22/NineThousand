#include "AssetManager.h"

namespace AssetManager {
	std::unordered_map<std::string, int> g_modelIndexMap;
	std::unordered_map<std::string, int> g_animationIndexMap;
	std::unordered_map<std::string, int> g_animatorIndexMap;
	std::unordered_map<std::string, int> g_textureIndexMap;
	std::unordered_map<std::string, int> g_materialIndexMap;
	std::unordered_map<std::string, int> g_meshIndexMap;

	// Models
	void LoadModel(const std::string& name, ModelType type, ModelCreateInfo& createInfo) {
		Model model(name, createInfo);
		model.LoadModel(type, createInfo);
		ComputeModelAABB(model);

		g_models.push_back(model);
		g_modelIndexMap[name] = g_models.size() - 1;

		// store meshes
		for (Mesh& mesh : model.meshes) {
			g_meshes.push_back(mesh);
			g_meshIndexMap[mesh.m_Name] = g_meshes.size() - 1;
		}
	}

	void LoadAssimpModel(const std::string& name, const std::string& path, ModelCreateInfo& createInfo) {
		Model model(name, createInfo);
		model.loadAssimpModel(path);
		ComputeModelAABB(model);

		g_models.push_back(model);
	    g_modelIndexMap[name] = g_models.size() - 1;

		// store meshes
		for (Mesh& mesh : model.meshes) {
			g_meshes.push_back(mesh);
			g_meshIndexMap[mesh.m_Name] = g_meshes.size() - 1;
		}
	}

	void DrawModel(const std::string& name, Shader& shader) {
		Model* existingModel = AssetManager::GetModelByName(name);

		if (!existingModel) {
			std::cout << "AssetManager::DrawModel() failed because '" << name << "' does not exist!\n";
			return;
		}

		existingModel->draw(shader);
	}

	void DrawModelInstanced(const std::string& name, Shader& shader, std::vector<glm::vec3> offsets) {
		Model* existingModel = AssetManager::GetModelByName(name);

		if (!existingModel) {
			std::cout << "AssetManager::DrawModel() failed because '" << name << "' does not exist!\n";
			return;
		}

		existingModel->DrawInstanced(shader, offsets);
	}

	Model* GetModelByName(const std::string& name) {
		auto it = g_modelIndexMap.find(name);
		if (it != g_modelIndexMap.end()) {
			int index = it->second;
			return &g_models[index];
		}

		std::cout << "AssetManager::GetModelByName() failed because '" << name << "' does not exist!\n";
		return nullptr;
	}

	void ComputeModelAABB(Model& model) {
		glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

		for (auto&& mesh : model.meshes) {
			for (auto&& vertex : mesh.vertices) {
				minAABB.x = std::min(minAABB.x, vertex.m_Position.x);
				minAABB.y = std::min(minAABB.y, vertex.m_Position.y);
				minAABB.z = std::min(minAABB.z, vertex.m_Position.z);

				maxAABB.x = std::max(maxAABB.x, vertex.m_Position.x);
				maxAABB.y = std::max(maxAABB.y, vertex.m_Position.y);
				maxAABB.z = std::max(maxAABB.z, vertex.m_Position.z);
			}
		}

		model.SetAABBMin(minAABB);
		model.SetAABBMax(maxAABB);
	}

	std::vector<Model>& GetModels() {
		return g_models;
	}

	void CleanupModels() {
		for (auto& model : g_models) {
			model.cleanup();
		}
		g_models.clear();
		g_modelIndexMap.clear();
		g_textures.clear();
	}

	// Meshes
	int GetMeshIndexByName(const std::string& name) {
		auto it = g_meshIndexMap.find(name);
		if (it != g_meshIndexMap.end()) {
			return it->second;
		}

	    std::cout << "AssetManager::GetMeshIndexByName() failed because '" << name << "' does not exist!\n";
		return -1;
	}

	Mesh* GetMeshByIndex(int index) {
		if (index >= 0 && index < g_meshes.size()) {
			return &g_meshes[index];
		}

		std::cout << "AssetManager::GetMeshByIndex() failed because index was -1\n";
		return nullptr;
	}

	// Textures
	TextureData DecodeTexture(const std::string& dir, const std::string& name, aiTextureType type) {
		stbi_set_flip_vertically_on_load(false);
		TextureData texData;
		texData.name = name;
		texData.type = type;

		std::string fullPath = dir + "/" + name;
		unsigned char* data = stbi_load(fullPath.c_str(), &texData.width, &texData.height, &texData.channels, 0);

		if (!data) {
			std::cout << "AssetManager::DecodeTexture() failed to decode: " << fullPath << std::endl;
			return texData;
		}
		bool isNormal = (type == aiTextureType_NORMALS || type == aiTextureType_HEIGHT);

		switch (texData.channels) {
		case 1:
			texData.internalFormat = GL_RED;
			texData.format = GL_RED;
			break;
		case 3:
			texData.internalFormat = isNormal ? GL_RGB : GL_SRGB;
			texData.format = GL_RGB;
			break;
		case 4:
			texData.internalFormat = isNormal ? GL_RGBA : GL_SRGB_ALPHA;
			texData.format = GL_RGBA;
			break;
		}

		size_t size = texData.width * texData.height * texData.channels;
		texData.pixels.assign(data, data + size);
		stbi_image_free(data);
		return texData;
	}

	void LoadTexture(const std::string& name, aiTextureType type) {
		if (g_textureIndexMap.find(name) != g_textureIndexMap.end())
			return;

		TextureData data = DecodeTexture("resources/fonts", name, type);
		Texture texture("resources/fonts", name, type);
		texture.m_format = data.format;
		texture.m_internalFormat = data.internalFormat;
		texture.m_numChannels = data.channels;
		texture.m_width = data.width;
		texture.m_height = data.height;

		texture.AllocateMemory(data);
		
		g_textures.push_back(texture);
		g_textureIndexMap[name] = g_textures.size() - 1;
	}

	void LoadAllTexturesAsync() {
		auto start = std::chrono::high_resolution_clock::now();
		std::vector<std::future<TextureData>> futures;

		for (const auto& entry : std::filesystem::directory_iterator("resources/textures")) {
			std::string filename = entry.path().filename().string();
			aiTextureType type = GetFileTextureType(filename);
			futures.push_back(std::async(std::launch::async, DecodeTexture, "resources/textures", filename, type));
		}

		for (auto& f : futures) {
			TextureData data = f.get();
			Texture texture("resources/textures", data.name, data.type);
			texture.m_format = data.format;
			texture.m_internalFormat = data.internalFormat;
			texture.m_numChannels = data.channels;
			texture.m_width = data.width;
			texture.m_height = data.height;

			texture.AllocateMemory(data);

			std::cout << "loaded texture: " << data.name << std::endl;
			g_textures.push_back(texture);
			g_textureIndexMap[data.name] = g_textures.size() - 1;
		}

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = end - start;
		std::cout << "Loaded " << g_textures.size() << " textures in " << duration.count() << " seconds.\n";
	}

	aiTextureType GetFileTextureType(const std::string& filename) {
		if (filename.find("_ALB") != std::string::npos) {
			return aiTextureType_DIFFUSE;
		}
		else if (filename.find("_NRM") != std::string::npos) {
			return aiTextureType_NORMALS;
		}
		else if (filename.find("_RMA") != std::string::npos) {
			return aiTextureType_SPECULAR; 
		}
		return aiTextureType_DIFFUSE; 
	}

	Texture* GetTextureByName(const std::string& name) {
		auto it = g_textureIndexMap.find(name);
		if (it != g_textureIndexMap.end()) {
			int index = it->second;
			return &g_textures[index];
		}

		std::cout << "AssetManager::GetTextureByName() failed because '" << name << "' does not exist!\n";
		return nullptr;
	}

	Texture* GetTextureByIndex(int index) {
		if (index != -1) {
			return &g_textures[index];
		}

		std::cout << "AssetManager::GetTextureByIndex() failed because index was -1\n";
		return nullptr;
	}
	
	int GetTextureIndexByName(const std::string& name, bool ignoreWarning) {
		auto it = g_textureIndexMap.find(name);
		if (it != g_textureIndexMap.end()) {
			return it->second;
		}

		if (!ignoreWarning) {
			std::cout << "AssetManager::GetTextureIndexByName() failed because '" << name << "' does not exist!\n";
		}
		return -1;
	}

// Materials
	void BuildMaterials() {
		Material& defaultMaterial = g_materials.emplace_back();
		defaultMaterial.name = "Default";
		defaultMaterial.baseTexture = GetTextureIndexByName("Default_ALB.png");
		defaultMaterial.normalTexture = GetTextureIndexByName("Default_NRM.png");
		defaultMaterial.rmaTexture = GetTextureIndexByName("Default_RMA.png");
		g_materialIndexMap[defaultMaterial.name] = 0;

		for (Texture& texture : g_textures) {
			if (texture.m_file.find("_ALB") != std::string::npos) {
				Material& material = g_materials.emplace_back();

				material.name = GetMaterialNameFromTextureFile(texture.m_file);
				int baseIndex = GetTextureIndexByName(material.name + "_ALB.png", true);
				int normalIndex = GetTextureIndexByName(material.name + "_NRM.png", true);
				int rmaIndex = GetTextureIndexByName(material.name + "_RMA.png", true);
				material.baseTexture = baseIndex;
				material.normalTexture = (normalIndex != -1) ? normalIndex : GetTextureIndexByName("Default_NRM.png");
				material.rmaTexture = (rmaIndex != -1) ? rmaIndex : GetTextureIndexByName("Default_RMA.png");

				g_materialIndexMap[material.name] = g_materials.size() - 1;
			}
		}
	}

	std::string GetMaterialNameFromTextureFile(const std::string& filePath) {
		std::filesystem::path path(filePath);
		std::string filenameWithoutExtension = path.stem().string();

		const std::string suffix = "_ALB";
		if (filenameWithoutExtension.size() > suffix.size() &&
			filenameWithoutExtension.compare(filenameWithoutExtension.size() - suffix.size(), suffix.size(), suffix) == 0) {
			filenameWithoutExtension.erase(filenameWithoutExtension.size() - suffix.size(), suffix.size());
		}

		return filenameWithoutExtension;
	}

	Material* GetDefaultMaterial() {
		if (!g_materials.empty()) {
			return &g_materials[0];
		}

		std::cerr << "AssetManager:GetDefaultMaterial() error: g_materials is empty" << std::endl;
		return nullptr;
	}

	Material* GetMaterialByName(const std::string& name) {
		auto it = g_materialIndexMap.find(name);
		if (it != g_materialIndexMap.end()) {
			int index = it->second;
			return &g_materials[index];
		}

		std::cout << "AssetManager::GetMaterialByName() failed because '" << name << "' does not exist!\n";
		return nullptr;
	}

	Material* GetMaterialByIndex(int index) {
		if (index >= 0 && index < g_materials.size()) {
			return &g_materials[index];
		}

		std::cout << "AssetManager::GetMaterialByIndex() failed because index was -1\n";
		return GetDefaultMaterial();
	}

	std::vector<Material>& GetAllMaterials() {
		return g_materials;
	}

	int GetMaterialIndexByName(const std::string& name) {
		auto it = g_materialIndexMap.find(name);
		if (it != g_materialIndexMap.end()) {
			return it->second;
		}

		std::cout << "AssetManager::GetMaterialIndexByName() failed because '" << name << "' does not exist!\n";
		return -1;
	}
		 
// Animations
	void LoadAnimation(const std::string& name, const std::string& path, Model* model) {
		static bool reserved = false;
		if (!reserved) {
			g_animations.reserve(20);  
			reserved = true;
		}

		Animation animation(path, model);
		g_animations.push_back(animation);
		g_animationIndexMap[name] = g_animations.size() - 1;
	}

	Animation* GetAnimationByName(const std::string& name) {
		auto it = g_animationIndexMap.find(name);
		if (it != g_animationIndexMap.end()) {
			int index = it->second;
			return &g_animations[index];
		}

		std::cout << "AssetManager::GetAnimationByName() failed because '" << name << "' does not exist!\n";
		return nullptr;
	}

	void LoadAnimator(const std::string& name, Animation* animation) {
		static bool reserved = false;
		if (!reserved) {
			g_animators.reserve(20);
			reserved = true;
		}

		Animator animator(animation);
		g_animators.push_back(animator);
		g_animatorIndexMap[name] = g_animators.size() - 1;
	}

	Animator* GetAnimatorByName(const std::string& name) {
		auto it = g_animatorIndexMap.find(name);
		if (it != g_animatorIndexMap.end()) {
			int index = it->second;
			return &g_animators[index];
		}

		std::cout << "AssetManager::GetAnimatorByName() failed because '" << name << "' does not exist!\n";
		return nullptr;
	}
}