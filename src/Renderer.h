#pragma once
#include<glew.h>
#include<string>


// These #define's are called Macros. The way that they work is basically you're 
// replacing the defined symbol with the code inside the definition. This happens 
// during preprocessing before the code gets sent to the compiler so essentially '
// the compiler sees it as if you've written those lines instead of all the places you wrote 
// ASSERT or GLCall
// This is a c++ feature coupled with the Microsoft call to break in the ide,
// If we weren't using Visual Studio, __debugbreak() would be dependent on the compiler.
// __debugbreak is called a "Compiler Intrinsic"
#define ASSERT(x) if (!(x)) __debugbreak(); 

// #x turns x into a string
// __FILE__ & __LINE__ are not intrinsics and should be supported by all compilers
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))




void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);