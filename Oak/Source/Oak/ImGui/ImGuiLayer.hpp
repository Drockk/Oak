#pragma once

#include "Oak/Core/Layer.hpp"

#include "Oak/Events/ApplicationEvent.hpp"
#include "Oak/Events/KeyEvent.hpp"
#include "Oak/Events/MouseEvent.hpp"

namespace oak
{
    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer() = default;

        virtual void onAttach() override;
        virtual void onDetach() override;
        virtual void onEvent(Event& t_event) override;

        void begin();
        void end();

        void blockEvents(bool t_block) { m_BlockEvents = t_block; }

        void setDarkThemeColors();

        uint32_t getActiveWidgetID() const;
    private:
        bool m_BlockEvents{ true };
    };
}
