#include "MuseumArtifact.h"

MuseumArtifact::MuseumArtifact(const std::string& id, const std::string& name,
                              const std::string& description, const std::string& period,
                              const std::string& location, const std::string& material)
    : id(id), name(name), description(description), period(period), location(location), material(material) {
} 