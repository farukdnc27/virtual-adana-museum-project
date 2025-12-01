#include "ResourceManager.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <fstream>  


//bu classı museum object classından ayırdım daha iyi modüler bir yapı olsun diye
//kaynak yükleme erişim ve yönetimi için hazırladım ve kullanıyorum
bool ResourceManager::LoadTexture(const std::string& path) {
    if (textureCache.find(path) != textureCache.end()) {
        return true; 
    }

    try {
        Texture texture(path);
        textureCache[path] = std::move(texture);
        std::cout << "Texture yüklendi: " << path << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Texture yükleme hatası: " << path << " - " << e.what() << std::endl;
        return false;
    }
}

const Texture* ResourceManager::GetTexture(const std::string& path) {
    auto it = textureCache.find(path);
    if (it != textureCache.end()) {
        return &it->second;
    }
    return nullptr;
}

void ResourceManager::UnloadTexture(const std::string& path) {
    textureCache.erase(path);
}

void ResourceManager::UnloadAllTextures() {
    textureCache.clear();
}

bool ResourceManager::LoadModel(const std::string& path) {
    if (modelCache.find(path) != modelCache.end()) {
        return true;
    }

    //  dizinin ayarlaarını yapptım
    size_t lastSlash = path.find_last_of("/\\");
    std::string directory = (lastSlash != std::string::npos) ? path.substr(0, lastSlash) : "";

    // model yükleme işlemi
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_FlipUVs);

    if (!scene) {
        std::cerr << "Model yüklenemedi: " << importer.GetErrorString() << std::endl;
        return false;
    }

    // model verilerini oluştur
    auto modelData = std::make_unique<ModelData>();
    modelData->directory = directory;
    modelData->isGLBModel = IsGLBFile(path);

    // meshleri işle
    ProcessNode(scene->mRootNode, scene, *modelData);

    // materiaları işle
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* material = scene->mMaterials[i];
        modelData->materials.push_back(ProcessMaterial(material, directory));
    }

    // model tipini kaydet
    bool isGLB = modelData->isGLBModel;

    // modeli önbelleğe ekle
    modelCache[path] = std::move(modelData);
    std::cout << (isGLB ? "GLB" : "OBJ") << " model yüklendi: " << path << std::endl;
    return true;
}

const ModelData* ResourceManager::GetModel(const std::string& path) {
    auto it = modelCache.find(path);
    if (it != modelCache.end()) {
        return it->second.get();
    }
    return nullptr;
}

void ResourceManager::UnloadModel(const std::string& path) {
    modelCache.erase(path);
}

void ResourceManager::UnloadAllModels() {
    modelCache.clear();
}

std::string ResourceManager::FixTexturePath(const std::string& path, const std::string& basePath) const {
    std::string fixedPath = path;
    
    // windows ve unix path ayracını düzelt
    if (fixedPath.find("\\") != std::string::npos) {
        fixedPath = fixedPath.substr(fixedPath.find_last_of("\\") + 1);
    }
    if (fixedPath.find("/") != std::string::npos) {
        fixedPath = fixedPath.substr(fixedPath.find_last_of("/") + 1);
    }
    
    // base path ekle
    if (!basePath.empty() && basePath.back() != '/' && basePath.back() != '\\') {
        return basePath + "/" + fixedPath;
    }
    return basePath + fixedPath;
}

bool ResourceManager::IsGLBFile(const std::string& path) const {
    std::string extension = path.substr(path.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension == "glb";
}

void ResourceManager::ProcessNode(aiNode* node, const aiScene* scene, ModelData& modelData) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        modelData.meshes.push_back(ProcessMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, modelData);
    }
}

ModelData::Mesh ResourceManager::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    ModelData::Mesh result;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // vertex verilerini işle
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // position
        vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });

        // normal
        vertices.insert(vertices.end(), { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z });

        // texture coordinates
        if (mesh->mTextureCoords[0]) {
            float u = mesh->mTextureCoords[0][i].x;
            float v = 1.0f - mesh->mTextureCoords[0][i].y;
            vertices.insert(vertices.end(), { u, v });
        }
        else {
            vertices.insert(vertices.end(), { 0.0f, 0.0f });
        }

        // tangent ve bitangent
        auto processTangentSpace = [&](const aiVector3D* vec, float defaultVal = 0.0f) {
            vertices.insert(vertices.end(),
                vec ? std::initializer_list<float>{vec[i].x, vec[i].y, vec[i].z} :
                std::initializer_list<float>{ defaultVal, defaultVal, defaultVal });
        };

        processTangentSpace(mesh->mTangents);
        processTangentSpace(mesh->mBitangents);
    }

    // ındices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
    }

    // material
    if (mesh->mMaterialIndex >= 0) {
        result.materialName = std::to_string(mesh->mMaterialIndex);
    }

    result.vertices = vertices;
    result.indices = indices;
    return result;
}

ModelData::Material ResourceManager::ProcessMaterial(aiMaterial* mat, const std::string& basePath) {
    ModelData::Material material;
    if (!mat) return material;

    aiColor3D color;
    float value;
    int illum;

    // Temel materyal özelliklerini yükle
    if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
        material.ambient = glm::vec3(color.r, color.g, color.b);
    }
    if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        material.diffuse = glm::vec3(color.r, color.g, color.b);
    }
    if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
        material.specular = glm::vec3(color.r, color.g, color.b);
    }
    if (mat->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS) {
        material.shininess = value;
    }
    if (mat->Get(AI_MATKEY_OPACITY, value) == AI_SUCCESS) {
        material.opacity = value;
    }
    if (mat->Get(AI_MATKEY_REFRACTI, value) == AI_SUCCESS) {
        material.refractiveIndex = value;
    }
    if (mat->Get(AI_MATKEY_SHADING_MODEL, illum) == AI_SUCCESS) {
        material.illuminationModel = illum;
    }

    // textureları yükle
    std::string outPath;
    LoadTextureFromMaterial(mat, aiTextureType_DIFFUSE, basePath, 
        material.diffuseMap, "Diffuse", 0);
    LoadTextureFromMaterial(mat, aiTextureType_NORMALS, basePath, 
        material.normalMap, "Normal", 0);
    LoadTextureFromMaterial(mat, aiTextureType_DIFFUSE_ROUGHNESS, basePath, 
        material.roughnessMap, "Roughness", 0);
    LoadTextureFromMaterial(mat, aiTextureType_METALNESS, basePath, 
        material.metallicMap, "Metallic", 0);

    return material;
}

bool ResourceManager::LoadTextureFromMaterial(aiMaterial* mat, aiTextureType type,
    const std::string& basePath, std::string& outPath,
    const std::string& textureType, int materialIndex) {
    
    if (mat->GetTextureCount(type) > 0) {
        aiString str;
        if (mat->GetTexture(type, 0, &str) == AI_SUCCESS) {
            std::string texturePath = str.C_Str();
            outPath = FixTexturePath(texturePath, basePath);
            
            // Dosya varlığını kontrol et
            std::ifstream file(outPath);
            if (!file.good()) {
                std::cerr << "UYARI: " << textureType << " map dosyası bulunamadı: " << outPath << std::endl;
                return false;
            }
            file.close();
            
            // texture yükle
            LoadTexture(outPath);
            std::cout << textureType << " map yüklendi: " << outPath << std::endl;
            return true;
        }
    }
    return false;
} 