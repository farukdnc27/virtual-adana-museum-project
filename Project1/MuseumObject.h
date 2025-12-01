#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Texture.h"
#include "Shader.h"
#include <unordered_map>
#include <memory>
#include "Frustum.h"
#include "ResourceManager.h"
#include "MuseumArtifact.h"


//bu classı abstract olarak tasarlamıaştım başlangıçta ancak sonrasında her obje için tekrar tekrar fonksiyonları doldurmak 
//pek kullanışlı değildi onun yerine virtual tanımlaması kullanarak bağzı kısımlarda override edebilmeyi sağladım
// robotu yazan kişi için kolaylık sağlamış oldum

class MuseumObject {
public:
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
	};

	struct Material {
		// Temel materyal özellikleri
		glm::vec3 ambient = glm::vec3(1.0f);
		glm::vec3 diffuse = glm::vec3(1.0f);
		glm::vec3 specular = glm::vec3(0.5f);
		float shininess = 32.0f;
		float opacity = 1.0f;
		float refractiveIndex = 1.45f;
		int illuminationModel = 2;

		// Görsel ayarlar
		//bunları benim fonksiyonlarda değiştirme fonksiyonu ekleyebilmek için yazdım
		float brightness = 1.0f;
		bool useCustomMaterial = false; // Vertex renkleri veya manuel renk ayarı için  flag
		bool useVertexColors = false;   // Vertex renkleri kullanımı için flag
		bool isColorManuallySet = false; // Manuel renk ayarı için flag

		// Texture yolları
		std::string diffuseMap;
		std::string normalMap;
		std::string roughnessMap;
		std::string metallicMap;

		Material() = default; //zorunlu constructor olsun diye bu şrkilde kullandım
	};

	struct Mesh {
		std::string name;  // mesh adı
		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		Material material;
		unsigned int VAO, VBO, EBO;
	};

	// Bounding box için yapı performasn optimizasyonuiçin ekledim
	struct BoundingBox {
		glm::vec3 min;
		glm::vec3 max;
		float radius; // sınırlayıcı küre için yarıçap

		BoundingBox() : min(glm::vec3(FLT_MAX)), max(glm::vec3(-FLT_MAX)), radius(0.0f) {}
	};

	MuseumObject(const std::string& name, const std::string& description,
		const std::string& modelPath, const std::string& texturePath,
		glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);
	virtual ~MuseumObject();

	virtual void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix,
		const glm::vec3& lightPos, const glm::vec3& viewPos);
	void SetPosition(glm::vec3 newPosition);
	void SetScale(glm::vec3 newScale);
	void SetRotation(glm::vec3 newRotation);
	void enableVertexColors(bool enable);
	bool loadTexture(const std::string& path, const std::string& type);
	void setColor(glm::vec3 color, float brightness);
	void setBrightness(float brightness);

	const Shader& GetShader() const { return shader; }
	const std::string& GetName() const { return name; }
	const std::string& GetDescription() const { return description; }
	glm::vec3 GetPosition() const { return position; }

	bool loadModel(const std::string& path);
	Material loadMaterial(aiMaterial* mat);
	/*Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	void processNode(aiNode* node, const aiScene* scene);*/
	bool isGLBFile(const std::string& path) const;
	void setupMesh(Mesh& mesh);

	const std::vector<Mesh>& GetMeshes() const { return meshes; }

	// Görünürlük kontrolü burda yapılarak performasn artışı sağladım
	// normalde 4 dakika sürüyordu şuan 37 saniye
	bool IsVisible(const Frustum& frustum) const {
		// Bounding box'ı dünya koordinatlarına dönüştür
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, scale);

		// Dönüştürülmüş merkez noktası
		glm::vec3 center = glm::vec3(model * glm::vec4(boundingBox.min + (boundingBox.max - boundingBox.min) * 0.5f, 1.0f));
		
		// Dönüştürülmüş yarıçap (en büyük ölçek faktörünü kullan)
		float maxScale = std::max(std::max(scale.x, scale.y), scale.z);
		float transformedRadius = boundingBox.radius * maxScale;

		return frustum.IsSphereVisible(center, transformedRadius);
	}

	// Bounding box'ı güncelle
	void UpdateBoundingBox() {
		boundingBox = BoundingBox();
		
		// Tüm vertex'leri kontrol et
		for (const auto& mesh : meshes) {
			for (size_t i = 0; i < mesh.vertices.size(); i += 14) { // 14 = vertex boyutu
				glm::vec3 vertex(
					mesh.vertices[i],
					mesh.vertices[i + 1],
					mesh.vertices[i + 2]
				);

				// Min ve max noktaları güncelle
				boundingBox.min = glm::min(boundingBox.min, vertex);
				boundingBox.max = glm::max(boundingBox.max, vertex);
			}
		}

		// merkez noktası
		glm::vec3 center = boundingBox.min + (boundingBox.max - boundingBox.min) * 0.5f;
		
		// yarıçapı hesapla 
		float maxDist = 0.0f;
		for (const auto& mesh : meshes) {
			for (size_t i = 0; i < mesh.vertices.size(); i += 14) {
				glm::vec3 vertex(
					mesh.vertices[i],
					mesh.vertices[i + 1],
					mesh.vertices[i + 2]
				);
				float dist = glm::length(vertex - center);
				maxDist = std::max(maxDist, dist);
			}
		}
		boundingBox.radius = maxDist;
	}

	// eser bilgisi için yeni metodlar
	void SetArtifactInfo(std::shared_ptr<MuseumArtifact> artifact) { artifactInfo = artifact; }
	const MuseumArtifact* GetArtifactInfo() const { return artifactInfo.get(); }
	bool IsNearCamera(const glm::vec3& cameraPos) const;

protected:
	std::vector<Mesh> meshes;
	std::unordered_map<std::string, Texture> textureCache;
	std::string lastBoundTexture; // son kullanılan textureı takip etmek için

private:
	std::string name;
	std::string description;
	std::string directory;
	std::vector<Material> materials;
	Shader shader;
	bool isGLBModel;
	BoundingBox boundingBox;
	std::shared_ptr<MuseumArtifact> artifactInfo; // eser bilgisini imgui aktarma için kullandığım class 

	void cleanup();

	// Yardımcı fonksiyonlar
	static std::string FixTexturePath(const std::string& path, const std::string& basePath);
	static bool LoadTextureFromMaterial(aiMaterial* mat, aiTextureType type, 
		const std::string& basePath, std::string& outPath, 
		std::unordered_map<std::string, Texture>& textureCache,
		const std::string& textureType, int materialIndex);
};