#include"Renderer.h"
#include<iostream>

// This routine just removes any existing error codes GL might have
// It should be called just before a function call to OpenGL
void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

// This routine prints the errors. Assuming the errors were cleared before the 
// the function call, this shoud return any errors a function produced.
// GL just gives you a number back and you can find what that number means by
// converting it to hexadecimal and searching the GL header file which in this case is glew.h
bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ": " << line << std::endl;
        return false;
    }
    return true;
}