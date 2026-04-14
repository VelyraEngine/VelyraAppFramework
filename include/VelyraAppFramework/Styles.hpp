#pragma once

#include <VelyraAppFramework/AppInclude.hpp>

namespace Velyra::App::Styles {

    static constexpr ImVec2 DefaultWindowSize = ImVec2(800.0f, 600.0f);

    static constexpr float ButtonHeight       = 25.0f; // In pixels

    static constexpr auto ColorRed            = ImVec4(0.8f, 0.0f, 0.0f, 1.0f);
    static constexpr auto ColorGreen          = ImVec4(0.0f, 0.8f, 0.0f, 1.0f);
    static constexpr auto ColorYellow         = ImVec4(0.8f, 0.8f, 0.0f, 1.0f);
    static constexpr auto ColorHighLight      = ImVec4(1.0f, 230.0f / 255.0f, 120.0f / 255.0f, 125.0f/ 255.0f);
    static constexpr auto ColorSoftHighLight  = ImVec4(1.0f, 230.0f / 255.0f, 120.0f / 255.0f, 50.0f / 255.0f);

}