#ifndef __FRAMEBUFFER__
#define __FRAMEBUFFER__

#include <GL/glew.h>
#include "errors.h"
#include "texture.h"

class FrameBuffer {
public:
    FrameBuffer();
    void BufferInit(int width, int  height);
    void Bind() const;
    void Resize(int width, int height);
    void Unbind() const;

    GLuint GetDescriptor() { return descriptor; }
    GLuint GetTexDescriptor() { return tex_color_buf.GetDescriptor(); }
    int GetWidth() { return width; }
    int GetHeight() { return height; }

    void SetWidth(int w) { width = w; }
    void SetHeight(int h) { height = h; }
    
    ~FrameBuffer();
    
protected:
    GLuint descriptor;
    
private:
    int width;
    int height;
    RenderTexture tex_color_buf;
    unsigned int depth;
};

#endif /* End of __FRAMEBUFFER__ */