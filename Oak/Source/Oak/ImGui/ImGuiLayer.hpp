#pragma once

#include "Oak/Core/Layer.hpp"

#include "Oak/Events/ApplicationEvent.hpp"
#include "Oak/Events/KeyEvent.hpp"
#include "Oak/Events/MouseEvent.hpp"

namespace oak {
    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer() = default;

        void onAttach() override;
        void onDetach() override;
        void onEvent(Event& e) override;

        void begin();
        void end();

        void blockEvents(bool block) { m_BlockEvents = block; }
        
        void setDarkThemeColors();

        uint32_t getActiveWidgetID() const;

    private:
        bool m_BlockEvents{ true };
    };
}
