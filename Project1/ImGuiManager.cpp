#include "ImGuiManager.h"
#include "SceneManager.h"
#include "MuseumObject.h"
#include "InputManager.h"
#include <iostream>

// Static üye değişkeni tanımlama
ImGuiManager* ImGuiManager::instance = nullptr;

ImGuiManager::ImGuiManager(GLFWwindow* window) : window(window) {
    // Constructor'da Initialize çağırmıyoruz
}

ImGuiManager& ImGuiManager::GetInstance(GLFWwindow* window) {
    if (instance == nullptr) {
        instance = new ImGuiManager(window);
    }
    return *instance;
}

void ImGuiManager::DestroyInstance() {
    if (instance != nullptr) {
        instance->Shutdown();
        delete instance;
        instance = nullptr;
    }
}

void ImGuiManager::Initialize(GLFWwindow* window) {
    if (!window) {
        std::cerr << "ImGuiManager::Initialize: Geçersiz pencere!" << std::endl;
        return;
    }

    this->window = window;

    // ImGui başlatma
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Klavye navigasyonunu etkinleştir

    // ImGui stil ayarları
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 3.0f;

    // Platform/Renderer backends
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        std::cerr << "ImGui GLFW başlatılamadı!" << std::endl;
        return;
    }

    if (!ImGui_ImplOpenGL3_Init("#version 330 core")) {
        std::cerr << "ImGui OpenGL3 başlatılamadı!" << std::endl;
        return;
    }

    std::cout << "ImGui başarıyla başlatıldı" << std::endl;
}

void ImGuiManager::Shutdown() {
    if (!window) {
        std::cerr << "ImGuiManager Geçersiz pencere!" << std::endl;
        return;
    }

    // bu kısımda ben imgui da mause ile erişim problemi yaşadım bunda dolayı bu ksıımı değiştirmeyin öfd
    try {
        // Önce ImGui'nin son frame çiz
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Platform backend'ini temizle
        ImGui_ImplGlfw_Shutdown();
        
        // Renderer backend'ini temizle
        ImGui_ImplOpenGL3_Shutdown();

        // ImGui context'ini temizle
        ImGui::DestroyContext();

        // Pencere referansını temizle
        window = nullptr;

        std::cout << "ImGui  kapatıldı" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "ImGui kaptırkene hata oluştu: " << e.what() << std::endl;
    }
}

void ImGuiManager::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::Render(const SceneManager& sceneManager) {
    // Ana menü çubuğu
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Ayarlar")) {
            ImGui::MenuItem("Isik Kontrolu", NULL, &showLightControls);
            ImGui::MenuItem("Kamera Kontrolu", NULL, &showCameraControls);
            ImGui::MenuItem("Frustum Kontrolu", NULL, &showFrustumControls);
            ImGui::MenuItem("Genel Ayarlar", NULL, &showSettings);
            ImGui::MenuItem("Yardim", NULL, &showHelp);
            ImGui::MenuItem("Debug Bilgileri", NULL, &showDebugInfo);
            ImGui::MenuItem("Isik Rengi", NULL, &showLightWindow);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // çıktı penceresi
    if (showDebugInfo) {
        ShowDebugWindow(fps, cameraPosition, isMouseLocked);
    }

    // light kontrolleri
    if (showLightControls) {
        ShowLightControls();
    }

    // frustum kontrolleri
    if (showFrustumControls) {
        ShowFrustumControls(const_cast<SceneManager&>(sceneManager));
    }

    // rbt kontrolleri
    if (showRobotControls) {
        ShowRobotControls();
    }

    // help menüsü bu ksıım düzenlenebilir
    if (showHelp) {
        ImGui::Begin("Yardim", &showHelp);
        ImGui::Text("Kontroller:");
        ImGui::BulletText("WASD - Kamera hareketi");
        ImGui::BulletText("Mouse - Bakis acisi");
        ImGui::BulletText("tab - Mouse kilitleme");
        ImGui::BulletText("F11 - Tam ekran");
        ImGui::BulletText("ESC - Cikis");
        ImGui::BulletText("R - Eser bilgisi pop up'ini ac/kapat");
        ImGui::Separator();
        ImGui::Text("Robot Kontrolleri:");
        ImGui::BulletText("Ok Tuslari - Robot hareketi");
        ImGui::BulletText("Q - Robotu saat yonunde donder");
        ImGui::BulletText("E - Robotu saat yonunun tersine donder");
        ImGui::BulletText("R - Robot kol yakinaki eser bilgisini gosterme/gizleme");
        ImGui::End();
    }

    if (showDebugWindow) {
        ImGui::Begin("Isik Rengi", &showLightWindow);
        
        // Işık kontrolleri
       // if (ImGui::CollapsingHeader("Isik Rengi Ayari")) {
            Light* mainLight = sceneManager.GetLight(0);
            if (mainLight != nullptr) {
                if (ImGui::ColorEdit3("Isik Rengi", lightColor)) {
                    mainLight->SetColor(glm::vec3(lightColor[0], lightColor[1], lightColor[2]));
                }
            }
        //}

        ImGui::End();
    }

    EndFrame();
}

