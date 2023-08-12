#include <Oak.hpp>
#include <Oak/Core/EntryPoint.hpp>

#include "EditorLayer.hpp"

class OakEd final : public oak::Application
{
public:
    OakEd(const oak::ApplicationSpecification& spec): Application(spec) {
        pushLayer(new EditorLayer());
    }
};


oak::Application* createApplication(oak::ApplicationCommandLineArgs args)
{
    oak::ApplicationSpecification spec;
    spec.name = "OakEd";
    spec.commandLineArgs = args;

    return new OakEd(spec);
}
