#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"

Texture::Texture() {
    GLCall(glGenTextures(1, &descriptor)); // 1 - количество текстур для генерации
}

void Texture::Bind() const {
    GLCall(glBindTexture(GL_TEXTURE_2D, descriptor)); // привязка текстуры
}

void Texture::Bind(GLenum slot) const {
    GLCall(glActiveTexture(slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, descriptor)); // привязка текстуры
}

void Texture::Unbind() const { 
    GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // отвязка объекта текстуры
}

Texture::~Texture() {
    GLCall(glDeleteTextures(1, &descriptor));
}

//------------------------------------------------------------------------------------------------------
void RenderTexture:: CreateAttachment(GLenum slot) const {
    // присоединение текстуры к объекту текущего кадрового буфера
    // (тип объекта кадра, тип прикрепления, тип текстуры, объект текстуры, используемый для вывода МИП-уровень)
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, slot, GL_TEXTURE_2D, descriptor, 0)); 
}

void RenderTexture::Init(int width, int height, GLuint internalformat, GLenum format, GLenum type, GLint param) {
    Bind();
    this->width = width;
    this->height = height;
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, NULL));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param));
    Unbind();
}