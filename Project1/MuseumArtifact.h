#pragma once
#include <string>
#include <glm/glm.hpp>

class MuseumArtifact {
public:
    MuseumArtifact(const std::string& id, const std::string& name, 
                  const std::string& description, const std::string& period,
                  const std::string& location, const std::string& material);

    const std::string& GetId() const { return id; }
    const std::string& GetName() const { return name; }
    const std::string& GetDescription() const { return description; }
    const std::string& GetPeriod() const { return period; }
    const std::string& GetLocation() const { return location; }
    const std::string& GetMaterial() const { return material; }

    // POPup gösterim mesafesi
    static constexpr float POPUP_DISTANCE = 5.0f;

private:
    std::string id;           // ESERID
    std::string name;         // adi
    std::string description;  // aciklamasi
	std::string period;       // tarhi
    std::string location;     // bulundugu mekan
	std::string material;     // materyali
}; 