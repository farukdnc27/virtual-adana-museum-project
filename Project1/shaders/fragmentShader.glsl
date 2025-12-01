#version 330 core
out vec4 FragColor;

#define NUM_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;
in vec3 VertexColor; // vertex shaderdan gelen renk bilgisi

uniform bool specialTextureMode;
uniform sampler2D specialTexture;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float brightness;
    float shininess;
    float opacity;
    float refractiveIndex;
    int illuminationModel;
    
    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasRoughnessMap;
    bool hasMetallicMap;
    bool useVertexColors;
};

// Işık yapısı 3 farklı tür için yazdım 
struct Light {
    vec3 position;
    vec3 direction;
    vec3 color;
    float ambientStrength;
    float specularStrength;
    float intensity;
    float range;
    int type; // 0: Point, 1: Spot, 2: Directional
    float cutOff;
    float outerCutOff;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

// Texture 
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;
uniform sampler2D metallicMap;

// PBR fonksiyonları
const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 calculateLight(vec3 normal, vec3 viewDir, vec3 albedo, float roughness, float metallic) {
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 L;
    float attenuation = 1.0;
    float spotEffect = 1.0;

    // Işık türüne göre hesaplama
    if (light.type == 0) { // Point light
        L = normalize(light.position - FragPos);
        float distance = length(light.position - FragPos);
        if (distance > light.range) return vec3(0.0);
        attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);//bu değerleri kullanmamı ai söyledi
    }
    else if (light.type == 1) { // Spot light
        L = normalize(light.position - FragPos);
        float distance = length(light.position - FragPos);
        if (distance > light.range) return vec3(0.0);
        
        // Spot ışık açısı kontrolü
        float theta = dot(L, normalize(-light.direction));
        float epsilon = light.cutOff - light.outerCutOff;
        spotEffect = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        
        attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    }
    else if (light.type == 2) { // Directional light
        L = normalize(-light.direction);
    }

    vec3 H = normalize(viewDir + L);

    // Ambient
    vec3 ambient = light.ambientStrength * albedo * light.color;

    // Diffuse
    float diff = max(dot(normal, L), 0.0);
    vec3 diffuse = diff * albedo * light.color;

    // Specular
    vec3 F = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);
    float NDF = DistributionGGX(normal, H, roughness);
    float G = GeometrySmith(normal, viewDir, L, roughness);
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    specular *= light.specularStrength * light.color;

    // Toplam
    vec3 result = (ambient + (diffuse + specular) * attenuation * spotEffect * light.intensity);
    return result;
}

void main() {
    // Texture değerlerini al
    vec4 diffuseColor = material.hasDiffuseMap ? texture(diffuseMap, TexCoords) : vec4(material.diffuse, 1.0);
    if (specialTextureMode) {
        diffuseColor = texture(specialTexture, TexCoords);
    }
    
    // Normal map'ten normal vektörü al - Vertex shader'dan gelen TBN matrisini kullan
    vec3 normal = normalize(Normal);
    if (material.hasNormalMap) {
        // Vertex shader'dan gelen optimize edilmiş TBN matrisini kullan
        normal = normalize(TBN * (texture(normalMap, TexCoords).rgb * 2.0 - 1.0));
    }

    // Eğer texture yoksa varsayılan değerleri kullan
    // BU kısım normalde obkjelerin textıre olmadan dahai yüklemesi için eklndi
    float roughness = 0.5;
    float metallic = 0.0;
    if (material.hasRoughnessMap) {
        roughness = texture(roughnessMap, TexCoords).r;
    }
    if (material.hasMetallicMap) {
        metallic = texture(metallicMap, TexCoords).r;
    }

    // View vektörü
    vec3 viewDir = normalize(viewPos - FragPos);

    // Temel renk
    vec3 albedo = diffuseColor.rgb * material.brightness;
    
    // Vertex renkleri kullanılıyorsa - Vertex shader'dan gelen VertexColor'ı kullan
    if (material.useVertexColors) {
        albedo = VertexColor * material.brightness;
    }
    
    // Işık hesaplaması
    vec3 result = calculateLight(normal, viewDir, albedo, roughness, metallic);

    // Parlaklık kontrolü 
    result *= min(material.brightness, 5.0); // Maksimum 5x parlaklık

    
    result = result / (result + vec3(1.0));
    result = pow(result, vec3(1.0/2.2));

    float finalOpacity = material.opacity * diffuseColor.a;
    FragColor = vec4(result, finalOpacity);
}