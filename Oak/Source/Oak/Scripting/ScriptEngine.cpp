#include "oakpch.hpp"
#include "Oak/Scripting/ScriptEngine.hpp"

#include "Oak/Scripting/ScriptGlue.hpp"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/threads.h"

#include "FileWatch.h"

#include "Oak/Core/Application.hpp"
#include "Oak/Core/Timer.hpp"
#include "Oak/Core/Buffer.hpp"
#include "Oak/Core/FileSystem.hpp"

#include "Oak/Project/Project.hpp"

namespace oak
{
    static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
    {
        { "System.Single", ScriptFieldType::Float },
        { "System.Double", ScriptFieldType::Double },
        { "System.Boolean", ScriptFieldType::Bool },
        { "System.Char", ScriptFieldType::Char },
        { "System.Int16", ScriptFieldType::Short },
        { "System.Int32", ScriptFieldType::Int },
        { "System.Int64", ScriptFieldType::Long },
        { "System.Byte", ScriptFieldType::Byte },
        { "System.UInt16", ScriptFieldType::UShort },
        { "System.UInt32", ScriptFieldType::UInt },
        { "System.UInt64", ScriptFieldType::ULong },

        { "Oak.Vector2", ScriptFieldType::Vector2 },
        { "Oak.Vector3", ScriptFieldType::Vector3 },
        { "Oak.Vector4", ScriptFieldType::Vector4 },

        { "Oak.Entity", ScriptFieldType::Entity },
    };

    namespace utils
    {
        static MonoAssembly* loadMonoAssembly(const std::filesystem::path& assemblyPath, bool loadPDB = false)
        {
            ScopedBuffer fileData = FileSystem::readFileBinary(assemblyPath);

            // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
            MonoImageOpenStatus status;
            MonoImage* image = mono_image_open_from_data_full(fileData.as<char>(), fileData.size(), 1, &status, 0);

            if (status != MONO_IMAGE_OK)
            {
                const char* errorMessage = mono_image_strerror(status);
                // Log some error message using the errorMessage data
                return nullptr;
            }

            if (loadPDB)
            {
                std::filesystem::path pdbPath = assemblyPath;
                pdbPath.replace_extension(".pdb");

                if (std::filesystem::exists(pdbPath))
                {
                    ScopedBuffer pdbFileData = FileSystem::readFileBinary(pdbPath);
                    mono_debug_open_image_from_memory(image, pdbFileData.as<const mono_byte>(), pdbFileData.size());
                    OAK_LOG_CORE_INFO("Loaded PDB {}", pdbPath);
                }
            }

            std::string pathString = assemblyPath.string();
            MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
            mono_image_close(image);

            return assembly;
        }

        void printAssemblyTypes(MonoAssembly* assembly)
        {
            MonoImage* image = mono_assembly_get_image(assembly);
            const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
            int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

            for (int32_t i = 0; i < numTypes; i++)
            {
                uint32_t cols[MONO_TYPEDEF_SIZE];
                mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

                const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
                const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
                OAK_LOG_CORE_TRACE("{}.{}", nameSpace, name);
            }
        }

        ScriptFieldType monoTypeToScriptFieldType(MonoType* monoType)
        {
            std::string typeName = mono_type_get_name(monoType);

            auto it = s_ScriptFieldTypeMap.find(typeName);
            if (it == s_ScriptFieldTypeMap.end())
            {
                OAK_LOG_CORE_ERROR("Unknown type: {}", typeName);
                return ScriptFieldType::None;
            }

            return it->second;
        }

    }

    struct ScriptEngineData
    {
        MonoDomain* rootDomain = nullptr;
        MonoDomain* appDomain = nullptr;

        MonoAssembly* coreAssembly = nullptr;
        MonoImage* coreAssemblyImage = nullptr;

        MonoAssembly* appAssembly = nullptr;
        MonoImage* appAssemblyImage = nullptr;

        std::filesystem::path coreAssemblyFilepath;
        std::filesystem::path appAssemblyFilepath;

        ScriptClass entityClass;

        std::unordered_map<std::string, Ref<ScriptClass>> entityClasses;
        std::unordered_map<UUID, Ref<ScriptInstance>> entityInstances;
        std::unordered_map<UUID, ScriptFieldMap> entityScriptFields;

