#include "oakpch.hpp"
#include "Oak/Scripting/ScriptGlue.hpp"
#include "Oak/Scripting/ScriptEngine.hpp"

#include "Oak/Core/UUID.hpp"
#include "Oak/Core/KeyCodes.hpp"
#include "Oak/Core/Input.hpp"

#include "Oak/Scene/Scene.hpp"
#include "Oak/Scene/Entity.hpp"

#include "Oak/Physics/Physics2D.hpp"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "box2d/b2_body.h"

namespace oak
{
    namespace utils
    {
        std::string monoStringToString(MonoString* string)
        {
            char* cStr = mono_string_to_utf8(string);
            std::string str(cStr);
            mono_free(cStr);
            return str;
        }
    }

    static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define OAK_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Oak.InternalCalls::" #Name, Name)

    static void nativeLog(MonoString* string, int parameter)
    {
        std::string str = utils::monoStringToString(string);
        std::cout << str << ", " << parameter << std::endl;
    }

    static void nativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
    {
        OAK_LOG_CORE_WARN("Value: {0}", *parameter);
        *outResult = glm::normalize(*parameter);
    }

    static float nativeLog_VectorDot(glm::vec3* parameter)
    {
        OAK_LOG_CORE_WARN("Value: {0}", *parameter);
        return glm::dot(*parameter, *parameter);
    }

    static MonoObject* getScriptInstance(UUID entityID)
    {
        return ScriptEngine::getManagedInstance(entityID);
    }

    static bool entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        MonoType* managedType = mono_reflection_type_get_type(componentType);
        OAK_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
        return s_EntityHasComponentFuncs.at(managedType)(entity);
    }

    static uint64_t entity_FindEntityByName(MonoString* name)
    {
        char* nameCStr = mono_string_to_utf8(name);

        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->findEntityByName(nameCStr);
        mono_free(nameCStr);

        if (!entity)
            return 0;

        return entity.getUUID();
    }

    static void transformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        *outTranslation = entity.getComponent<TransformComponent>().translation;
    }

    static void transformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        entity.getComponent<TransformComponent>().translation = *translation;
    }

    static void rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.runtimeBody;
        body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
    }

    static void rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.runtimeBody;
        body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
    }

    static void rigidbody2DComponent_GetLinearVelocity(UUID entityID, glm::vec2* outLinearVelocity)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.runtimeBody;
        const b2Vec2& linearVelocity = body->GetLinearVelocity();
        *outLinearVelocity = glm::vec2(linearVelocity.x, linearVelocity.y);
    }

    static Rigidbody2DComponent::BodyType rigidbody2DComponent_GetType(UUID entityID)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.runtimeBody;
        return utils::rigidbody2DTypeFromBox2DBody(body->GetType());
    }

    static void rigidbody2DComponent_SetType(UUID entityID, Rigidbody2DComponent::BodyType bodyType)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.runtimeBody;
        body->SetType(utils::rigidbody2DTypeToBox2DBody(bodyType));
    }

    static MonoString* textComponent_GetText(UUID entityID)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        return ScriptEngine::createString(tc.textString.c_str());
    }

    static void textComponent_SetText(UUID entityID, MonoString* textString)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        tc.textString = utils::monoStringToString(textString);
    }

    static void textComponent_GetColor(UUID entityID, glm::vec4* color)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        *color = tc.color;
    }

    static void textComponent_SetColor(UUID entityID, glm::vec4* color)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        tc.color = *color;
    }

    static float textComponent_GetKerning(UUID entityID)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        return tc.kerning;
    }

    static void textComponent_SetKerning(UUID entityID, float kerning)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        tc.kerning = kerning;
    }

    static float textComponent_GetLineSpacing(UUID entityID)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        return tc.lineSpacing;
    }

    static void textComponent_SetLineSpacing(UUID entityID, float lineSpacing)
    {
        Scene* scene = ScriptEngine::getsceneContext();
        OAK_CORE_ASSERT(scene);
        Entity entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        tc.lineSpacing = lineSpacing;
    }

    static bool input_IsKeyDown(KeyCode keycode)
    {
        return Input::isKeyPressed(keycode);
    }

    template<typename... Component>
    static void registerComponent()
    {
        ([]()
            {
                std::string_view typeName = typeid(Component).name();
                size_t pos = typeName.find_last_of(':');
                std::string_view structName = typeName.substr(pos + 1);
                std::string managedTypename = fmt::format("Oak.{}", structName);

                MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::getcoreAssemblyImage());
                if (!managedType)
                {
                    OAK_LOG_CORE_ERROR("Could not find component type {}", managedTypename);
                    return;
                }
                s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.hasComponent<Component>(); };
            }(), ...);
    }

    template<typename... Component>
    static void registerComponent(ComponentGroup<Component...>)
    {
        registerComponent<Component...>();
    }

    void ScriptGlue::registerComponents()
    {
        s_EntityHasComponentFuncs.clear();
        registerComponent(AllComponents{});
    }

    void ScriptGlue::registerFunctions()
    {
        OAK_ADD_INTERNAL_CALL(nativeLog);
        OAK_ADD_INTERNAL_CALL(nativeLog_Vector);
        OAK_ADD_INTERNAL_CALL(nativeLog_VectorDot);

        OAK_ADD_INTERNAL_CALL(getScriptInstance);

        OAK_ADD_INTERNAL_CALL(entity_HasComponent);
        OAK_ADD_INTERNAL_CALL(entity_FindEntityByName);

        OAK_ADD_INTERNAL_CALL(transformComponent_GetTranslation);
        OAK_ADD_INTERNAL_CALL(transformComponent_SetTranslation);

        OAK_ADD_INTERNAL_CALL(rigidbody2DComponent_ApplyLinearImpulse);
        OAK_ADD_INTERNAL_CALL(rigidbody2DComponent_ApplyLinearImpulseToCenter);
        OAK_ADD_INTERNAL_CALL(rigidbody2DComponent_GetLinearVelocity);
        OAK_ADD_INTERNAL_CALL(rigidbody2DComponent_GetType);
        OAK_ADD_INTERNAL_CALL(rigidbody2DComponent_SetType);

        OAK_ADD_INTERNAL_CALL(textComponent_GetText);
        OAK_ADD_INTERNAL_CALL(textComponent_SetText);
        OAK_ADD_INTERNAL_CALL(textComponent_GetColor);
        OAK_ADD_INTERNAL_CALL(textComponent_SetColor);
        OAK_ADD_INTERNAL_CALL(textComponent_GetKerning);
        OAK_ADD_INTERNAL_CALL(textComponent_SetKerning);
        OAK_ADD_INTERNAL_CALL(textComponent_GetLineSpacing);
        OAK_ADD_INTERNAL_CALL(textComponent_SetLineSpacing);

        OAK_ADD_INTERNAL_CALL(input_IsKeyDown);
    }
}
