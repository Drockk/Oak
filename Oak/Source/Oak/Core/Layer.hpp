#pragma once

#include "Oak/Core/Base.hpp"
#include "Oak/Core/Timestep.hpp"
#include "Oak/Events/Event.hpp"

namespace oak
{
    class Layer
    {
    public:
        Layer(const std::string& t_name = "Layer");
        virtual ~Layer() = default;

        virtual void onAttach() {}
        virtual void onDetach() {}
        virtual void onUpdate(Timestep t_timestep) {}
        virtual void onImGuiRender() {}
        virtual void onEvent(Event& t_event) {}

        const std::string& getName() const
        {
            return m_DebugName;
        }

    protected:
        std::string m_DebugName{};
    };
}
