#pragma once

#include "Model.h"

class Cube : public Model {
public: 
	Cube(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f))
		: Model(pos, size) {}

	void init() {
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

        Texture diffuse("resources/textures", "Concrete_Diff.png", aiTextureType_DIFFUSE);
        diffuse.load(false);

        Texture specular("resources/textures", "Concrete_Spec.png", aiTextureType_SPECULAR);
        specular.load(false);

        if (diffuse.id) {
            std::cout << "Cube diffuse texture loaded: " << diffuse.path << std::endl;
        }
        if (specular.id) {
            std::cout << "Cube specular texture loaded: " << specular.path << std::endl;
        }

        Mesh cubeMesh(Vertex::genList(vertices, noVertices), indices);
        cubeMesh.textures.push_back(diffuse);    // Add diffuse texture
        cubeMesh.textures.push_back(specular);  // Add specular texture

        meshes.push_back(cubeMesh);
	}

    
   /* void drawCube(Shader& shader) {
        shader.activate();
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position) * rotation;
        shader.setMat4("model", model);
        for (auto& mesh : meshes) {
            mesh.draw(shader);
        }
    }*/

};


class Plane : public Model {
public: 
    Plane(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f))
        : Model(pos, size) {}

    void init() {
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

        Texture diffuse("resources/textures", "atlas.png", aiTextureType_DIFFUSE);
        diffuse.load(false);

        Texture specular("resources/textures", "atlas.png", aiTextureType_SPECULAR);
        specular.load(false);

        if (diffuse.id) {
            std::cout << "Plane diffuse texture loaded: " << diffuse.path << std::endl;
        }
        if (specular.id) {
            std::cout << "Plane specular texture loaded: " << specular.path << std::endl;
        }

        Mesh planeMesh(Vertex::genList(vertices, noVertices), indices);
        planeMesh.textures.push_back(diffuse);    // Add diffuse texture
        planeMesh.textures.push_back(specular);   // Add specular texture

        meshes.push_back(planeMesh);
    }
};

//class Lamp : public Cube {
//public:
//    glm::vec3 lightColor;
//
//    Lamp(glm::vec3 lightColor = glm::vec3(1.0f),
//        glm::vec4 ambient = glm::vec4(1.0f),
//        glm::vec4 diffuse = glm::vec4(1.0f),
//        glm::vec4 specular = glm::vec4(1.0f),
//        float k0 = 1.0f,
//        float k1 = 0.07f,
//        float k2 = 0.017f,
//        glm::vec3 pos = glm::vec3(0.0f),
//        glm::vec3 size = glm::vec3(1.0f))
//        : lightColor(lightColor)
//};