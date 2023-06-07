#include <Oak.hpp>
#include <Core/Entrypoint.hpp>

#include <memory>

class SandboxApplication final : public oak::Application
{
public:
    SandboxApplication() = default;
    ~SandboxApplication() = default;
};

std::unique_ptr<oak::Application> createApplication()
{
    return std::make_unique<SandboxApplication>();
}
