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

/*
#define assert(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    assert(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
	while(GLenum error = glGetError()) {
		std::cout << "[OpenGL_Error] (" << error << "):" << function << " " << file << ":" << line << std::endl;
        return false;
	}
    return true;
}
*/

#endif /* End of __ERRORS__ */