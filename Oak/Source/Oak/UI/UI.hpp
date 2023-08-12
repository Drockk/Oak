#pragma once

#include <imgui/imgui.h>

namespace oak::ui {
    struct ScopedStyleColor
    {
        ScopedStyleColor() = default;

        ScopedStyleColor(ImGuiCol t_idx, ImVec4 t_color, bool t_predicate = true): m_Set(t_predicate)
        {
            if (t_predicate) {
                ImGui::PushStyleColor(t_idx, t_color);
            }
        }

        ScopedStyleColor(ImGuiCol t_idx, ImU32 t_color, bool t_predicate = true): m_Set(t_predicate)
        {
            if (t_predicate) {
                ImGui::PushStyleColor(t_idx, t_color);
            }
        }

        ~ScopedStyleColor()
        {
            if (m_Set) {
                ImGui::PopStyleColor();
            }
        }
    private:
        bool m_Set{ false };
    };
}
