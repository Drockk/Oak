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
        m_Shader.loadFromFile("resources/shaders/shader.vs", "resources/shaders/shader.fs");

        float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        m_Texture1.loadFromFile("resources/textures/container.jpg");
        m_Texture2.loadFromFile("resources/textures/awesomeface.png");

        // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
        m_Shader.use();
        m_Shader.setInt("texture1", 0);
        m_Shader.setInt("texture2", 1);
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

            glActiveTexture(GL_TEXTURE0);
            m_Texture1.bindTexture();
            glActiveTexture(GL_TEXTURE1);
            m_Texture2.bindTexture();

            m_Shader.use();

            // pass projection matrix to shader (note that in this case it could change every frame)
            glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            m_Shader.setMat4("projection", projection);

            // camera/view transformation
            glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            m_Shader.setMat4("view", view);

            // render boxes
            glBindVertexArray(m_VAO);
            for (unsigned int i = 0; i < 10; i++)
            {
                // calculate the model matrix for each object and pass it to shader before drawing
                glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
                model = glm::translate(model, cubePositions[i]);
                float angle = 20.0f * i;
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                m_Shader.setMat4("model", model);

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            m_Window.onUpdate();
            m_EventQueue->proccess();
        }
    }

    void Application::shutdown()
    {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);

        m_Window.onShutdown();
    }
}