        Scope<filewatch::FileWatch<std::string>> appAssemblyFileWatcher;
        bool assemblyReloadPending = false;

#ifdef OAK_DEBUG
        bool enableDebugging = true;
#else
        bool enableDebugging = false;
#endif
        // Runtime

        Scene* sceneContext = nullptr;
    };

    static ScriptEngineData* s_Data = nullptr;

    static void onAppAssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
    {
        if (!s_Data->assemblyReloadPending && change_type == filewatch::Event::modified)
        {
            s_Data->assemblyReloadPending = true;

            Application::get().submitToMainThread([]()
                {
                    s_Data->appAssemblyFileWatcher.reset();
                    ScriptEngine::reloadAssembly();
                });
        }
    }

    void ScriptEngine::init()
    {
        s_Data = new ScriptEngineData();

        initMono();
        ScriptGlue::registerFunctions();

        bool status = loadAssembly("Resources/Scripts/Oak-ScriptCore.dll");
        if (!status)
        {
            OAK_LOG_CORE_ERROR("[ScriptEngine] Could not load Oak-ScriptCore assembly.");
            return;
        }

        auto scriptModulePath = Project::getAssetDirectory() / Project::getActive()->getConfig().scriptModulePath;
        status = loadAppAssembly(scriptModulePath);
        if (!status)
        {
            OAK_LOG_CORE_ERROR("[ScriptEngine] Could not load app assembly.");
            return;
        }

        loadAssemblyClasses();

        ScriptGlue::registerComponents();

        // Retrieve and instantiate class
        s_Data->entityClass = ScriptClass("Oak", "Entity", true);
    }

    void ScriptEngine::shutdown()
    {
        shutdownMono();
        delete s_Data;
    }

    void ScriptEngine::initMono()
    {
        mono_set_assemblies_path("mono/lib");

        if (s_Data->enableDebugging)
        {
            const char* argv[2] = {
                "--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebugger.log",
                "--soft-breakpoints"
            };

            mono_jit_parse_options(2, (char**)argv);
            mono_debug_init(MONO_DEBUG_FORMAT_MONO);
        }

        MonoDomain* rootDomain = mono_jit_init("HazelJITRuntime");
        OAK_CORE_ASSERT(rootDomain);

        // Store the root domain pointer
        s_Data->rootDomain = rootDomain;

        if (s_Data->enableDebugging)
            mono_debug_domain_create(s_Data->rootDomain);

        mono_thread_set_main(mono_thread_current());
    }

    void ScriptEngine::shutdownMono()
    {
        mono_domain_set(mono_get_root_domain(), false);

        mono_domain_unload(s_Data->appDomain);
        s_Data->appDomain = nullptr;

        mono_jit_cleanup(s_Data->rootDomain);
        s_Data->rootDomain = nullptr;
    }

    bool ScriptEngine::loadAssembly(const std::filesystem::path& filepath)
    {
        // Create an App Domain
        s_Data->appDomain = mono_domain_create_appdomain("OakScriptRuntime", nullptr);
        mono_domain_set(s_Data->appDomain, true);

        s_Data->coreAssemblyFilepath = filepath;
        s_Data->coreAssembly = utils::loadMonoAssembly(filepath, s_Data->enableDebugging);
        if (s_Data->coreAssembly == nullptr)
            return false;

        s_Data->coreAssemblyImage = mono_assembly_get_image(s_Data->coreAssembly);
        return true;
    }

    bool ScriptEngine::loadAppAssembly(const std::filesystem::path& filepath)
    {
        s_Data->appAssemblyFilepath = filepath;
        s_Data->appAssembly = utils::loadMonoAssembly(filepath, s_Data->enableDebugging);
        if (s_Data->appAssembly == nullptr)
            return false;

        s_Data->appAssemblyImage = mono_assembly_get_image(s_Data->appAssembly);

        s_Data->appAssemblyFileWatcher = createScope<filewatch::FileWatch<std::string>>(filepath.string(), onAppAssemblyFileSystemEvent);
        s_Data->assemblyReloadPending = false;
        return true;
    }

