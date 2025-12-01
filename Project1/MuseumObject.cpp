#include "MuseumObject.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>
#include <unordered_map>
#include <map>

MuseumObject::MuseumObject(const std::string& name, const std::string& description,
    const std::string& modelPath, const std::string& texturePath,
    glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
    : name(name), description(description),
    position(position), scale(scale), rotation(rotation),
    shader("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl") {

    if (!loadModel(modelPath)) {
        std::cerr << "Model yüklenmedi: " << modelPath << std::endl;
    }
}

MuseumObject::~MuseumObject() {
    cleanup();
}

void MuseumObject::cleanup() {
    for (auto& mesh : meshes) {
        glDeleteVertexArrays(1, &mesh.VAO);
        glDeleteBuffers(1, &mesh.VBO);
        glDeleteBuffers(1, &mesh.EBO);
    }
    meshes.clear();
}

bool MuseumObject::isGLBFile(const std::string& path) const {
    std::string extension = path.substr(path.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension == "glb";
}

// texture yol düzeltme için yardımcı fonksiyonlar kullanıldı çok fazla hata aldığım için bu classa fazladan kontrol ekledim öfd 
std::string MuseumObject::FixTexturePath(const std::string& path, const std::string& basePath) {
    std::string fixedPath = path;
    
	// windows ve unix path ayracını düzeltme 
    // claude ai dosya yollarını düzeltmek için bunu önerdi bu şekilde ekledim öfd
    if (fixedPath.find("\\") != std::string::npos) {
        fixedPath = fixedPath.substr(fixedPath.find_last_of("\\") + 1);
    }
    if (fixedPath.find("/") != std::string::npos) {
        fixedPath = fixedPath.substr(fixedPath.find_last_of("/") + 1);
    }
    
    // Base path ekle
    if (!basePath.empty() && basePath.back() != '/' && basePath.back() != '\\') {
        return basePath + "/" + fixedPath;
    }
    return basePath + fixedPath;
}

// Texture yükleme için yardımcı fonksiyon
bool MuseumObject::LoadTextureFromMaterial(aiMaterial* mat, aiTextureType type, 
    const std::string& basePath, std::string& outPath, 
    std::unordered_map<std::string, Texture>& textureCache,
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
            
            std::cout << textureType << " map yolu: " << outPath << std::endl;
            return true;
        }
    }
    return false;
}

MuseumObject::Material MuseumObject::loadMaterial(aiMaterial* mat) {
    Material material;
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

    // Texture'ları yükle
    std::string basePath = directory;
    if (!basePath.empty() && basePath.back() != '/' && basePath.back() != '\\') {
        basePath += "/";
    }

    // Her texture tipi için aynı fonksiyonu kullan
    LoadTextureFromMaterial(mat, aiTextureType_DIFFUSE, basePath, 
        material.diffuseMap, textureCache, "Diffuse", 0);
    LoadTextureFromMaterial(mat, aiTextureType_NORMALS, basePath, 
        material.normalMap, textureCache, "Normal", 0);
    LoadTextureFromMaterial(mat, aiTextureType_DIFFUSE_ROUGHNESS, basePath, 
        material.roughnessMap, textureCache, "Roughness", 0);
    LoadTextureFromMaterial(mat, aiTextureType_METALNESS, basePath, 
        material.metallicMap, textureCache, "Metallic", 0);

    return material;
}

bool MuseumObject::loadModel(const std::string& path) {
    // resourcemanager üzerinden modeli yükle
    if (!ResourceManager::GetInstance().LoadModel(path)) {
        return false;
    }

    const ModelData* modelData = ResourceManager::GetInstance().GetModel(path);
    if (!modelData) {
        return false;
    }

    // model verilerini buraya aktardık
    meshes.clear();
    for (const auto& modelMesh : modelData->meshes) {
        Mesh mesh;
        mesh.name = "mesh_" + std::to_string(meshes.size());
        mesh.vertices = modelMesh.vertices;
        mesh.indices = modelMesh.indices;

        // material bilgilerini aktar
        if (!modelMesh.materialName.empty()) {
            int materialIndex = std::stoi(modelMesh.materialName);
            if (materialIndex >= 0 && materialIndex < modelData->materials.size()) {
                const auto& modelMaterial = modelData->materials[materialIndex];
                mesh.material.ambient = modelMaterial.ambient;
                mesh.material.diffuse = modelMaterial.diffuse;
                mesh.material.specular = modelMaterial.specular;
                mesh.material.shininess = modelMaterial.shininess;
                mesh.material.opacity = modelMaterial.opacity;
                mesh.material.refractiveIndex = modelMaterial.refractiveIndex;
                mesh.material.illuminationModel = modelMaterial.illuminationModel;
                mesh.material.diffuseMap = modelMaterial.diffuseMap;
                mesh.material.normalMap = modelMaterial.normalMap;
                mesh.material.roughnessMap = modelMaterial.roughnessMap;
                mesh.material.metallicMap = modelMaterial.metallicMap;
            }
        }

        setupMesh(mesh);
        meshes.push_back(std::move(mesh));
    }

    // bounding boxı güncelleyirum
    UpdateBoundingBox();
    
    std::cout << "Model başarıyla yüklendi: " << path << std::endl;
    return true;
}

