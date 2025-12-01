#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <cstring>
#include <iostream>

class Texture {
public:
    Texture();
    Texture(const std::string& path);
    Texture(const Texture& other);
    Texture& operator=(const Texture& other);
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

	bool loadFromFile(const std::string& path);

    // Bellekten texture yükleme
    void loadFromMemory(const void* data, unsigned int width, unsigned int height, const char* formatHint) {
        if (m_RendererID != 0) {
            glDeleteTextures(1, &m_RendererID);
        }

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        // Texture parametrelerini ayarla
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Format kontrolü
        GLenum format = GL_RGB;
        if (strcmp(formatHint, "rgba8888") == 0) {
            format = GL_RGBA;
        }
        else if (strcmp(formatHint, "rgb888") == 0) {
            format = GL_RGB;
        }
        else {
            std::cerr << "Desteklenmeyen texture formatı: " << formatHint << std::endl;
            return;
        }

        // Texture'ı yükle
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        m_Width = width;
        m_Height = height;
        m_BPP = (format == GL_RGBA) ? 4 : 3;

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Wrap mode ayarları
    void SetWrapMode(int wrapS, int wrapT) {
        Bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
        Unbind();
    }

private:
    unsigned int m_RendererID;
    std::string m_FilePath;
    unsigned char* m_LocalBuffer;
    int m_Width, m_Height, m_BPP;
};
