#pragma once

#include "Oak/Renderer/OrthographicCamera.hpp"

#include "Oak/Renderer/Texture.hpp"

#include "Oak/Renderer/Camera.hpp"
#include "Oak/Renderer/EditorCamera.hpp"
#include "Oak/Renderer/Font.hpp"

#include "Oak/Scene/Components.hpp"

namespace oak {
    class Renderer2D
    {
    public:
        static void init();
        static void shutdown();

        static void beginScene(const Camera& camera, const glm::mat4& transform);
        static void beginScene(const EditorCamera& camera);
        static void beginScene(const OrthographicCamera& camera); // TODO: Remove
        static void endScene();
        static void flush();

        // Primitives
        static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
        static void drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
        static void drawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
        static void drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

        static void drawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
        static void drawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), int entityID = -1);

        static void drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
        static void drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
        static void drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
        static void drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

        static void drawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);
        
        static void drawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID = -1);

        static void drawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
        static void drawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

        static void drawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID);

        struct TextParams
        {
            glm::vec4 color{ 1.0f };
            float kerning = 0.0f;
            float lineSpacing = 0.0f;
        };
        static void drawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID = -1);
        static void drawString(const std::string& string, const glm::mat4& transform, const TextComponent& component, int entityID = -1);

        static float getLineWidth();
        static void setLineWidth(float width);

        // Stats
        struct Statistics
        {
            uint32_t drawCalls = 0;
            uint32_t quadCount = 0;

            uint32_t getTotalVertexCount() const { return quadCount * 4; }
            uint32_t getTotalIndexCount() const { return quadCount * 6; }
        };
        static void resetStats();
        static Statistics getStats();

    private:
        static void startBatch();
        static void nextBatch();
    };
}
