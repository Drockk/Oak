#include <Oak.hpp>
#include <Oak/Core/Entrypoint.hpp>

#include "EditorLayer.hpp"

class OakEd : public oak::Application
{
    OakEd(const oak::ApplicationSpecification& spec) : Application(spec)
    {
        pushLayer(new EditorLayer());
    }
};

std::unique_ptr<oak::Application> createApplication(ApplicationCommandLineArgs args)
{
    ApplicationSpecification spec;
    spec.Name = "OakEd";
    spec.CommandLineArgs = args;

    return std::make_unique<OakEd>(spec);
}
