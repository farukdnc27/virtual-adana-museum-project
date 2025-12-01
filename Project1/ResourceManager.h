#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>
#include "Texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// MODEL DATA TUTTMA YAPISI
struct ModelData {
    struct Mesh {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        std::string materialName;
    };

    struct Material {
        glm::vec3 ambient = glm::vec3(1.0f);
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(0.5f);
        float shininess = 32.0f;
        float opacity = 1.0f;
        float refractiveIndex = 1.45f;
        int illuminationModel = 2;

        // texture yolları
        std::string diffuseMap;
        std::string normalMap;
        std::string roughnessMap;
        std::string metallicMap;
    };

    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    std::string directory;
    bool isGLBModel = false;
};

class ResourceManager {
public:
    static ResourceManager& GetInstance() {
        static ResourceManager instance;
        return instance;
    }

    // texture yönetimi
    bool LoadTexture(const std::string& path);
    const Texture* GetTexture(const std::string& path);
    void UnloadTexture(const std::string& path);
    void UnloadAllTextures();

    // model yönetimi
    bool LoadModel(const std::string& path);
    const ModelData* GetModel(const std::string& path);
    void UnloadModel(const std::string& path);
    void UnloadAllModels();

    // yrdmcı fonksiyonlar
    std::string FixTexturePath(const std::string& path, const std::string& basePath) const;
    bool IsGLBFile(const std::string& path) const;

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // texture ve model için cacheler
    std::unordered_map<std::string, Texture> textureCache;
    std::unordered_map<std::string, std::unique_ptr<ModelData>> modelCache;

    // model yükleme yardımcı fonksiyonları
    void ProcessNode(aiNode* node, const aiScene* scene, ModelData& modelData);
    ModelData::Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    ModelData::Material ProcessMaterial(aiMaterial* mat, const std::string& basePath);
    bool LoadTextureFromMaterial(aiMaterial* mat, aiTextureType type,
        const std::string& basePath, std::string& outPath,
        const std::string& textureType, int materialIndex);
}; 