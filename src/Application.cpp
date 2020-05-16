#include<glew.h>
#include<glfw3.h>

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"


struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

// Parses the shaders from the .shader file
static ShaderProgramSource ParseShader(const std::string& filepath)
{
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;


    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                // Set node to vertex
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                // Set node to fragment
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

// Compiles the shader for openGL
static unsigned int  CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char)); // Some kind of weird work around here
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

// Creates and calls compile for the shader
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

// Draws an aqua colored triangle on the screen using the shaders in Basic.shader
static unsigned int DrawTriangle()
{
    // std::cout << glGetString(GL_VERSION) << std::endl; // This is how you can view the current version

    // This section is just setting gl up with the data
    float positions[6] = { -0.5f, -0.5f,
                            0.0f,  0.5f,
                            0.5f, -0.5f }; // Creating a series of positions
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // This is to enable the following command
    // index = 0 => first attribute
    // size = 2 => first position has 2 components
    // type = GL_FLOAT => These points are floats
    // normalized = False => They're already floats, we don't need them normalized 
    // stride = sizeof(float) * 2 => the step required to get to the next vertex
    // ptr = 0 => there's no further definition to be had in this case
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); // This is a single attribute
    // There's a difference between attributes and vertices here. The positions described in the 
    // positions array are three vertices, they are being described by the above single attribute.

    // The shader => Uses the functions CreateShader and CompileShader defined above.
    // Writing shaders in cpp files looks like this and it's clearly horrendous so 
    // the shaders were moved to a .shader file and parsed in using the ParseShader function.
    /*std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;"
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n;";

    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n;";*/

        // Bring in the shaders and hand them off to openGL
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    return shader;
    // Print out the shaders 
    // std::cout << "VERTEX: " << std::endl;
    // std::cout << source.VertexSource << std::endl;
    // std::cout << "FRAGMENT: " << std::endl;
    // std::cout << source.FragmentSource << std::endl;
}

static unsigned int DrawSquare()
{
    float positions[12] = { -0.5f, -0.5f,
                             0.5f, -0.5f,
                             0.5f,  0.5f,

                             0.5f,  0.5f,
                            -0.5f,  0.5f,
                            -0.5f, -0.5f
    }; // Creating a series of positions
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);
    return shader;

    // Above draws the square using a vertex buffer which, in this case is storing the same coordinates
    // twice for 2 of the vertices. The square really has four relevant vertices but in order to define
    // it as 2 triangles we're storing the first and last vertices of the first triangle twice. This 
    // is problematic when we get to larger designs because vertices may became large and we don't 
    // want to consume all of our GPU memory being redundant. To avoid the problem we can use an index buffer
    // and refer to the index when we need the same vertices again. The function DrawEfficientSquare() 
    // implements the index buffer.
}

static unsigned int DrawEfficientSquare()
{
    float positions[] = { -0.5f, -0.5f,
                           0.5f, -0.5f,
                           0.5f,  0.5f,
                          -0.5f,  0.5f
                        }; // Creating a series of positions

    // This is the index buffer
    unsigned int indices[] = {
                                0, 1, 2, // First triangle
                                2, 3, 0  // Second triangle
                             };

    VertexBuffer vb(positions, 4 * 2 * sizeof(float));

    
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));
    
    IndexBuffer ib(indices, 6);
    
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader));

    // This is how we reference a variable in the shader
    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);
    GLCall(glUniform4f(location, 0.2, 0.4, 0.4, 1.0));

    return shader;
}
// ***************************************************************************************************************************************//
                                                                // ENTRY POINT // 
// ***************************************************************************************************************************************//
int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 960, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); // Modify the frame rate

    // ^^This has to happen after a valid context is created^^
    // If you're linking GLEW statically you need to go to the sln properties 
    // and add GLEW_STATIC under C++ preprocessor definitions
    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl; // Print current version
    {
        // Create vertices
        float positions[] = { -0.5f, -0.5f,
                               0.5f, -0.5f,
                               0.5f,  0.5f,
                              -0.5f,  0.5f
        };

        // This is the index buffer
        unsigned int indices[] = {
                                    0, 1, 2, // First triangle
                                    2, 3, 0  // Second triangle
        };
        IndexBuffer ib(indices, 6);

        // Create vertex array object
        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        // Create Vertex Buffer, push layout, 
        VertexArray va;
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        // Create the shaders and bind
        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        GLCall(glUseProgram(shader));

        // This is how we reference a variable in the shader
        GLCall(int location = glGetUniformLocation(shader, "u_Color"));
        ASSERT(location != -1);
        GLCall(glUniform4f(location, 0.2, 0.4, 0.4, 1.0));

        GLCall(glBindVertexArray(0));
        GLCall(glUseProgram(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        // **************************** Color Adjust ****************************
        float r = 0.2;
        float increment = 0.05;


        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            GLCall(glUseProgram(shader));
            GLCall(glUniform4f(location, r, 0.4, 0.4, 1.0));

            va.Bind();
            ib.Bind();
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            // Vary the color
            if (r > 1.0f)
                increment = -0.05;
            else if (r < 0.0f)
                increment = 0.05;
            r += increment;

            // This is the dated method of drawing
            /*glBegin(GL_TRIANGLES);
            glVertex2f(-0.5f, -0.5f);
            glVertex2f( 0.0f, 0.5f );
            glVertex2f( 0.5f, -0.5f);
            glEnd();*/

            // GLClearError();  // This is no longer needed because of the GLCall Macro

            // DRAWING TRIANGLE :
            // glDrawArrays(GL_TRIANGLES, 0, 3); // Without an index buffer
            // glDrawElements(GL_TRIANGLES, 3); // With an index buffer 

            // DRAWING INEFFICIENT SQUARE :
            // glDrawArrays(GL_TRIANGLES, 0, 6); // Without an index buffer


            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        glDeleteProgram(shader); // Destroy the stuff on exit
    }
    glfwTerminate();
    return 0;