#include "Application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

// settings
constexpr uint32_t SCR_WIDTH{ 800 };
constexpr uint32_t SCR_HEIGHT{ 600 };

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse{ true };
float yaw{ -90.0f };	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch{ 0.0f };
float lastX{ 800.0f / 2.0 };
float lastY{ 600.0 / 2.0 };
float fov{ 45.0f };

// timing
float deltaTime{ 0.0f };	// time between current frame and last frame
float lastFrame{ 0.0f };

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

namespace oak
{
    Application::Application(const std::string& name)
    {
        m_EventQueue = std::make_shared<EventQueue>();
        m_Window.createWindow(name, 800, 600, m_EventQueue);
    }

    void Application::init()
    {
        m_EventQueue->registerEventListener(EventType::MouseMoved, [this]([[maybe_unused]] const EventPointer& event) {

            auto mouseMovedEvent = static_cast<const MouseMovedEvent*>(event.get());
            auto xPos = mouseMovedEvent->getPositionX();
            auto yPos = mouseMovedEvent->getPositionY();

            if (firstMouse)
            {
                lastX = xPos;
                lastY = yPos;
                firstMouse = false;
            }

            auto xoffset = xPos - lastX;
            auto yoffset = lastY - yPos; // reversed since y-coordinates go from bottom to top
            lastX = xPos;
            lastY = yPos;

            auto sensitivity{ 0.1f };
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            yaw += xoffset;
            pitch += yoffset;

            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            glm::vec3 front{};
            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(front);
        });

        m_EventQueue->registerEventListener(EventType::MouseScrolled, [this]([[maybe_unused]] const EventPointer& event) {

            auto mouseScrolledEvent = static_cast<const MouseScrolledEvent*>(event.get());
            auto yoffset = mouseScrolledEvent->getOffsetY();

            fov -= (float)yoffset;
            if (fov < 1.0f)
                fov = 1.0f;
            if (fov > 45.0f)
                fov = 45.0f;
            });

        m_EventQueue->registerEventListener(EventType::WindowClose, [this]([[maybe_unused]] const EventPointer& event) {
            m_Running = false;
        });

        m_EventQueue->registerEventListener(EventType::WindowResize, [](const EventPointer& event) {
            auto windowResizeEvent = static_cast<const  WindowResizeEvent*>(event.get());
            glViewport(0, 0, windowResizeEvent->getWidth(), windowResizeEvent->getHeight());
        });

        //Glad: Load all OpenGL function pointers
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
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

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
            // per-frame time logic
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            //Render here.
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // be sure to activate shader when setting uniforms/drawing objects
            m_LightingShader.use();
            m_LightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
            m_LightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

            // view/projection transformations
            glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            m_LightingShader.setMat4("projection", projection);
            m_LightingShader.setMat4("view", view);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f);
            m_LightingShader.setMat4("model", model);

            // render the cube
            glBindVertexArray(m_CubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            // also draw the lamp object
            m_LightCubeShader.use();
            m_LightCubeShader.setMat4("projection", projection);
            m_LightCubeShader.setMat4("view", view);
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
