#include "oakpch.hpp"
#include "Oak/Renderer/Renderer2D.hpp"

#include "Oak/Renderer/VertexArray.hpp"
#include "Oak/Renderer/Shader.hpp"
#include "Oak/Renderer/UniformBuffer.hpp"
#include "Oak/Renderer/RenderCommand.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MSDFData.hpp"

namespace oak
{
    struct QuadVertex
    {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texCoord;
        float texIndex;
        float tilingFactor;

        // Editor-only
        int entityID;
    };

    struct CircleVertex
    {
        glm::vec3 worldPosition;
        glm::vec3 localPosition;
        glm::vec4 color;
        float thickness;
        float fade;

        // Editor-only
        int entityID;
    };

    struct LineVertex
    {
        glm::vec3 position;
        glm::vec4 color;

        // Editor-only
        int entityID;
    };

    struct TextVertex
    {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texCoord;

        // TODO: bg color for outline/bg

        // Editor-only
        int entityID;
    };

    struct Renderer2DData
    {
        static const uint32_t maxQuads = 20000;
        static const uint32_t maxVertices = maxQuads * 4;
        static const uint32_t maxIndices = maxQuads * 6;
        static const uint32_t maxTextureSlots = 32; // TODO: RenderCaps

        Ref<VertexArray> quadVertexArray;
        Ref<VertexBuffer> quadVertexBuffer;
        Ref<Shader> quadShader;
        Ref<Texture2D> whiteTexture;

        Ref<VertexArray> circleVertexArray;
        Ref<VertexBuffer> circleVertexBuffer;
        Ref<Shader> circleShader;

        Ref<VertexArray> lineVertexArray;
        Ref<VertexBuffer> lineVertexBuffer;
        Ref<Shader> lineShader;

        Ref<VertexArray> textVertexArray;
        Ref<VertexBuffer> textVertexBuffer;
        Ref<Shader> textShader;

        uint32_t quadIndexCount = 0;
        QuadVertex* quadVertexBufferBase = nullptr;
        QuadVertex* quadVertexBufferPtr = nullptr;

        uint32_t circleIndexCount = 0;
        CircleVertex* circleVertexBufferBase = nullptr;
        CircleVertex* circleVertexBufferPtr = nullptr;

        uint32_t lineVertexCount = 0;
        LineVertex* lineVertexBufferBase = nullptr;
        LineVertex* lineVertexBufferPtr = nullptr;

        uint32_t textIndexCount = 0;
        TextVertex* textVertexBufferBase = nullptr;
        TextVertex* textVertexBufferPtr = nullptr;

        float lineWidth = 2.0f;

        std::array<Ref<Texture2D>, maxTextureSlots> textureSlots;
        uint32_t textureSlotIndex = 1; // 0 = white texture

        Ref<Texture2D> fontAtlasTexture;

        glm::vec4 quadVertexPositions[4];

        Renderer2D::Statistics stats;

        struct CameraData
        {
            glm::mat4 viewProjection;
        };
        CameraData cameraBuffer;
        Ref<UniformBuffer> cameraUniformBuffer;
    };

    static Renderer2DData s_Data;

