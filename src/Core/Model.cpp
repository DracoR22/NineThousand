#include "Model.h"

Model::Model(const std::string& name, const ModelCreateInfo& createInfo)
	: m_name(name), pos(createInfo.position), size(createInfo.size), rotation(createInfo.rotation) {}

void Model::draw(Shader& shader) {
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, pos);
	model = glm::scale(model, size);
	model *= rotation;

	/*model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));*/

	shader.setMat4("model", model);

	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].draw(shader);
}

void Model::DrawInstanced(Shader& shader, std::vector<glm::vec3> offsets) {
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, pos);
	model = glm::scale(model, size);
	model *= rotation;

	shader.setMat4("model", model);

	int fixedOffsets = std::min(UPPER_BOUND, (int)offsets.size());

	glBindBuffer(GL_ARRAY_BUFFER, instanceOffsetVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fixedOffsets * sizeof(glm::vec3), &offsets[0]);

	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].draw(shader, offsets.size());
}

void Model::setPosition(const glm::vec3& newPos) {
	pos = newPos;
}

void Model::setRotation(const glm::mat4& newRotation) {
	rotation = newRotation;
}

void Model::setSize(const glm::vec3& newSize) {
	size = newSize;
}

const std::string& Model::GetName() {
	return m_name;
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

		Texture diffuse("resources/textures", "brickwall_diffuse.jpg", aiTextureType_DIFFUSE);
		diffuse.load(false);

		Texture specular("resources/textures", "brickwall_specular.jpg", aiTextureType_SPECULAR);
		specular.load(false);

		Texture normal("resources/textures", "brickwall_normal.jpg", aiTextureType_NORMALS);
		normal.load(false);


		if (diffuse.id) {
			std::cout << "Cube diffuse texture loaded: " << diffuse.path << std::endl;
		}
		if (specular.id) {
			std::cout << "Cube specular texture loaded: " << specular.path << std::endl;
		}

		if (normal.id) {
			std::cout << "Cube normal texture loaded: " << normal.path << std::endl;
		}

		std::vector<Vertex> vertexlist = Vertex::genList(vertices, noVertices);
		Vertex::CalcTanVectors(vertexlist, indices);

		Mesh cubeMesh(vertexlist, indices);
		cubeMesh.textures.push_back(diffuse);    
		cubeMesh.textures.push_back(specular);  
		cubeMesh.textures.push_back(normal);  

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
			// First triangle
			-0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
			 0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
			 0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
			 // Second triangle
			 -0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
			  0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
			 -0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,    0.0f, 1.0f
		};

		std::vector<unsigned int> indices(noVertices);
		for (unsigned int i = 0; i < noVertices; i++) {
			indices[i] = i;
		}

		Texture diffuse("resources/textures", "brickwall_diffuse.jpg", aiTextureType_DIFFUSE);
		diffuse.load(false);

		Texture specular("resources/textures", "brickwall_specular.jpg", aiTextureType_SPECULAR);
		specular.load(false);

		Texture normal("resources/textures", "brickwall_normal.jpg", aiTextureType_NORMALS);
		normal.load(false);

		if (diffuse.id) {
			std::cout << "Plane diffuse texture loaded: " << diffuse.path << std::endl;
		}
		if (specular.id) {
			std::cout << "Plane specular texture loaded: " << specular.path << std::endl;
		}

		if (normal.id) {
			std::cout << "Plane normal texture loaded: " << normal.path << " (ID: " << normal.id << ")" << std::endl;
		}
		else {
			std::cout << "Failed to load normal texture!" << std::endl;
		}

		std::vector<Vertex> vertexlist = Vertex::genList(vertices, noVertices);
		Vertex::CalcTanVectors(vertexlist, indices);

		Mesh planeMesh(vertexlist, indices);
		planeMesh.textures.push_back(diffuse);  
		planeMesh.textures.push_back(specular); 
		planeMesh.textures.push_back(normal);

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
	directory = "resources/textures";

	processNode(scene->mRootNode, scene);
}

void Model::CreateInstanceBuffers() {
	glGenBuffers(1, &instanceOffsetVBO);

	glBindBuffer(GL_ARRAY_BUFFER, instanceOffsetVBO);
	glBufferData(GL_ARRAY_BUFFER, UPPER_BOUND * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		std::string meshName = mesh->mName.C_Str();

		// skip certain meshes
		if (meshName == "ArmsFemale") {
			continue;
		}

		if (meshName.find("AKS74U_Scope") != std::string::npos || meshName == "AKS74U_Lens") {
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
		vertex.Position = glm::vec3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);

		// normals
		vertex.Normal = glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		);

		// texture coords
		if (mesh->mTextureCoords[0]) {
			vertex.TexCoords = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f);
		}

		// tangent vector
		vertex.Tangent = {
			mesh->mTangents[i].x,
			mesh->mTangents[i].y,
			mesh->mTangents[i].z
		};

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

		//if (noTex) {
		//	// diffuse color
		//	aiColor4D diff(1.0f);
		//	aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diff);

		//	// specular color
		//	aiColor4D spec(1.0f);
		//	aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &spec);

		//	return Mesh(br, vertices, indices, diff, spec);
		//}

		// ----------------- custom texture per mesh --------------------------
	/*	if (std::string(mesh->mName.C_Str()) == "Glock") {
			Texture armsDiffuse(directory, "Glock_ALB.png", aiTextureType_DIFFUSE);
			armsDiffuse.load(true);
			textures.push_back(armsDiffuse);		

			Texture armsNormal(directory, "Glock_NRM.png", aiTextureType_DIFFUSE);
			armsNormal.load(true);
			textures.push_back(armsNormal);
		}

		Texture defaultDiffuse(directory, "brickwall_diffuse.jpg", aiTextureType_DIFFUSE);
		defaultDiffuse.load(true);
		textures.push_back(defaultDiffuse);*/

		// diffuse maps
		std::vector<Texture> diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// specular maps
		std::vector<Texture> specularMaps = loadTextures(material, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// normal maps (.obj files use aiTextureType_HEIGHT) 
		std::vector<Texture> normalMaps = loadTextures(material, aiTextureType_NORMALS);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	ExtractBoneWeightForVertices(vertices, mesh, scene);

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadTextures(aiMaterial* mat, aiTextureType type) {
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::string fullPath = str.C_Str();
		std::string fileName = fullPath.substr(fullPath.find_last_of("/\\") + 1);
		std::cout << "textures: " << fileName << std::endl;

		// prevent duplicate loading
		bool skip = false;
		for (const auto& loadedTex : textures_loaded) {
			if (loadedTex.path == fileName) { // Compare file names
				textures.push_back(loadedTex);
				skip = true;
				break;
			}
		}

		if (!skip) {
			Texture tex(directory, fileName, type);
			tex.load(false);
			textures.push_back(tex);
			textures_loaded.push_back(tex);
		}
	}
	return textures;
}

void Model::cleanup() {
	for (Mesh mesh : meshes) {
		mesh.cleanup();
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