void MuseumObject::setupMesh(Mesh& mesh) {
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), 
        mesh.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), 
        mesh.indices.data(), GL_STATIC_DRAW);

    const int stride = 14 * sizeof(float);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MuseumObject::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix,
    const glm::vec3& lightPos, const glm::vec3& viewPos) {
    
    shader.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);

    shader.setMat4("model", model);
    shader.setMat4("view", viewMatrix);
    shader.setMat4("projection", projectionMatrix);
    shader.setVec3("viewPos", viewPos);
    shader.setVec3("lightPos", lightPos);

    for (auto& mesh : meshes) {
        shader.setVec3("material.ambient", mesh.material.ambient);
        shader.setVec3("material.diffuse", mesh.material.diffuse);
        shader.setVec3("material.specular", mesh.material.specular);
        shader.setFloat("material.shininess", mesh.material.shininess);
        shader.setFloat("material.brightness", mesh.material.brightness);

        if (!mesh.material.diffuseMap.empty()) {
            if (mesh.material.diffuseMap != lastBoundTexture) {
                if (!lastBoundTexture.empty()) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }

                const Texture* texture = ResourceManager::GetInstance().GetTexture(mesh.material.diffuseMap);
                if (texture) {
                    glActiveTexture(GL_TEXTURE0);
                    texture->Bind(0);
                    lastBoundTexture = mesh.material.diffuseMap;
                    shader.setInt("material.diffuseMap", 0);
                    shader.setBool("material.hasDiffuseMap", true);
                }
            }
        } else {
            if (!lastBoundTexture.empty()) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
                shader.setBool("material.hasDiffuseMap", false);
                lastBoundTexture.clear();
            }
        }

        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    if (!lastBoundTexture.empty()) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        lastBoundTexture.clear();
    }
}

void MuseumObject::SetPosition(glm::vec3 newPosition) {
    position = newPosition;
}

void MuseumObject::SetScale(glm::vec3 newScale) {
    scale = newScale;
}

void MuseumObject::SetRotation(glm::vec3 newRotation) {
    rotation = newRotation;
}

void MuseumObject::setColor(glm::vec3 color, float brightness) {
    for (auto& mesh : meshes) {
        mesh.material.useCustomMaterial = true;
        mesh.material.isColorManuallySet = true;
        mesh.material.useVertexColors = false;

        mesh.material.diffuse = color;
        mesh.material.ambient = color * 0.2f;
        mesh.material.specular = glm::vec3(0.5f);
        mesh.material.brightness = brightness;

        mesh.material.diffuseMap.clear();
    }
}

void MuseumObject::setBrightness(float brightness) {
    for (auto& mesh : meshes) {
        mesh.material.brightness = glm::clamp(brightness, 0.1f, 5.0f);
    }
}

void MuseumObject::enableVertexColors(bool enable) {
    for (auto& mesh : meshes) {
        mesh.material.useCustomMaterial = enable;
        mesh.material.useVertexColors = enable;
        mesh.material.isColorManuallySet = false;
    }
}

bool MuseumObject::loadTexture(const std::string& path, const std::string& type) {
    if (textureCache.find(path) != textureCache.end()) {
		return true; // texture zaten yüklediyse tekrara etmemesi için yazdım
    }

    try {
        Texture texture(path);
        textureCache[path] = std::move(texture);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Texture yükleme hatası: " << path << " - " << e.what() << std::endl;
        return false;
    }
}

bool MuseumObject::IsNearCamera(const glm::vec3& cameraPos) const {
    if (!artifactInfo) return false;
    
    glm::vec3 objectCenter = position;
    float distance = glm::distance(cameraPos, objectCenter);
    return distance <= MuseumArtifact::POPUP_DISTANCE;
}