    void Renderer2D::init()
    {
        OAK_PROFILE_FUNCTION();

        s_Data.quadVertexArray = VertexArray::create();

        s_Data.quadVertexBuffer = VertexBuffer::create(s_Data.maxVertices * sizeof(QuadVertex));
        s_Data.quadVertexBuffer->setLayout({
            { ShaderDataType::Float3, "a_Position"     },
            { ShaderDataType::Float4, "a_Color"        },
            { ShaderDataType::Float2, "a_TexCoord"     },
            { ShaderDataType::Float,  "a_TexIndex"     },
            { ShaderDataType::Float,  "a_TilingFactor" },
            { ShaderDataType::Int,    "a_EntityID"     }
            });
        s_Data.quadVertexArray->addVertexBuffer(s_Data.quadVertexBuffer);

        s_Data.quadVertexBufferBase = new QuadVertex[s_Data.maxVertices];

        uint32_t* quadIndices = new uint32_t[s_Data.maxIndices];

        uint32_t offset = 0;
        for (uint32_t i = 0; i < s_Data.maxIndices; i += 6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }

        Ref<IndexBuffer> quadIB = IndexBuffer::create(quadIndices, s_Data.maxIndices);
        s_Data.quadVertexArray->setIndexBuffer(quadIB);
        delete[] quadIndices;

        // Circles
        s_Data.circleVertexArray = VertexArray::create();

        s_Data.circleVertexBuffer = VertexBuffer::create(s_Data.maxVertices * sizeof(CircleVertex));
        s_Data.circleVertexBuffer->setLayout({
            { ShaderDataType::Float3, "a_WorldPosition" },
            { ShaderDataType::Float3, "a_LocalPosition" },
            { ShaderDataType::Float4, "a_Color"         },
            { ShaderDataType::Float,  "a_Thickness"     },
            { ShaderDataType::Float,  "a_Fade"          },
            { ShaderDataType::Int,    "a_EntityID"      }
            });
        s_Data.circleVertexArray->addVertexBuffer(s_Data.circleVertexBuffer);
        s_Data.circleVertexArray->setIndexBuffer(quadIB); // Use quad IB
        s_Data.circleVertexBufferBase = new CircleVertex[s_Data.maxVertices];

        // Lines
        s_Data.lineVertexArray = VertexArray::create();

        s_Data.lineVertexBuffer = VertexBuffer::create(s_Data.maxVertices * sizeof(LineVertex));
        s_Data.lineVertexBuffer->setLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color"    },
            { ShaderDataType::Int,    "a_EntityID" }
            });
        s_Data.lineVertexArray->addVertexBuffer(s_Data.lineVertexBuffer);
        s_Data.lineVertexBufferBase = new LineVertex[s_Data.maxVertices];

        // Text
        s_Data.textVertexArray = VertexArray::create();

        s_Data.textVertexBuffer = VertexBuffer::create(s_Data.maxVertices * sizeof(TextVertex));
        s_Data.textVertexBuffer->setLayout({
            { ShaderDataType::Float3, "a_Position"     },
            { ShaderDataType::Float4, "a_Color"        },
            { ShaderDataType::Float2, "a_TexCoord"     },
            { ShaderDataType::Int,    "a_EntityID"     }
            });
        s_Data.textVertexArray->addVertexBuffer(s_Data.textVertexBuffer);
        s_Data.textVertexArray->setIndexBuffer(quadIB);
        s_Data.textVertexBufferBase = new TextVertex[s_Data.maxVertices];

        s_Data.whiteTexture = Texture2D::create(TextureSpecification());
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

        int32_t samplers[s_Data.maxTextureSlots];
        for (uint32_t i = 0; i < s_Data.maxTextureSlots; i++)
            samplers[i] = i;

        s_Data.quadShader = Shader::create("assets/shaders/Renderer2D_Quad.glsl");
        s_Data.circleShader = Shader::create("assets/shaders/Renderer2D_Circle.glsl");
        s_Data.lineShader = Shader::create("assets/shaders/Renderer2D_Line.glsl");
        s_Data.textShader = Shader::create("assets/shaders/Renderer2D_Text.glsl");

        // Set first texture slot to 0
        s_Data.textureSlots[0] = s_Data.whiteTexture;

        s_Data.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        s_Data.quadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
        s_Data.quadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
        s_Data.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

        s_Data.cameraUniformBuffer = UniformBuffer::create(sizeof(Renderer2DData::CameraData), 0);
    }

    void Renderer2D::shutdown()
    {
        OAK_PROFILE_FUNCTION();

        delete[] s_Data.quadVertexBufferBase;
    }

    void Renderer2D::beginScene(const OrthographicCamera& camera)
    {
        OAK_PROFILE_FUNCTION();

        s_Data.cameraBuffer.viewProjection = camera.getViewProjectionMatrix();
        s_Data.cameraUniformBuffer->setData(&s_Data.cameraBuffer, sizeof(Renderer2DData::CameraData));

        startBatch();
    }

    void Renderer2D::beginScene(const Camera& camera, const glm::mat4& transform)
    {
        OAK_PROFILE_FUNCTION();

        s_Data.cameraBuffer.viewProjection = camera.getProjection() * glm::inverse(transform);
        s_Data.cameraUniformBuffer->setData(&s_Data.cameraBuffer, sizeof(Renderer2DData::CameraData));

        startBatch();
    }

    void Renderer2D::beginScene(const EditorCamera& camera)
    {
        OAK_PROFILE_FUNCTION();

        s_Data.cameraBuffer.viewProjection = camera.getViewProjection();
        s_Data.cameraUniformBuffer->setData(&s_Data.cameraBuffer, sizeof(Renderer2DData::CameraData));

        startBatch();
    }

    void Renderer2D::endScene()
    {
        OAK_PROFILE_FUNCTION();

        flush();
    }

    void Renderer2D::startBatch()
    {
        s_Data.quadIndexCount = 0;
        s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

        s_Data.circleIndexCount = 0;
        s_Data.circleVertexBufferPtr = s_Data.circleVertexBufferBase;

        s_Data.lineVertexCount = 0;
        s_Data.lineVertexBufferPtr = s_Data.lineVertexBufferBase;

        s_Data.textIndexCount = 0;
        s_Data.textVertexBufferPtr = s_Data.textVertexBufferBase;

        s_Data.textureSlotIndex = 1;
    }

    void Renderer2D::flush()
    {
        if (s_Data.quadIndexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.quadVertexBufferPtr - (uint8_t*)s_Data.quadVertexBufferBase);
            s_Data.quadVertexBuffer->setData(s_Data.quadVertexBufferBase, dataSize);

            // Bind textures
            for (uint32_t i = 0; i < s_Data.textureSlotIndex; i++)
                s_Data.textureSlots[i]->bind(i);

            s_Data.quadShader->bind();
            RenderCommand::drawIndexed(s_Data.quadVertexArray, s_Data.quadIndexCount);
            s_Data.stats.drawCalls++;
        }

        if (s_Data.circleIndexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.circleVertexBufferPtr - (uint8_t*)s_Data.circleVertexBufferBase);
            s_Data.circleVertexBuffer->setData(s_Data.circleVertexBufferBase, dataSize);

            s_Data.circleShader->bind();
            RenderCommand::drawIndexed(s_Data.circleVertexArray, s_Data.circleIndexCount);
            s_Data.stats.drawCalls++;
        }

        if (s_Data.lineVertexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.lineVertexBufferPtr - (uint8_t*)s_Data.lineVertexBufferBase);
            s_Data.lineVertexBuffer->setData(s_Data.lineVertexBufferBase, dataSize);

            s_Data.lineShader->bind();
            RenderCommand::setLineWidth(s_Data.lineWidth);
            RenderCommand::drawLines(s_Data.lineVertexArray, s_Data.lineVertexCount);
            s_Data.stats.drawCalls++;
        }

        if (s_Data.textIndexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.textVertexBufferPtr - (uint8_t*)s_Data.textVertexBufferBase);
            s_Data.textVertexBuffer->setData(s_Data.textVertexBufferBase, dataSize);

            auto buf = s_Data.textVertexBufferBase;
            s_Data.fontAtlasTexture->bind(0);

            s_Data.textShader->bind();
            RenderCommand::drawIndexed(s_Data.textVertexArray, s_Data.textIndexCount);
            s_Data.stats.drawCalls++;
        }
    }

    void Renderer2D::nextBatch()
    {
        flush();
        startBatch();
    }

    void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
    {
        drawQuad({ position.x, position.y, 0.0f }, size, color);
    }

    void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
    {
        OAK_PROFILE_FUNCTION();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        drawQuad(transform, color);
    }

    void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        drawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
    }

    void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        OAK_PROFILE_FUNCTION();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        drawQuad(transform, texture, tilingFactor, tintColor);
    }

    void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
    {
        OAK_PROFILE_FUNCTION();

        constexpr size_t quadVertexCount = 4;
        const float textureIndex = 0.0f; // White Texture
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        const float tilingFactor = 1.0f;

        if (s_Data.quadIndexCount >= Renderer2DData::maxIndices)
            nextBatch();

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.quadVertexBufferPtr->position = transform * s_Data.quadVertexPositions[i];
            s_Data.quadVertexBufferPtr->color = color;
            s_Data.quadVertexBufferPtr->texCoord = textureCoords[i];
            s_Data.quadVertexBufferPtr->texIndex = textureIndex;
            s_Data.quadVertexBufferPtr->tilingFactor = tilingFactor;
            s_Data.quadVertexBufferPtr->entityID = entityID;
            s_Data.quadVertexBufferPtr++;
        }

        s_Data.quadIndexCount += 6;

        s_Data.stats.quadCount++;
    }

    void Renderer2D::drawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
    {
        OAK_PROFILE_FUNCTION();

        constexpr size_t quadVertexCount = 4;
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

        if (s_Data.quadIndexCount >= Renderer2DData::maxIndices)
            nextBatch();

        float textureIndex = 0.0f;
        for (uint32_t i = 1; i < s_Data.textureSlotIndex; i++)
        {
            if (*s_Data.textureSlots[i] == *texture)
            {
                textureIndex = (float)i;
                break;
            }
        }

        if (textureIndex == 0.0f)
        {
            if (s_Data.textureSlotIndex >= Renderer2DData::maxTextureSlots)
                nextBatch();

            textureIndex = (float)s_Data.textureSlotIndex;
            s_Data.textureSlots[s_Data.textureSlotIndex] = texture;
            s_Data.textureSlotIndex++;
        }

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.quadVertexBufferPtr->position = transform * s_Data.quadVertexPositions[i];
            s_Data.quadVertexBufferPtr->color = tintColor;
            s_Data.quadVertexBufferPtr->texCoord = textureCoords[i];
            s_Data.quadVertexBufferPtr->texIndex = textureIndex;
            s_Data.quadVertexBufferPtr->tilingFactor = tilingFactor;
            s_Data.quadVertexBufferPtr->entityID = entityID;
            s_Data.quadVertexBufferPtr++;
        }

        s_Data.quadIndexCount += 6;

        s_Data.stats.quadCount++;
    }

    void Renderer2D::drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        drawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
    }

    void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        OAK_PROFILE_FUNCTION();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        drawQuad(transform, color);
    }

    void Renderer2D::drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        drawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
    }

    void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
    {
        OAK_PROFILE_FUNCTION();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        drawQuad(transform, texture, tilingFactor, tintColor);
    }

    void Renderer2D::drawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness /*= 1.0f*/, float fade /*= 0.005f*/, int entityID /*= -1*/)
    {
        OAK_PROFILE_FUNCTION();

        // TODO: implement for circles
        // if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
        // 	NextBatch();

        for (size_t i = 0; i < 4; i++)
        {
            s_Data.circleVertexBufferPtr->worldPosition = transform * s_Data.quadVertexPositions[i];
            s_Data.circleVertexBufferPtr->localPosition = s_Data.quadVertexPositions[i] * 2.0f;
            s_Data.circleVertexBufferPtr->color = color;
            s_Data.circleVertexBufferPtr->thickness = thickness;
            s_Data.circleVertexBufferPtr->fade = fade;
            s_Data.circleVertexBufferPtr->entityID = entityID;
            s_Data.circleVertexBufferPtr++;
        }

        s_Data.circleIndexCount += 6;

        s_Data.stats.quadCount++;
    }

    void Renderer2D::drawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
    {
        s_Data.lineVertexBufferPtr->position = p0;
        s_Data.lineVertexBufferPtr->color = color;
        s_Data.lineVertexBufferPtr->entityID = entityID;
        s_Data.lineVertexBufferPtr++;

        s_Data.lineVertexBufferPtr->position = p1;
        s_Data.lineVertexBufferPtr->color = color;
        s_Data.lineVertexBufferPtr->entityID = entityID;
        s_Data.lineVertexBufferPtr++;

        s_Data.lineVertexCount += 2;
    }

    void Renderer2D::drawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
    {
        glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
        glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
        glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
        glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

        drawLine(p0, p1, color, entityID);
        drawLine(p1, p2, color, entityID);
        drawLine(p2, p3, color, entityID);
        drawLine(p3, p0, color, entityID);
    }

    void Renderer2D::drawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
    {
        glm::vec3 lineVertices[4];
        for (size_t i = 0; i < 4; i++)
            lineVertices[i] = transform * s_Data.quadVertexPositions[i];

        drawLine(lineVertices[0], lineVertices[1], color, entityID);
        drawLine(lineVertices[1], lineVertices[2], color, entityID);
        drawLine(lineVertices[2], lineVertices[3], color, entityID);
        drawLine(lineVertices[3], lineVertices[0], color, entityID);
    }

    void Renderer2D::drawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
    {
        if (src.Texture)
            drawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
        else
            drawQuad(transform, src.Color, entityID);
    }

    void Renderer2D::drawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID)
    {
        const auto& fontGeometry = font->getMSDFData()->fontGeometry;
        const auto& metrics = fontGeometry.getMetrics();
        Ref<Texture2D> fontAtlas = font->getAtlasTexture();

        s_Data.fontAtlasTexture = fontAtlas;

        double x = 0.0;
        double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
        double y = 0.0;

        const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();

        for (size_t i = 0; i < string.size(); i++)
        {
            char character = string[i];
            if (character == '\r')
                continue;

            if (character == '\n')
            {
                x = 0;
                y -= fsScale * metrics.lineHeight + textParams.LineSpacing;
                continue;
            }

            if (character == ' ')
            {
                float advance = spaceGlyphAdvance;
                if (i < string.size() - 1)
                {
                    char nextCharacter = string[i + 1];
                    double dAdvance;
                    fontGeometry.getAdvance(dAdvance, character, nextCharacter);
                    advance = (float)dAdvance;
                }

                x += fsScale * advance + textParams.Kerning;
                continue;
            }

            if (character == '\t')
            {
                // NOTE(Yan): is this right?
                x += 4.0f * (fsScale * spaceGlyphAdvance + textParams.Kerning);
                continue;
            }

            auto glyph = fontGeometry.getGlyph(character);
            if (!glyph)
                glyph = fontGeometry.getGlyph('?');
            if (!glyph)
                return;

            double al, ab, ar, at;
            glyph->getQuadAtlasBounds(al, ab, ar, at);
            glm::vec2 texCoordMin((float)al, (float)ab);
            glm::vec2 texCoordMax((float)ar, (float)at);

            double pl, pb, pr, pt;
            glyph->getQuadPlaneBounds(pl, pb, pr, pt);
            glm::vec2 quadMin((float)pl, (float)pb);
            glm::vec2 quadMax((float)pr, (float)pt);

            quadMin *= fsScale, quadMax *= fsScale;
            quadMin += glm::vec2(x, y);
            quadMax += glm::vec2(x, y);

            float texelWidth = 1.0f / fontAtlas->getWidth();
            float texelHeight = 1.0f / fontAtlas->getHeight();
            texCoordMin *= glm::vec2(texelWidth, texelHeight);
            texCoordMax *= glm::vec2(texelWidth, texelHeight);

            // render here
            s_Data.textVertexBufferPtr->position = transform * glm::vec4(quadMin, 0.0f, 1.0f);
            s_Data.textVertexBufferPtr->color = textParams.Color;
            s_Data.textVertexBufferPtr->texCoord = texCoordMin;
            s_Data.textVertexBufferPtr->entityID = entityID;
            s_Data.textVertexBufferPtr++;

            s_Data.textVertexBufferPtr->position = transform * glm::vec4(quadMin.x, quadMax.y, 0.0f, 1.0f);
            s_Data.textVertexBufferPtr->color = textParams.Color;
            s_Data.textVertexBufferPtr->texCoord = { texCoordMin.x, texCoordMax.y };
            s_Data.textVertexBufferPtr->entityID = entityID;
            s_Data.textVertexBufferPtr++;

            s_Data.textVertexBufferPtr->position = transform * glm::vec4(quadMax, 0.0f, 1.0f);
            s_Data.textVertexBufferPtr->color = textParams.Color;
            s_Data.textVertexBufferPtr->texCoord = texCoordMax;
            s_Data.textVertexBufferPtr->entityID = entityID;
            s_Data.textVertexBufferPtr++;

            s_Data.textVertexBufferPtr->position = transform * glm::vec4(quadMax.x, quadMin.y, 0.0f, 1.0f);
            s_Data.textVertexBufferPtr->color = textParams.Color;
            s_Data.textVertexBufferPtr->texCoord = { texCoordMax.x, texCoordMin.y };
            s_Data.textVertexBufferPtr->entityID = entityID;
            s_Data.textVertexBufferPtr++;

            s_Data.textIndexCount += 6;
            s_Data.stats.quadCount++;

            if (i < string.size() - 1)
            {
                double advance = glyph->getAdvance();
                char nextCharacter = string[i + 1];
                fontGeometry.getAdvance(advance, character, nextCharacter);

                x += fsScale * advance + textParams.Kerning;
            }
        }
    }

    void Renderer2D::drawString(const std::string& string, const glm::mat4& transform, const TextComponent& component, int entityID)
    {
        drawString(string, component.FontAsset, transform, { component.Color, component.Kerning, component.LineSpacing }, entityID);
    }

    float Renderer2D::getLineWidth()
    {
        return s_Data.lineWidth;
    }

    void Renderer2D::setLineWidth(float width)
    {
        s_Data.lineWidth = width;
    }

    void Renderer2D::resetStats()
    {
        memset(&s_Data.stats, 0, sizeof(Statistics));
    }

    Renderer2D::Statistics Renderer2D::getStats()
    {
        return s_Data.stats;
    }

}
