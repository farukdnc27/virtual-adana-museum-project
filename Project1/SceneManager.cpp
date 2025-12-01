#include "SceneManager.h"
#include "ImGuiManager.h"
#include "Robot.h"
#include <iostream>
#include <algorithm>

SceneManager::SceneManager()
	: sceneName("Default Scene"), sceneCenter(0.0f, 0.0f, 0.0f) {
	imguiManager = &ImGuiManager::GetInstance();
	std::cout << "SceneManager olusturuldu" << std::endl;
}

SceneManager::~SceneManager() {
	ClearScene();
	std::cout << "SceneManager temizlendi" << std::endl;
}

std::shared_ptr<MuseumObject> SceneManager::AddMuseumObject(const std::string& name, const std::string& description,
	const std::string& modelPath, const std::string& texturePath,
	const glm::vec3& position, const glm::vec3& scale,
	const glm::vec3& rotation) {

	auto obj = std::make_shared<MuseumObject>(name, description, modelPath, texturePath,
		position, scale, rotation);
	museumObjects.push_back(obj);

	std::cout << "Muze objesi eklendi: " << name << std::endl;
	return obj;
}

void SceneManager::RemoveMuseumObject(const std::string& name) {
	auto it = std::remove_if(museumObjects.begin(), museumObjects.end(),
		[&name](const std::shared_ptr<MuseumObject>& obj) {
			return obj->GetName() == name;
		});

	if (it != museumObjects.end()) {
		museumObjects.erase(it, museumObjects.end());
		std::cout << "Muze objesi silindi: " << name << std::endl;
	}
}

MuseumObject* SceneManager::GetMuseumObject(const std::string& name) {
	for (auto& obj : museumObjects) {
		if (obj->GetName() == name) {
			return obj.get();
		}
	}
	return nullptr;
}

std::vector<MuseumObject*> SceneManager::GetAllMuseumObjects() {
	std::vector<MuseumObject*> objects;
	for (auto& obj : museumObjects) {
		objects.push_back(obj.get());
	}
	return objects;
}

void SceneManager::AddLight(const glm::vec3& position, const glm::vec3& color,
	float ambientStrength, float specularStrength,
	float intensity, float range) {

	Light light(position, color, ambientStrength, specularStrength, intensity, range);
	lights.push_back(light);

	//burasi kontrol edilecek
   /* auto lightCube = std::make_unique<LightCube>(position, color);
	lightCubes.push_back(std::move(lightCube));*/

	std::cout << "Isik eklendi. Toplam isik sayisi: " << lights.size() << std::endl;
}

void SceneManager::RemoveLight(int index) {
	if (index >= 0 && index < lights.size()) {
		lights.erase(lights.begin() + index);
		//  lightCubes.erase(lightCubes.begin() + index);
		std::cout << "Isik silindi. Kalan isik sayisi: " << lights.size() << std::endl;
	}
}

Light* SceneManager::GetLight(int index) const {
	if (index >= 0 && index < lights.size()) {
		return const_cast<Light*>(&lights[index]);
	}
	return nullptr;
}

std::vector<Light>& SceneManager::GetAllLights() {
	return lights;
}

