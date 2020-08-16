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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------------------------
void Texture2D::Load(const std::string& filename) {
    // (путь, ширина, высота, количество каналов при загрузке изображения, количество каналов для отображения)
	// каналы STBI_grey = 1, STBI_grey_alpha = 2, STBI_rgb = 3, STBI_rgb_alpha = 4
	image = stbi_load(filename.c_str(), &width, &height, &cnt, 3); // загружаем текстуру
    if (image) {
        Init();
    }
    else {
        std::cout << "2D texture failed to load at path: " << filename << std::endl;
        //stbi_image_free(image);
    }
    //Init();
}

void Texture2D::Init() {
    Bind();
    // https://stackoverflow.com/questions/7380773/glteximage2d-segfault-related-to-width-height
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    // (текстурная цель, уровень мипмапа, формат текстуры, ширина, высота, 0, формат исходного изображения,
	//  тип данных исходного изображения, данные изображения)
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image)); // генерируем текстуру
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//GLCall(glGenerateMipmap(GL_TEXTURE_2D)); // генерация всех необходимых мипмапов для текстуры
	stbi_image_free(image); // освобождения памяти
    Unbind(); // отвязка объекта текстуры
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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