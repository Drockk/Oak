#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Window.hpp"

int main()
{
    oak::Window test;
    test.createWindow("Test", 800, 600);

    //Glag: Load all OpenGL function pointers
    auto version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
    }

    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    glViewport(0, 0, 800, 600);

    //Loop until the user closes the window.
    while (true)
    {
        //Render here.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        test.onUpdate();
    }

    test.onShutdown();
    return 0;
}
