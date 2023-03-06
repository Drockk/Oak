#include "Application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

// settings
constexpr uint32_t SCR_WIDTH{ 800 };
constexpr uint32_t SCR_HEIGHT{ 600 };

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

namespace oak
{
    Application::Application(const std::string& name)
    {
        m_Camera = Camera({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, -90.0f, 0.0f, SCR_WIDTH, SCR_HEIGHT);
        m_EventQueue = std::make_shared<EventQueue>();
        m_Window.createWindow(name, SCR_WIDTH, SCR_HEIGHT, m_EventQueue);
    }

    void Application::init()
    {
        m_EventQueue->registerEventListener(EventType::WindowClose, [this]([[maybe_unused]] const EventPointer& event) {
            m_Running = false;
        });

        m_EventQueue->registerEventListener(EventType::WindowResize, [](const EventPointer& event) {
            auto windowResizeEvent = static_cast<const  WindowResizeEvent*>(event.get());
            glViewport(0, 0, windowResizeEvent->getWidth(), windowResizeEvent->getHeight());
        });

        m_Camera.registerEvents(m_EventQueue);

        auto version = gladLoadGL(glfwGetProcAddress);
        if (version == 0)
        {
            std::cerr << "Failed to initialize OpenGL context" << std::endl;
        }

        std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

        glEnable(GL_DEPTH_TEST);

        //Create shader.
        m_LightingShader.loadFromFile("resources/shaders/colors.vs", "resources/shaders/colors.fs");
        m_LightCubeShader.loadFromFile("resources/shaders/lightCube.vs", "resources/shaders/lightCube.fs");

        float vertices[] = {
            -0.5f,  -0.5f, -0.5f,
             0.5f,  -0.5f, -0.5f,
             0.5f,   0.5f, -0.5f,
             0.5f,   0.5f, -0.5f,
            -0.5f,   0.5f, -0.5f,
            -0.5f,  -0.5f, -0.5f,

            -0.5f,  -0.5f,  0.5f,
             0.5f,  -0.5f,  0.5f,
             0.5f,   0.5f,  0.5f,
             0.5f,   0.5f,  0.5f,
            -0.5f,   0.5f,  0.5f,
            -0.5f,  -0.5f,  0.5f,

            -0.5f,   0.5f,  0.5f,
            -0.5f,   0.5f, -0.5f,
            -0.5f,  -0.5f, -0.5f,
            -0.5f,  -0.5f, -0.5f,
            -0.5f,  -0.5f,  0.5f,
            -0.5f,   0.5f,  0.5f,

             0.5f,   0.5f,  0.5f,
             0.5f,   0.5f, -0.5f,
             0.5f,  -0.5f, -0.5f,
             0.5f,  -0.5f, -0.5f,
             0.5f,  -0.5f,  0.5f,
             0.5f,   0.5f,  0.5f,

            -0.5f,  -0.5f, -0.5f,
             0.5f,  -0.5f, -0.5f,
             0.5f,  -0.5f,  0.5f,
             0.5f,  -0.5f,  0.5f,
            -0.5f,  -0.5f,  0.5f,
            -0.5f,  -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
        };

        glGenVertexArrays(1, &m_CubeVAO);
        glGenBuffers(1, &m_VBO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(m_CubeVAO);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
        glGenVertexArrays(1, &m_LightCubeVAO);
        glBindVertexArray(m_LightCubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    void Application::run()
    {
        glm::vec3 cubePositions[] = {
            glm::vec3(0.0f,  0.0f,  0.0f),
            glm::vec3(2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f,  2.0f, -2.5f),
            glm::vec3(1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };

        while (m_Running)
        {
            m_Camera.onUpdate();

            //Render here.
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // be sure to activate shader when setting uniforms/drawing objects
            m_LightingShader.use();
            m_LightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
            m_LightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

            m_LightingShader.setMat4("projection", m_Camera.getProjection());
            m_LightingShader.setMat4("view", m_Camera.getView());

            // world transformation
            glm::mat4 model = glm::mat4(1.0f);
            m_LightingShader.setMat4("model", model);

            // render the cube
            glBindVertexArray(m_CubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            // also draw the lamp object
            m_LightCubeShader.use();
            m_LightCubeShader.setMat4("projection", m_Camera.getProjection());
            m_LightCubeShader.setMat4("view", m_Camera.getView());
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
            m_LightCubeShader.setMat4("model", model);

            glBindVertexArray(m_LightCubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            m_Window.onUpdate();
            m_EventQueue->proccess();
        }
    }

    void Application::shutdown()
    {
        glDeleteVertexArrays(1, &m_CubeVAO);
        glDeleteVertexArrays(1, &m_LightCubeVAO);
        glDeleteBuffers(1, &m_VBO);

        m_Window.onShutdown();
    }
}