void ImGuiManager::UpdateDebugInfo(float currentFps, const glm::vec3& camPos, bool mouseLocked) {
    fps = currentFps;
    cameraPosition = camPos;
    isMouseLocked = mouseLocked;
}

void ImGuiManager::UpdateRobotInfo(const glm::vec3& position, float armAngle) {
    robotPosition = position;
    robotArmAngle = armAngle;
}

void ImGuiManager::ShowDebugWindow(float fps, const glm::vec3& cameraPos, bool isMouseLocked) {
    ImGui::Begin("Debug Bilgileri", &showDebugInfo);
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Kamera Pozisyonu: (%.1f, %.1f, %.1f)", 
        cameraPos.x, cameraPos.y, cameraPos.z);
    ImGui::Text("Mouse Durumu: %s", isMouseLocked ? "Kilitli" : "Serbest");
    
    ImGui::Separator();
    ImGui::Text("Robot Bilgileri:");
    ImGui::Text("Pozisyon: (%.1f, %.1f, %.1f)", 
        robotPosition.x, robotPosition.y, robotPosition.z);
    ImGui::Text("Kol Açisi: %.1f°", robotArmAngle);
    ImGui::End();
}
// light kontrollerinde atama problemi var öfd
void ImGuiManager::ShowLightControls() {
    ImGui::Begin("Isik Kontrolu", &showLightControls);
    
    if (ImGui::SliderFloat("Isik Parlakligi", &lightIntensity, 0.0f, 5.0f)) {
        // Işık yoğunluğu değiştiğinde yapılacak işlemler
    }

    ImGui::End();
}

void ImGuiManager::ShowRobotControls() {
    ImGui::Begin("Robot Kontrolu", &showRobotControls);
    
    // Robot kontrol arayüzü
    ImGui::Text("Robot Kontrolleri");
    ImGui::Separator();
    
    // Robot pozisyonu
    ImGui::Text("Pozisyon: (%.1f, %.1f, %.1f)", 
        robotPosition.x, robotPosition.y, robotPosition.z);
    
    // Kol açısı
    ImGui::Text("Kol Açisi: %.1f°", robotArmAngle);
    
    ImGui::End();
}

void ImGuiManager::UpdateArtifactInfo(const std::vector<std::shared_ptr<MuseumObject>>& objects, const glm::vec3& cameraPos) {
    if (!showArtifactInfo) return; // Eğer showArtifactInfo false ise popup gösterme

    // Robot pozisyonunu kullan
    glm::vec3 checkPosition = robotPosition; // Robot pozisyonunu kullan

    for (const auto& object : objects) {
        // Robot dışındaki objeleri kontrol et
        if (object->GetName() != "Robot" && object->IsNearCamera(checkPosition)) {
            ShowArtifactPopup(object.get());
            break; // Aynı anda sadece bir eser bilgisi göster esere yakınlığa göre ayarladım  öfd
        }
    }
}

void ImGuiManager::ShowArtifactPopup(const MuseumObject* object) {
    if (!object || !object->GetArtifactInfo()) return;

    const MuseumArtifact* artifact = object->GetArtifactInfo();
    
    // popup penceresi ayarları küçüktü büyüttüm öfd
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.8f), 
                           ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Always);
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | 
                            ImGuiWindowFlags_NoResize | 
                            ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoTitleBar;

    if (ImGui::Begin("##ArtifactInfo", nullptr, flags)) {
        // bas bilgisi
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Varsayılan font
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "%s", artifact->GetName().c_str());
        ImGui::PopFont();
        
        ImGui::Separator();
        
        //  müze seser bilgileri
        ImGui::TextWrapped("%s", artifact->GetDescription().c_str());
        ImGui::Spacing();
        
        ImGui::Text("Donem: %s", artifact->GetPeriod().c_str());
        ImGui::Text("Bulundugu Yer: %s", artifact->GetLocation().c_str());
        ImGui::Text("Malzeme: %s", artifact->GetMaterial().c_str());
    }
    ImGui::End();
}

void ImGuiManager::ShowFrustumControls(SceneManager& sceneManager) {
    ImGui::Begin("Frustum Culling Kontrolleri", &showFrustumControls);

    // Frustum culling açma/kapama
    bool isEnabled = sceneManager.IsFrustumCullingEnabled();
    if (ImGui::Checkbox("Frustum Culling Aktif", &isEnabled)) {
        sceneManager.EnableFrustumCulling(isEnabled);
    }

    // Bilgi metni
    ImGui::TextWrapped("Frustum Culling, kamera gorus alanindaki nesneleri render etmeyerek performansini artirir.");
    ImGui::TextWrapped("Aktif oldugunda, sadece gorus alaninda olan nesneler cizilir.");
    
    // Performans bilgisi
    ImGui::Separator();
    ImGui::Text("Performans Bilgisi:");
    ImGui::Text("- Gorus alanindaki nesneler render edilmez");
    ImGui::Text("- GPU kullanimi azalir");
    ImGui::Text("- FPS artisini saglar");

    ImGui::End();
} 