    void ScriptEngine::reloadAssembly()
    {
        mono_domain_set(mono_get_root_domain(), false);

        mono_domain_unload(s_Data->appDomain);

        loadAssembly(s_Data->coreAssemblyFilepath);
        loadAppAssembly(s_Data->appAssemblyFilepath);
        loadAssemblyClasses();

        ScriptGlue::registerComponents();

        // Retrieve and instantiate class
        s_Data->entityClass = ScriptClass("Oak", "Entity", true);
    }

    void ScriptEngine::onRuntimeStart(Scene* scene)
    {
        s_Data->sceneContext = scene;
    }

    bool ScriptEngine::entityClassExists(const std::string& fullClassName)
    {
        return s_Data->entityClasses.find(fullClassName) != s_Data->entityClasses.end();
    }

    void ScriptEngine::onCreateEntity(Entity entity)
    {
        const auto& sc = entity.getComponent<ScriptComponent>();
        if (ScriptEngine::entityClassExists(sc.className))
        {
            UUID entityID = entity.getUUID();

            Ref<ScriptInstance> instance = createRef<ScriptInstance>(s_Data->entityClasses[sc.className], entity);
            s_Data->entityInstances[entityID] = instance;

            // Copy field values
            if (s_Data->entityScriptFields.find(entityID) != s_Data->entityScriptFields.end())
            {
                const ScriptFieldMap& fieldMap = s_Data->entityScriptFields.at(entityID);
                for (const auto& [name, fieldInstance] : fieldMap)
                    instance->setFieldValueInternal(name, fieldInstance.m_Buffer);
            }

            instance->invokeOnCreate();
        }
    }

    void ScriptEngine::onUpdateEntity(Entity entity, Timestep ts)
    {
        UUID entityUUID = entity.getUUID();
        if (s_Data->entityInstances.find(entityUUID) != s_Data->entityInstances.end())
        {
            Ref<ScriptInstance> instance = s_Data->entityInstances[entityUUID];
            instance->invokeOnUpdate((float)ts);
        }
        else
        {
            OAK_LOG_CORE_ERROR("Could not find ScriptInstance for entity {}", entityUUID);
        }
    }

    Scene* ScriptEngine::getsceneContext()
    {
        return s_Data->sceneContext;
    }

    Ref<ScriptInstance> ScriptEngine::getEntityScriptInstance(UUID entityID)
    {
        auto it = s_Data->entityInstances.find(entityID);
        if (it == s_Data->entityInstances.end())
            return nullptr;

        return it->second;
    }


    Ref<ScriptClass> ScriptEngine::getEntityClass(const std::string& name)
    {
        if (s_Data->entityClasses.find(name) == s_Data->entityClasses.end())
            return nullptr;

        return s_Data->entityClasses.at(name);
    }

    void ScriptEngine::onRuntimeStop()
    {
        s_Data->sceneContext = nullptr;

        s_Data->entityInstances.clear();
    }

    std::unordered_map<std::string, Ref<ScriptClass>> ScriptEngine::getentityClasses()
    {
        return s_Data->entityClasses;
    }

    ScriptFieldMap& ScriptEngine::getScriptFieldMap(Entity entity)
    {
        OAK_CORE_ASSERT(entity);

        UUID entityID = entity.getUUID();
        return s_Data->entityScriptFields[entityID];
    }

