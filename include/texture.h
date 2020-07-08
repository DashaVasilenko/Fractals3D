#ifndef __TEXTURE__
#define __TEXTURE__

#include <iostream>
#include <string>
#include <array>
#include <GL/glew.h>
#include "stb_image.h"
#include "errors.h"

class Texture {
public:
    Texture();
    void Bind() const;
    void Bind(GLenum slot) const;
    GLuint GetDescriptor() { return descriptor; }
    void Unbind() const;
    ~Texture();

protected:
    GLuint descriptor;
    unsigned char* image;
    int width;
    int height;
    int cnt;
};

class RenderTexture : public Texture {
public:
    void CreateAttachment(GLenum slot) const;
    void Init(int width, int height, GLuint internalformat, GLenum format, GLenum type, GLint param);
};

#endif /* End of __TEXTURE__ */