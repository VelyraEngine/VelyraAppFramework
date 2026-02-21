#include <Pch.hpp>

#include <VelyraAppFramework/Widgets/RegexInput.hpp>

namespace Velyra::App::Widgets {

    void RegexInput::draw() {
        ImGui::InputTextWithHint(m_Label.c_str(), m_Hint.c_str(), &m_InputText);
        // Rebuild regex only when text changes
        if (ImGui::IsItemEdited()) {
            m_Regex.reset();
            m_RegexValid = true;

            if (!m_InputText.empty()) {
                try {
                    m_Regex.emplace(m_InputText, std::regex::ECMAScript | std::regex::icase);
                } catch (const std::regex_error&) {
                    m_RegexValid = false;
                }
            }
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Regex examples:\n200\n20[0-7]_status");
        }
        if (!m_RegexValid) {
            ImGui::TextColored(
                ImVec4(1.f, 0.3f, 0.3f, 1.f),
                "Invalid regex"
            );
        }
    }

    bool RegexInput::passRegex(const std::string &text) const {
        bool passes_regex = true;
        if (!m_InputText.empty()) {
            if (m_RegexValid && m_Regex.has_value()) {
                passes_regex = std::regex_search(text, *m_Regex);
            } else {
                // Fallback to simple substring search
                passes_regex = text.find(m_InputText) != std::string::npos;
            }
        }
        return passes_regex;
    }

    void RegexInput::reset() {
        m_InputText.clear();
        m_Regex.reset();
        m_RegexValid = true;

    }

}