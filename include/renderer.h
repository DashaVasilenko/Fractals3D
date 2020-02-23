#ifndef __RENDERER__
#define __RENDERER__

#include "window.h"

class Renderer {
public:
    void SetWidth(int width) { this->width = width; }
    void SetHeight(int height) { this->height = height; }
    void Init();
    void Update();
private:
    int width;
    int height;
};

#endif /* End of __RENDERER__ */ 