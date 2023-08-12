#include "oakpch.hpp"
#include "ScriptGlue.hpp"
#include "ScriptEngine.hpp"

#include "Oak/Core/UUID.hpp"
#include "Oak/Core/KeyCodes.hpp"
#include "Oak/Core/Input.hpp"

#include "Oak/Scene/Scene.hpp"
#include "Oak/Scene/Entity.hpp"

#include "Oak/Physics/Physics2D.hpp"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "box2d/b2_body.h"

namespace oak {
    namespace utils {
        std::string monoStringToString(MonoString* string)
        {
            char* cStr = mono_string_to_utf8(string);
            std::string str(cStr);
            mono_free(cStr);
            return str;
        }

    }

    static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define HZ_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Oak.InternalCalls::" #Name, Name)

    static void NativeLog(MonoString* string, int parameter)
    {
        std::string str = utils::monoStringToString(string);
        std::cout << str << ", " << parameter << std::endl;
    }

    static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
    {
        OAK_LOG_CORE_WARN("Value: {0}", *parameter);
        *outResult = glm::normalize(*parameter);
    }

    static float NativeLog_VectorDot(glm::vec3* parameter)
    {
        OAK_LOG_CORE_WARN("Value: {0}", *parameter);
        return glm::dot(*parameter, *parameter);
    }

    static MonoObject* GetScriptInstance(UUID entityID)
    {
        return ScriptEngine::getManagedInstance(entityID);
    }

    static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        MonoType* managedType = mono_reflection_type_get_type(componentType);
        OAK_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
        return s_EntityHasComponentFuncs.at(managedType)(entity);
    }

    static uint64_t Entity_FindEntityByName(MonoString* name)
    {
        char* nameCStr = mono_string_to_utf8(name);

        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->findEntityByName(nameCStr);
        mono_free(nameCStr);

        if (!entity)
            return 0;

        return entity.getUUID();
    }

    static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        *outTranslation = entity.getComponent<TransformComponent>().translation;
    }

    static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        entity.getComponent<TransformComponent>().translation = *translation;
    }

    static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.runtimeBody;
        body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
    }

    static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.runtimeBody;
        body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
    }

    static void Rigidbody2DComponent_GetLinearVelocity(UUID entityID, glm::vec2* outLinearVelocity)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.runtimeBody;
        const b2Vec2& linearVelocity = body->GetLinearVelocity();
        *outLinearVelocity = glm::vec2(linearVelocity.x, linearVelocity.y);
    }

    static Rigidbody2DComponent::BodyType Rigidbody2DComponent_GetType(UUID entityID)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.runtimeBody;
        return utils::rigidbody2DTypeFromBox2DBody(body->GetType());
    }

    static void Rigidbody2DComponent_SetType(UUID entityID, Rigidbody2DComponent::BodyType bodyType)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);

        auto& rb2d = entity.getComponent<Rigidbody2DComponent>();
        b2Body* body = (b2Body*)rb2d.runtimeBody;
        body->SetType(utils::rigidbody2DTypeToBox2DBody(bodyType));
    }

    static MonoString* TextComponent_GetText(UUID entityID)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        return ScriptEngine::createString(tc.textString.c_str());
    }

    static void TextComponent_SetText(UUID entityID, MonoString* textString)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        tc.textString = utils::monoStringToString(textString);
    }

    static void TextComponent_GetColor(UUID entityID, glm::vec4* color)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        *color = tc.color;
    }

    static void TextComponent_SetColor(UUID entityID, glm::vec4* color)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        tc.color = *color;
    }

    static float TextComponent_GetKerning(UUID entityID)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        return tc.kerning;
    }

    static void TextComponent_SetKerning(UUID entityID, float kerning)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        tc.kerning = kerning;
    }

    static float TextComponent_GetLineSpacing(UUID entityID)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        return tc.lineSpacing;
    }

    static void TextComponent_SetLineSpacing(UUID entityID, float lineSpacing)
    {
        auto* scene = ScriptEngine::getSceneContext();
        OAK_CORE_ASSERT(scene);
        auto entity = scene->getEntityByUUID(entityID);
        OAK_CORE_ASSERT(entity);
        OAK_CORE_ASSERT(entity.hasComponent<TextComponent>());

        auto& tc = entity.getComponent<TextComponent>();
        tc.lineSpacing = lineSpacing;
    }

    static bool Input_IsKeyDown(KeyCode keycode)
    {
        return Input::isKeyPressed(keycode);
    }

    template<typename... Component>
    static void RegisterComponent()
    {
        ([]() {
            std::string_view typeName = typeid(Component).name();
            auto pos = typeName.find_last_of(':');
            auto structName = typeName.substr(pos + 1);
            auto managedTypename = fmt::format("Oak.{}", structName);

            auto* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::getCoreAssemblyImage());
            if (!managedType) {
                OAK_LOG_CORE_ERROR("Could not find component type {}", managedTypename);
                return;
            }
            s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.hasComponent<Component>(); };
        }(), ...);
    }

    template<typename... Component>
    static void RegisterComponent(ComponentGroup<Component...>)
    {
        RegisterComponent<Component...>();
    }

    void ScriptGlue::registerComponents()
    {
        s_EntityHasComponentFuncs.clear();
        RegisterComponent(AllComponents{});
    }

    void ScriptGlue::registerFunctions()
    {
        HZ_ADD_INTERNAL_CALL(NativeLog);
        HZ_ADD_INTERNAL_CALL(NativeLog_Vector);
        HZ_ADD_INTERNAL_CALL(NativeLog_VectorDot);

        HZ_ADD_INTERNAL_CALL(GetScriptInstance);

        HZ_ADD_INTERNAL_CALL(Entity_HasComponent);
        HZ_ADD_INTERNAL_CALL(Entity_FindEntityByName);

        HZ_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
        HZ_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);
        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetLinearVelocity);
        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetType);
        HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_SetType);

        HZ_ADD_INTERNAL_CALL(TextComponent_GetText);
        HZ_ADD_INTERNAL_CALL(TextComponent_SetText);
        HZ_ADD_INTERNAL_CALL(TextComponent_GetColor);
        HZ_ADD_INTERNAL_CALL(TextComponent_SetColor);
        HZ_ADD_INTERNAL_CALL(TextComponent_GetKerning);
        HZ_ADD_INTERNAL_CALL(TextComponent_SetKerning);
        HZ_ADD_INTERNAL_CALL(TextComponent_GetLineSpacing);
        HZ_ADD_INTERNAL_CALL(TextComponent_SetLineSpacing);

        HZ_ADD_INTERNAL_CALL(Input_IsKeyDown);
    }
}
