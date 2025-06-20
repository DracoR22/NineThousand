#include "Model.h"
#include "AssetManager.h"

Model::Model(const std::string& name, const ModelCreateInfo& createInfo)
	: m_name(name) {}

void Model::draw(Shader& shader) {
	//glm::mat4 model = glm::mat4(1.0f);

	//model = glm::translate(model, pos);
	//model = glm::scale(model, size);
	//model *= rotation;

	//shader.setMat4("model", model);

	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].Draw(shader);
	}
}

void Model::DrawInstanced(Shader& shader, std::vector<glm::vec3> offsets) {
	/*glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, pos);
	model = glm::scale(model, size);
	model *= rotation;

	shader.setMat4("model", model);*/

	int fixedOffsets = std::min(UPPER_BOUND, (int)offsets.size());

	glBindBuffer(GL_ARRAY_BUFFER, m_instanceOffsetVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fixedOffsets * sizeof(glm::vec3), &offsets[0]);

	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader, offsets.size());
}

const std::string& Model::GetName() {
	return m_name;
}

void Model::SetAABBMin(glm::vec3 aabbMin) {
	m_aabbMin = aabbMin;
}

void Model::SetAABBMax(glm::vec3 aabbMax) {
	m_aabbMax = aabbMax;
}

void Model::LoadModel(ModelType type, ModelCreateInfo& createInfo) {

	if (createInfo.instanceOffsets.size() > UPPER_BOUND) {
		std::cerr << "Warning: instanceOffsets size exceeds UPPER_BOUND. Some instances may not be rendered." << std::endl;
	}

	if (type == ModelType::CUBE) {
		int noVertices = 36;

		float vertices[] = {
			// position                 normal              texcoord
			-0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,    1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,    0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,    0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,    0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,    0.0f, 1.0f
		};

		std::vector<unsigned int> indices(noVertices);
		for (unsigned int i = 0; i < noVertices; i++) {
			indices[i] = i;
		}

		Texture* baseColor = AssetManager::GetTextureByName(createInfo.baseTexture);
		Texture* normalMap = AssetManager::GetTextureByName(createInfo.normalTexture);
		Texture* rmaMap = AssetManager::GetTextureByName(createInfo.rmaTexture);

		baseColor->m_type = aiTextureType_DIFFUSE;
		normalMap->m_type = aiTextureType_NORMALS;
		rmaMap->m_type = aiTextureType_SPECULAR;

		std::vector<Vertex> vertexlist = Vertex::genList(vertices, noVertices);
		Vertex::CalcTanVectors(vertexlist, indices);

		Mesh cubeMesh("Cube_Primitive_Mesh", vertexlist, indices);
		cubeMesh.textures.push_back(*baseColor);    
		cubeMesh.textures.push_back(*rmaMap);  
		cubeMesh.textures.push_back(*normalMap);  

		if (createInfo.instanceOffsets.size() > 0) {
			CreateInstanceBuffers();

			cubeMesh.instanceOffsets = createInfo.instanceOffsets;
			cubeMesh.SetupInstance();
		}

		meshes.push_back(cubeMesh);
	}
	else if (type == ModelType::PLANE) {
		int noVertices = 6;

		float vertices[] = {
			// position               normal           texcoord
			-0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
			 0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
			 0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 1.0f,

			-0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
			 0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
			-0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,    0.0f, 1.0f
		};

		std::vector<unsigned int> indices(noVertices);
		for (unsigned int i = 0; i < noVertices; i++) {
			indices[i] = i;
		}

		Texture* baseColor = AssetManager::GetTextureByName(createInfo.baseTexture);
		Texture* normalMap = AssetManager::GetTextureByName(createInfo.normalTexture);
		Texture* rmaMap = AssetManager::GetTextureByName(createInfo.rmaTexture);

		baseColor->m_type = aiTextureType_DIFFUSE;
		normalMap->m_type = aiTextureType_NORMALS;
		rmaMap->m_type = aiTextureType_SPECULAR;

		std::vector<Vertex> vertexlist = Vertex::genList(vertices, noVertices);
		Vertex::CalcTanVectors(vertexlist, indices);

		Mesh planeMesh("Plane_Primitive_Mesh", vertexlist, indices);
		planeMesh.textures.push_back(*baseColor);
		planeMesh.textures.push_back(*rmaMap);
		planeMesh.textures.push_back(*normalMap);

		meshes.push_back(planeMesh);
	}
}

void Model::loadAssimpModel(std::string path) {
	Assimp::Importer import;

	const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;

		return;
	}
	/* directory = path.substr(0, path.find_last_of('/'));*/
	m_directory = "resources/textures";

	processNode(scene->mRootNode, scene);
}

