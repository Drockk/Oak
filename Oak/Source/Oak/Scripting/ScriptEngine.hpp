#pragma once

#include "Oak/Scene/Scene.hpp"
#include "Oak/Scene/Entity.hpp"

#include <filesystem>
#include <string>
#include <map>

extern "C" {
    typedef struct _MonoClass MonoClass;
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoMethod MonoMethod;
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
    typedef struct _MonoClassField MonoClassField;
    typedef struct _MonoString MonoString;
}

namespace oak {
    enum class ScriptFieldType
    {
        None = 0,
        Float, Double,
        Bool, Char, Byte, Short, Int, Long,
        UByte, UShort, UInt, ULong,
        Vector2, Vector3, Vector4,
        Entity
    };

    struct ScriptField
    {
        ScriptFieldType type;
        std::string name;

        MonoClassField* classField;
    };

    // ScriptField + data storage
    struct ScriptFieldInstance
    {
        ScriptField field;

        ScriptFieldInstance()
        {
            memset(m_Buffer, 0, sizeof(m_Buffer));
        }

        template<typename T>
        T getValue()
        {
            static_assert(sizeof(T) <= 16, "Type too large!");
            return *reinterpret_cast<T*>(m_Buffer);
        }

        template<typename T>
        void setValue(T value)
        {
            static_assert(sizeof(T) <= 16, "Type too large!");
            memcpy(m_Buffer, &value, sizeof(T));
        }

    private:
        uint8_t m_Buffer[16];

        friend class ScriptEngine;
        friend class ScriptInstance;
    };

    using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

    class ScriptClass
    {
    public:
        ScriptClass() = default;
        ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

        MonoObject* instantiate();
        MonoMethod* getMethod(const std::string& name, int parameterCount);
        MonoObject* invokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

        const std::map<std::string, ScriptField>& getFields() const { return m_Fields; }

    private:
        std::string m_ClassNamespace;
        std::string m_ClassName;

        std::map<std::string, ScriptField> m_Fields;

        MonoClass* m_MonoClass = nullptr;

        friend class ScriptEngine;
    };

    class ScriptInstance
    {
    public:
        ScriptInstance(Ref<ScriptClass> scriptClass, oak::Entity entity);

        void invokeOnCreate();
        void invokeOnUpdate(float ts);

        Ref<ScriptClass> getScriptClass() { return m_ScriptClass; }

        template<typename T>
        T getFieldValue(const std::string& name)
        {
            static_assert(sizeof(T) <= 16, "Type too large!");

            if (!getFieldValueInternal(name, s_FieldValueBuffer)) {
                return T();
            }

            return *reinterpret_cast<T*>(s_FieldValueBuffer);
        }

        template<typename T>
        void setFieldValue(const std::string& name, T value)
        {
            static_assert(sizeof(T) <= 16, "Type too large!");

            setFieldValueInternal(name, &value);
        }

        MonoObject* getManagedObject() { return m_Instance; }

    private:
        bool getFieldValueInternal(const std::string& name, void* buffer);
        bool setFieldValueInternal(const std::string& name, const void* value);

        Ref<ScriptClass> m_ScriptClass;

        MonoObject* m_Instance = nullptr;
        MonoMethod* m_Constructor = nullptr;
        MonoMethod* m_OnCreateMethod = nullptr;
        MonoMethod* m_OnUpdateMethod = nullptr;

        inline static char s_FieldValueBuffer[16];

        friend class ScriptEngine;
        friend struct ScriptFieldInstance;
    };

    class ScriptEngine
    {
    public:
        static void init();
        static void shutdown();

        static bool loadAssembly(const std::filesystem::path& filepath);
        static bool loadAppAssembly(const std::filesystem::path& filepath);

        static void reloadAssembly();

        static void onRuntimeStart(Scene* scene);
        static void onRuntimeStop();

        static bool entityClassExists(const std::string& fullClassName);
        static void onCreateEntity(oak::Entity entity);
        static void onUpdateEntity(oak::Entity entity, Timestep ts);

        static Scene* getSceneContext();
        static Ref<ScriptInstance> getEntityScriptInstance(oak::UUID entityID);

        static Ref<ScriptClass> getEntityClass(const std::string& name);
        static std::unordered_map<std::string, Ref<ScriptClass>> getEntityClasses();
        static ScriptFieldMap& getScriptFieldMap(oak::Entity entity);

        static MonoImage* getCoreAssemblyImage();

        static MonoObject* getManagedInstance(oak::UUID uuid);

        static MonoString* createString(const char* string);

    private:
        static void initMono();
        static void shutdownMono();

        static MonoObject* instantiateClass(MonoClass* monoClass);
        static void loadAssemblyClasses();

        friend class ScriptClass;
        friend class ScriptGlue;
    };

    namespace utils {
        inline const char* scriptFieldTypeToString(ScriptFieldType fieldType)
        {
            switch (fieldType)
            {
                case ScriptFieldType::None:
                    return "None";
                case ScriptFieldType::Float:
                    return "Float";
                case ScriptFieldType::Double:
                    return "Double";
                case ScriptFieldType::Bool:
                    return "Bool";
                case ScriptFieldType::Char:
                    return "Char";
                case ScriptFieldType::Byte:
                    return "Byte";
                case ScriptFieldType::Short:
                    return "Short";
                case ScriptFieldType::Int:
                    return "Int";
                case ScriptFieldType::Long:
                    return "Long";
                case ScriptFieldType::UByte:
                    return "UByte";
                case ScriptFieldType::UShort:
                    return "UShort";
                case ScriptFieldType::UInt:
                    return "UInt";
                case ScriptFieldType::ULong:
                    return "ULong";
                case ScriptFieldType::Vector2:
                    return "Vector2";
                case ScriptFieldType::Vector3:
                    return "Vector3";
                case ScriptFieldType::Vector4:
                    return "Vector4";
                case ScriptFieldType::Entity:
                    return "Entity";
            }

            OAK_CORE_ASSERT(false, "Unknown ScriptFieldType");
            return "None";
        }

        inline ScriptFieldType scriptFieldTypeFromString(std::string_view fieldType)
        {
            if (fieldType == "None") {
                return ScriptFieldType::None;
            }
            if (fieldType == "Float") {
                return ScriptFieldType::Float;
            }
            if (fieldType == "Double") {
                return ScriptFieldType::Double;
            }
            if (fieldType == "Bool") {
                return ScriptFieldType::Bool;
            }
            if (fieldType == "Char") {
                return ScriptFieldType::Char;
            }
            if (fieldType == "Byte") {
                return ScriptFieldType::Byte;
            }
            if (fieldType == "Short") {
                return ScriptFieldType::Short;
            }
            if (fieldType == "Int") {
                return ScriptFieldType::Int;
            }
            if (fieldType == "Long") {
                return ScriptFieldType::Long;
            }
            if (fieldType == "UByte") {
                return ScriptFieldType::UByte;
            }
            if (fieldType == "UShort") {
                return ScriptFieldType::UShort;
            }
            if (fieldType == "UInt") {
                return ScriptFieldType::UInt;
            }
            if (fieldType == "ULong") {
                return ScriptFieldType::ULong;
            }
            if (fieldType == "Vector2") {
                return ScriptFieldType::Vector2;
            }
            if (fieldType == "Vector3") {
                return ScriptFieldType::Vector3;
            }
            if (fieldType == "Vector4") {
                return ScriptFieldType::Vector4;
            }
            if (fieldType == "Entity") {
                return ScriptFieldType::Entity;
            }

            OAK_CORE_ASSERT(false, "Unknown ScriptFieldType");
            return ScriptFieldType::None;
        }
    }
}