void SceneManager::LoadAdanaMuseumScene() {
	ClearScene();
	SetSceneName("Adana Muzesi");
	SetSceneCenter(glm::vec3(0.0f, 0.0f, 0.0f));

	std::cout << "Adana Muzesi sahnesi yukleniyor..." << std::endl;

	//  muze binasi
	AddMuseumObject("Muze Binasi", "Muze giris salonu", "models/museum/museum11.obj", "",
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f));

	// Lahit 1
	//auto kod okuanabilirliği açısından kullanıldı
	auto lahit1 = AddMuseumObject("lahit1", "Antik Lahit 1", "models/lahit1/lahit1.obj", "",
		glm::vec3(-17.0f, 0.75f, 6.0f), glm::vec3(1.0f), glm::vec3(0.0f));
	lahit1->SetArtifactInfo(std::make_shared<MuseumArtifact>(
		"lahit1",
		"Akhilleus Lahdi",
		"Akhilleus Lahdi,1958'de Tarsus kazilarinda ortaya cikarildi ve Troya savasini yuksek kabartma olarak tasvir eden bir lahittir. ",
		"Roma Dönemi (M.S. 2-3. yüzyil)",
		"Adana Muzesi",
		"Mermer"
	));

	// Lahit 2
	auto lahit2 = AddMuseumObject("lahit2", "Antik Lahit 2", "models/lahit2/lahit2.obj", "",
		glm::vec3(-17.0f, 0.75f, -7.0f), glm::vec3(1.0f), glm::vec3(0.0f, 180.0f, 0.0f));
	lahit2->SetArtifactInfo(std::make_shared<MuseumArtifact>(
		"lahit 2",
		"Antik Lahit ",
		"Bu lahit, Roma dönemine ait önemli bir mezar anitidir. Üzerindeki kabartmalar ve süslemeler, dönemin sanat anlayisini yansitmaktadir.Deforme olmustur ve kapaği yoktur.",
		"Roma Dönemi (M.S. 2-3. yüzyil)",
		"Adana Muzesi",
		"Mermer"
	));

	// Lahit 3
	auto lahit3 = AddMuseumObject("lahit3", "Antik Lahit 3", "models/lahit3/lahit3.obj", "",
		glm::vec3(-4.0f, 0.75f, -7.0f), glm::vec3(1.0f), glm::vec3(0.0f, 90.0f, 0.0f));
	lahit3->SetArtifactInfo(std::make_shared<MuseumArtifact>(
		"lahit 3",
		"Mezar tasi",
		"Bu Mezar Tasi, Roma dönemine ait bir mezar tasisidir. Üzerindeki kabartmalar ve süslemeler, dönemin sanat anlayisini yansitmaktadir.",
		"Roma Dönemi (M.S. 2. yüzyil)",
		"Adana Muzesi",
		"Taş"
	));

	// Lahit 4
	auto lahit4 = AddMuseumObject("lahit4", "Büyük Lahit", "models/lahit4/lahit4.obj", "",
		glm::vec3(-27.0f, 1.0f, 0.0f), glm::vec3(0.75f), glm::vec3(0.0f, -90.0f, 0.0f));
	lahit4->SetArtifactInfo(std::make_shared<MuseumArtifact>(
		"lahit 4",
		"Arabali Tanri Tarhunda",
		"Bogalarin cektigi bir araba uzerinde ayakta duran Firtina Tanrisinin temsil eden 2,5 metre yukseklikindeki heykel,1998 yilinda Adana'nin Cine koyundeki bir tarlada gomulub bulunmustur.",
		"Geç Hitit Dönemi (M.Ö. 1200–700)",
		"Adana Muzesi",
		"Siyah bazalt tasi ve kalker tasi"
	));

	// Lahit 5
	auto lahit5 = AddMuseumObject("lahit5", "Süslü Lahit", "models/lahit5/lahit5.obj", "",
		glm::vec3(-5.0f, 0.75f, 6.0f), glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	lahit5->SetArtifactInfo(std::make_shared<MuseumArtifact>(
		"lahit 5",
		"Romali Senatör Heykeli",
		"Roma toga'si giymis bir senatorun betimleyen bu tunc heykel, Adana'nin Karatas ilcesi yakinlarinda bulunmustur.",
		"Roma Dönemi (M.S. 1.yüzyil)",
		"Adana Muzesi",
		"Tunc"
	));
	//bu animasyonu kendinden olan robot
	/*AddMuseumObject("robot", "robot", "models/robot/robot.glb", "",
		glm::vec3(-5.0f, 1.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f, 90.0f, 0.0f));

	AddMuseumObject("robot", "robot", "models/robot/kol.obj", "",
		glm::vec3(-5.0f, 1.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f, 90.0f, 0.0f));*/


		/* bu benim test objem kahve bardagi
	AddMuseumObject("baricade", "Sergi Objesi", "models/baricade/coffee_cup_obj.obj", "",
		glm::vec3(10.0f, 0.0f, 3.0f), glm::vec3(1.0f), glm::vec3(0.0f, 180.0f, 0.0f));
*/
	// Ana isik
	AddLight(glm::vec3(6.0f, 6.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		0.5f, 0.5f, 0.3f, 75.0f);

	////ekstra ışığın rengini güzelleştirmek için
	//AddLight(glm::vec3(-15.0f, 4.0f, 0.0f), glm::vec3(0.9f, 0.9f, 1.0f),
	//    0.3f, 0.4f, 0.25f, 50.0f);

	std::cout << "Adana Muzesi sahnesi yuklendi. Objeler: " << GetObjectCount()
		<< ", Isikklar: " << GetLightCount() << std::endl;
}
/*
void SceneManager::LoadTestScene() {
	ClearScene();
	SetSceneName("Test Sahnesi");
	SetSceneCenter(glm::vec3(0.0f, 0.0f, 0.0f));

	// kod calismas ise bunu cagir hatayi gor ancak cube.obj ekle meyui unutma
	AddMuseumObject("test_cube", "Test Kupu", "models/cube.obj", "",
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
	AddLight(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		0.3f, 0.5f, 0.8f, 10.0f);

	std::cout << "Test sahnesi yuklendi" << std::endl;
}*/

void SceneManager::ClearScene() {
	museumObjects.clear();
	lights.clear();
	//lightCubes.clear();
	std::cout << "Sahne temizlendi" << std::endl;
}

void SceneManager::Update(const glm::vec3& cameraPosition, float deltaTime) {
	// Isik yogunluklarini guncelle
	UpdateLightIntensities(cameraPosition);

	// Light cube pozisyon guncelle
   /* for (size_t i = 0; i < lights.size() && i < lightCubes.size(); ++i) {
		lightCubes[i]->SetPosition(lights[i].GetPosition());
	}*/
}

void SceneManager::Draw(const glm::mat4& view, const glm::mat4& projection,
	const glm::vec3& cameraPosition) {

	// Frustum'u güncelle
	if (enableFrustumCulling) {
		frustum.Update(view, projection);
	}

	// Tüm shader'lar için ışık ayarları
	SetupLightsForShaders();

	// Müze objelerini çiz
	for (auto& obj : museumObjects) {
		// Frustum culling kontrolü
		if (enableFrustumCulling && !obj->IsVisible(frustum)) {
			continue; // Görünür değilse çizme
		}

		// Her obje için ana ışık kullanılıyor
		glm::vec3 lightPos = lights.empty() ? glm::vec3(0.0f) : lights[0].GetPosition();
		obj->Draw(view, projection, lightPos, cameraPosition);
	}

	// Light cube'lari ben  bunu testlerde kullnyorum
	/*
	for (auto& lightCube : lightCubes) {
		lightCube->Draw(view, projection);
	}
	*/

	// Eser bilgilerini güncelle
	imguiManager->UpdateArtifactInfo(museumObjects, cameraPosition);
}

void SceneManager::UpdateLightIntensities(const glm::vec3& cameraPosition) {
	for (auto& light : lights) {
		float distance = glm::length(light.GetPosition() - cameraPosition);

		if (distance <= LIGHT_ACTIVATION_DISTANCE) {
			// Mesafe azaldikca yogunluk artar
			float intensity = 1.0f - (distance / LIGHT_ACTIVATION_DISTANCE);
			light.SetIntensity(intensity * 0.8f); // Maksimum %80 yogunluk
		}
		else {
			light.SetIntensity(0.0f);
		}
	}
}

void SceneManager::SetupLightsForShaders() {
	// Her muze objesi icin shader isik ayarlari
	for (auto& obj : museumObjects) {
		const Shader& shader = obj->GetShader();


		if (!lights.empty()) {
			ShaderSetup::SetupLight(shader, lights[0]);
		}
	}
}

void SceneManager::PrintSceneInfo() {
	std::cout << "\n=== SAHNE BILGILERI ===" << std::endl;
	std::cout << "Sahne Adi: " << sceneName << std::endl;
	std::cout << "Sahne Merkezi: X:" << sceneCenter.x << " Y:" << sceneCenter.y << " Z:" << sceneCenter.z << std::endl;
	std::cout << "Toplam Objeler: " << GetObjectCount() << std::endl;
	std::cout << "Toplam Isiklar: " << GetLightCount() << std::endl;

	std::cout << "\n--- OBJELER ---" << std::endl;
	for (size_t i = 0; i < museumObjects.size(); ++i) {
		std::cout << i + 1 << ". " << museumObjects[i]->GetName() << std::endl; 
	}

	std::cout << "\n--- ISIKLAR ---" << std::endl;
	for (size_t i = 0; i < lights.size(); ++i) {
		const auto& light = lights[i];
		std::cout << i + 1 << ". Pozisyon: (" << light.GetPosition().x << ", "
			<< light.GetPosition().y << ", " << light.GetPosition().z << ")" << std::endl;
	}
	std::cout << "===============" << std::endl;
}