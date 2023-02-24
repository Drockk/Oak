#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Window.hpp"

int main()
{
    oak::Window test;
    test.createWindow("Test", 800, 600);

    GLFWwindow* window;

    //Initialize the library
    //if (!glfwInit())
    //{
    //    return -1;
    //}

    //Create a windowed mode window and its OpenGL context.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(800, 600, "Oak Engine", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    //Make the window's context current.
    glfwMakeContextCurrent(window);

    //Glag: Load all OpenGL function pointers
    auto version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
    }

    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    glViewport(0, 0, 800, 600);

    //Loop until the user closes the window.
    while (!glfwWindowShouldClose(window))
    {
        //Render here.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Swap front and back buffers.
        glfwSwapBuffers(window);

        //Poll for and process events.
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