    void ScriptEngine::loadAssemblyClasses()
    {
        s_Data->entityClasses.clear();

        const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->appAssemblyImage, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
        MonoClass* entityClass = mono_class_from_name(s_Data->coreAssemblyImage, "Oak", "Entity");

        for (int32_t i = 0; i < numTypes; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char* nameSpace = mono_metadata_string_heap(s_Data->appAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
            const char* className = mono_metadata_string_heap(s_Data->appAssemblyImage, cols[MONO_TYPEDEF_NAME]);
            std::string fullName;
            if (strlen(nameSpace) != 0)
                fullName = fmt::format("{}.{}", nameSpace, className);
            else
                fullName = className;

            MonoClass* monoClass = mono_class_from_name(s_Data->appAssemblyImage, nameSpace, className);

            if (monoClass == entityClass)
                continue;

            bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
            if (!isEntity)
                continue;

            Ref<ScriptClass> scriptClass = createRef<ScriptClass>(nameSpace, className);
            s_Data->entityClasses[fullName] = scriptClass;


            // This routine is an iterator routine for retrieving the fields in a class.
            // You must pass a gpointer that points to zero and is treated as an opaque handle
            // to iterate over all of the elements. When no more values are available, the return value is NULL.

            int fieldCount = mono_class_num_fields(monoClass);
            OAK_LOG_CORE_WARN("{} has {} fields:", className, fieldCount);
            void* iterator = nullptr;
            while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
            {
                const char* fieldName = mono_field_get_name(field);
                uint32_t flags = mono_field_get_flags(field);
                if (flags & FIELD_ATTRIBUTE_PUBLIC)
                {
                    MonoType* type = mono_field_get_type(field);
                    ScriptFieldType fieldType = utils::monoTypeToScriptFieldType(type);
                    OAK_LOG_CORE_WARN("  {} ({})", fieldName, utils::scriptFieldTypeToString(fieldType));

                    scriptClass->m_Fields[fieldName] = { fieldType, fieldName, field };
                }
            }

        }

        auto& entityClasses = s_Data->entityClasses;

        //mono_field_get_value()

    }

    MonoImage* ScriptEngine::getcoreAssemblyImage()
    {
        return s_Data->coreAssemblyImage;
    }


    MonoObject* ScriptEngine::getManagedInstance(UUID uuid)
    {
        OAK_CORE_ASSERT(s_Data->entityInstances.find(uuid) != s_Data->entityInstances.end());
        return s_Data->entityInstances.at(uuid)->getManagedObject();
    }

    MonoString* ScriptEngine::createString(const char* string)
    {
        return mono_string_new(s_Data->appDomain, string);
    }

    MonoObject* ScriptEngine::instantiateClass(MonoClass* monoClass)
    {
        MonoObject* instance = mono_object_new(s_Data->appDomain, monoClass);
        mono_runtime_object_init(instance);
        return instance;
    }

    ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
        : m_ClassNamespace(classNamespace), m_ClassName(className)
    {
        m_MonoClass = mono_class_from_name(isCore ? s_Data->coreAssemblyImage : s_Data->appAssemblyImage, classNamespace.c_str(), className.c_str());
    }

    MonoObject* ScriptClass::instantiate()
    {
        return ScriptEngine::instantiateClass(m_MonoClass);
    }

    MonoMethod* ScriptClass::getMethod(const std::string& name, int parameterCount)
    {
        return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
    }

    MonoObject* ScriptClass::invokeMethod(MonoObject* instance, MonoMethod* method, void** params)
    {
        MonoObject* exception = nullptr;
        return mono_runtime_invoke(method, instance, params, &exception);
    }

    ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
        : m_ScriptClass(scriptClass)
    {
        m_Instance = scriptClass->instantiate();

        m_Constructor = s_Data->entityClass.getMethod(".ctor", 1);
        m_OnCreateMethod = scriptClass->getMethod("OnCreate", 0);
        m_OnUpdateMethod = scriptClass->getMethod("OnUpdate", 1);

        // Call Entity constructor
        {
            UUID entityID = entity.getUUID();
            void* param = &entityID;
            m_ScriptClass->invokeMethod(m_Instance, m_Constructor, &param);
        }
    }

    void ScriptInstance::invokeOnCreate()
    {
        if (m_OnCreateMethod)
            m_ScriptClass->invokeMethod(m_Instance, m_OnCreateMethod);
    }

    void ScriptInstance::invokeOnUpdate(float ts)
    {
        if (m_OnUpdateMethod)
        {
            void* param = &ts;
            m_ScriptClass->invokeMethod(m_Instance, m_OnUpdateMethod, &param);
        }
    }

    bool ScriptInstance::getFieldValueInternal(const std::string& name, void* buffer)
    {
        const auto& fields = m_ScriptClass->getFields();
        auto it = fields.find(name);
        if (it == fields.end())
            return false;

        const ScriptField& field = it->second;
        mono_field_get_value(m_Instance, field.classField, buffer);
        return true;
    }

    bool ScriptInstance::setFieldValueInternal(const std::string& name, const void* value)
    {
        const auto& fields = m_ScriptClass->getFields();
        auto it = fields.find(name);
        if (it == fields.end())
            return false;

        const ScriptField& field = it->second;
        mono_field_set_value(m_Instance, field.classField, (void*)value);
        return true;
    }

}
