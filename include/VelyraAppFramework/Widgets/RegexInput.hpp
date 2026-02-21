#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <optional>
#include <regex>

namespace Velyra::App::Widgets {

    class RegexInput {
    public:
        explicit RegexInput(const std::string& label, const std::string& hint = ""):
        m_Label(label),
        m_Hint(hint),
        m_InputText(""),
        m_Regex(std::nullopt) {}

        void draw();

        bool passRegex(const std::string& text) const;

        void reset();

        const std::optional<std::regex>& getRegex() const {
            return m_Regex;
        }

        bool isRegexValid() const {
            return m_RegexValid && m_Regex.has_value();
        }

        bool hasInput() const {
            return !m_InputText.empty();
        }

        const std::string& getInput() const {
            return m_InputText;
        }

    private:
        const std::string m_Label;
        const std::string m_Hint;

        std::string m_InputText;
        std::optional<std::regex> m_Regex;
        bool m_RegexValid = true;
    };

}