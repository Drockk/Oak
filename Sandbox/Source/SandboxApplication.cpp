#include <Oak.hpp>
#include <Oak/Core/Entrypoint.hpp>

#include <memory>

class SandboxApplication final : public oak::Application
{
public:
    SandboxApplication() = default;
    ~SandboxApplication() = default;
};

oak::Scope<oak::Application> createApplication()
{
    return oak::createScope<SandboxApplication>();
}
