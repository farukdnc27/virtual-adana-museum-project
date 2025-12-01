#include "Texture.h"
#include <iostream>
#include <fstream>
#include <glad/glad.h>

// stb_image.h'yi sadece bir kez tanımla
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "../../Linking/include/stb_image.h"

#endif

Texture::Texture()
    : m_RendererID(0), m_FilePath(""), m_LocalBuffer(nullptr),
    m_Width(0), m_Height(0), m_BPP(0) {
}

Texture::Texture(const std::string& path)
    : m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
    m_Width(0), m_Height(0), m_BPP(0)
{
    stbi_set_flip_vertically_on_load(1);
    
    // Önce dosya varlığını kontrol et
    std::ifstream file(path);
    if (!file.good()) {
        std::cerr << "HATA: Texture dosyası bulunamadı: " << path << std::endl;
        return;
    }
    file.close();

    // Texture'ı yükle
    int width, height, channels;
    unsigned char* buffer = stbi_load(path.c_str(), &width, &height, &channels, 0);
    
    if (!buffer) {
        std::cerr << "HATA: stbi_load başarısız oldu: " << stbi_failure_reason() << std::endl;
        return;
    }

    // Değerleri sınıf üyelerine ata
    m_Width = width;
    m_Height = height;
    m_BPP = channels;
    m_LocalBuffer = buffer;

    // Texture boyutlarını kontrol et
    if (m_Width <= 0 || m_Height <= 0) {
        std::cerr << "HATA: Geçersiz texture boyutları: " << m_Width << "x" << m_Height << std::endl;
        stbi_image_free(m_LocalBuffer);
        m_LocalBuffer = nullptr;
        return;
    }

    // Format ve internal format belirle
    GLenum format = GL_RGB;
    GLenum internalFormat = GL_RGB8;
    
    switch (m_BPP) {
        case 1:
            format = GL_RED;
            internalFormat = GL_R8;
            break;
        case 3:
            format = GL_RGB;
            internalFormat = GL_RGB8;
            break;
        case 4:
            format = GL_RGBA;
            internalFormat = GL_RGBA8;
            break;
        default:
            std::cerr << "HATA: Desteklenmeyen BPP değeri: " << m_BPP << std::endl;
            stbi_image_free(m_LocalBuffer);
            m_LocalBuffer = nullptr;
            return;
    }

    // OpenGL texture oluştur
    glGenTextures(1, &m_RendererID);
    if (m_RendererID == 0) {
        std::cerr << "HATA: glGenTextures başarısız oldu" << std::endl;
        stbi_image_free(m_LocalBuffer);
        m_LocalBuffer = nullptr;
        return;
    }

    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    // Texture parametrelerini ayarla
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Texture verilerini yükle
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 1 byte hizalama
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_LocalBuffer);
    
    // OpenGL hata kontrolü
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "HATA: glTexImage2D başarısız oldu. Hata kodu: " << err << std::endl;
        glDeleteTextures(1, &m_RendererID);
        stbi_image_free(m_LocalBuffer);
        m_LocalBuffer = nullptr;
        m_RendererID = 0;
        return;
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Belleği temizle
    stbi_image_free(m_LocalBuffer);
    m_LocalBuffer = nullptr;
}

Texture::Texture(const Texture& other)
    : Texture(other.m_FilePath) // yeniden yükleme yapılır
{
}

Texture& Texture::operator=(const Texture& other)
{
    if (this != &other) {
        if (m_RendererID != 0)
            glDeleteTextures(1, &m_RendererID);

        *this = Texture(other.m_FilePath); // yeniden yükleme
    }
    return *this;
}

Texture::Texture(Texture&& other) noexcept
    : m_RendererID(other.m_RendererID), m_FilePath(std::move(other.m_FilePath)),
    m_LocalBuffer(other.m_LocalBuffer), m_Width(other.m_Width),
    m_Height(other.m_Height), m_BPP(other.m_BPP)
{
    other.m_RendererID = 0;
    other.m_LocalBuffer = nullptr;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other) {
        if (m_RendererID != 0)
            glDeleteTextures(1, &m_RendererID);

        m_RendererID = other.m_RendererID;
        m_FilePath = std::move(other.m_FilePath);
        m_LocalBuffer = other.m_LocalBuffer;
        m_Width = other.m_Width;
        m_Height = other.m_Height;
        m_BPP = other.m_BPP;

        other.m_RendererID = 0;
        other.m_LocalBuffer = nullptr;
    }
    return *this;
}

Texture::~Texture()
{
    if (m_RendererID != 0)
        glDeleteTextures(1, &m_RendererID);
    if (m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
}

void Texture::Bind(unsigned int slot) const
{
    if (m_RendererID != 0) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture::loadFromFile(const std::string& path)
{
    m_FilePath = path;

    std::ifstream file(path);
    if (!file.good()) {
        std::cerr << "HATA: Texture dosyası bulunamadı: " << path << std::endl;
        return false;
    }
    file.close();

    m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 0);
    if (!m_LocalBuffer) {
        std::cerr << "HATA: stbi_load başarısız: " << path << std::endl;
        return false;
    }

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    GLenum format = GL_RGB;
    if (m_BPP == 1) format = GL_RED;
    else if (m_BPP == 3) format = GL_RGB;
    else if (m_BPP == 4) format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_LocalBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(m_LocalBuffer);
    m_LocalBuffer = nullptr;

    return true;
}

