#pragma once

#include "../../Core/Model.h"

class PrimitiveModel : public Model {
public:
    enum class Type {
        CUBE, 
        PLANE
    };

    PrimitiveModel(const std::string& name, Type type, ModelCreateInfo& createInfo)
        : Model(name, createInfo), m_type(type) {}

 /*   void Init() {
        if (m_type == Type::CUBE) {
            InitCube();
        }
        else if (m_type == Type::PLANE) {
            InitPlane();
        }
    }*/

private:
    Type m_type;

    void InitCube() {
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

    void InitPlane() {
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