void Model::CreateInstanceBuffers() {
	glGenBuffers(1, &m_instanceOffsetVBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_instanceOffsetVBO);
	glBufferData(GL_ARRAY_BUFFER, UPPER_BOUND * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		std::string meshName = mesh->mName.C_Str();

		// skip certain meshes for now
		if (meshName == "ArmsFemale") {
			continue;
		}

		if (meshName.find("AKS74U_Scope") != std::string::npos || meshName == "AKS74U_Lens" || meshName == "AKS74U_RedDot") {
			continue;
		}

		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	/*std::string meshName = mesh->mName.C_Str();
	std::cout << "Mesh " << " is named: " << meshName << std::endl;*/
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		// position
		vertex.m_Position = glm::vec3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);

		// normals
		if (mesh->HasNormals()) {
			vertex.m_Normal = glm::vec3(
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z
			);
		}

		// texture coords
		if (mesh->mTextureCoords[0]) {
			vertex.m_TexCoords = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);
		}
		else {
			vertex.m_TexCoords = glm::vec2(0.0f);
		}

		// tangent vector
		if (mesh->HasTangentsAndBitangents()) {
			vertex.m_Tangent = {
			mesh->mTangents[i].x,
			mesh->mTangents[i].y,
			mesh->mTangents[i].z
			};
		}

		vertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	// process material
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		
			// diffuse maps
			std::vector<Texture> diffuseMaps = LoadDefaultMaterials(material, aiTextureType_DIFFUSE);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			// RMA maps
			LoadRMAMaterials(mesh->mName.C_Str(), material, textures);

			// normal maps (.obj files use aiTextureType_HEIGHT) 
			std::vector<Texture> normalMaps = LoadDefaultMaterials(material, aiTextureType_NORMALS);
			if (normalMaps.empty()) {
				normalMaps = LoadDefaultMaterials(material, aiTextureType_HEIGHT);
			}
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	ExtractBoneWeightForVertices(vertices, mesh, scene);

	return Mesh(mesh->mName.C_Str(), vertices, indices, textures);
}

std::vector<Texture> Model::LoadDefaultMaterials(aiMaterial* mat, aiTextureType type) {
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::string fullPath = str.C_Str();
		std::string fileName = fullPath.substr(fullPath.find_last_of("/\\") + 1);

		// prevent duplicate loading
		bool skip = false;
		for (const auto& loadedTex : m_textures_loaded) {
			if (loadedTex.m_path == fileName) {
				textures.push_back(loadedTex);
				skip = true;
				break;
			}
		}

		if (!skip) {
			Texture* texture = AssetManager::GetTextureByName(fileName);
			if (texture) {
				textures.push_back(*texture);
				m_textures_loaded.push_back(*texture);
			}
		}
	}
	return textures;
}

void Model::LoadRMAMaterials(const std::string& meshName, aiMaterial* material, std::vector<Texture>& textures) {
	static const std::unordered_map<std::string, std::string> RMAOverrides = {
		{"Glock",     "Glock_RMA.png"},
		{"ArmsMale",  "Hands_RMA.png"},
		{"AKS74UBarrel",  "AKS74U_4_RMA.png"},
		{"AKS74UBolt",  "AKS74U_1_RMA.png"},
		{"AKS74UHandGuard",  "AKS74U_0_RMA.png"},
		{"AKS74UMag",  "AKS74U_3_RMA.png"},
		{"AKS74UPistolGrip",  "AKS74U_2_RMA.png"},
		{"AKS74UReceiver",  "AKS74U_1_RMA.png"},
		{"AKS74U_ScopeSupport",  "AKS74U_ScopeSupport_RMA.png"},
		{"AKS74U_ScopeMain",  "AKS74U_ScopeMain_RMA.png"},
		{"AKS74U_ScopeFrontCap",  "AKS74U_ScopeVxor_RMA.png"},
		{"AKS74U_ScopeBackCap",  "AKS74U_ScopeVxorr_RMA.png"},
		{"Magazine_low",  "P90_Mag_RMA.png"},
		{"Magazine_low2",  "P90_Mag_RMA.png"},
		{"LowReceiver_low",  "P90_Main_RMA.png"},
		{"SideRail_low",  "P90_Rails_RMA.png"},
		{"TopRailStandard_low",  "P90_Rails_RMA.png"},
		{"UpperReceiver_low",  "P90_FrontEnd_RMA.png"},
		{"SideRailScrews_low",  "P90_Rails_RMA.png"},
		{"Compensator_low",  "P90_FrontEnd_RMA.png"},
		{"ChargingHandle_low",  "P90_FrontEnd_RMA.png"},
		{"ChargingHandlePlate_low",  "P90_FrontEnd_RMA.png"},
		{"Katana", "Katana_RMA.png"},
		{"PoolLadder3", "PoolLadder_RMA.png"}
	};

	auto it = RMAOverrides.find(meshName);
	if (it != RMAOverrides.end()) {
		/*Texture tex(m_directory, it->second, aiTextureType_SPECULAR);*/
		Texture* texture = AssetManager::GetTextureByName(it->second);
		/*tex.load(false);*/
		textures.push_back(*texture);
		m_textures_loaded.push_back(*texture);
	}
	else {
		auto specularMaps = LoadDefaultMaterials(material, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
}


void Model::cleanup() {
	for (Mesh mesh : meshes) {
		mesh.Cleanup();
	}
}

// animations
std::map<std::string, BoneInfo>& Model::GetBoneInfoMap() {
	return m_BoneInfoMap;
}

int& Model::GetBoneCount() {
	return m_BoneCounter;
}

void Model::SetVertexBoneDataToDefault(Vertex& vertex) {
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
		vertex.m_BoneIDs[i] = -1;
		vertex.m_Weights[i] = 0.0f;
	}
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		if (vertex.m_BoneIDs[i] < 0)
		{
			vertex.m_Weights[i] = weight;
			vertex.m_BoneIDs[i] = boneID;
			break;
		}
	}
}

void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	auto& boneInfoMap = m_BoneInfoMap;
	int& boneCount = m_BoneCounter;

	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = boneCount;
			newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
			boneInfoMap[boneName] = newBoneInfo;
			boneID = boneCount;
			boneCount++;
		}
		else
		{
			boneID = boneInfoMap[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}