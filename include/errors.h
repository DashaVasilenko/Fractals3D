#ifndef __ERRORS__
#define __ERRORS__

#include <iostream>
#include <string>
#include <cassert>

#define GLCall(x) GLClearError();\
    x;\
    assert(GLLogCall());

static inline void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

static inline bool GLLogCall() {
	while(GLenum error = glGetError()) {
		std::cout << "[OpenGL_Error] (" << error << ")" << std::endl;
        return false;
	}
    return true;
}

#endif /* End of __ERRORS__ */