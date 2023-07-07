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

oak::Scope<oak::Application> createApplication(oak::ApplicationCommandLineArgs args)
{
    oak::ApplicationSpecification spec;
    spec.name = "OakEd";
    spec.commandLineArgs = args;

    return oak::createScope<OakEd>(spec);
}
