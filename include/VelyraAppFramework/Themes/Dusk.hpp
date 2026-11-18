#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <VelyraCore/ImGui/ImGui.hpp>

/**
 * @brief Dusk is a dark theme with an ink blue base and a soft periwinkle accent. It started as the combination of the
 *        FinanceManager (slate blue) and InvestManager (indigo) themes. Panels tile the window, so windows are square
 *        while cards, frames and popups have gently rounded corners.
 */
namespace Velyra::App::Themes::Dusk {

    /**
     * @brief Applies the colors and sizes of the theme. Call once after the ImGui context is created. Sizes are scaled
     *        by the DPI scale the framework applied (ImGuiStyle::FontScaleDpi), so the theme looks the same on every
     *        monitor. The ImPlot style is only touched when ImPlot is enabled on the context.
     * @param context - The context the ImGui (and optionally ImPlot) context was created for.
     */
    void apply(const Core::Context& context);

    // Semantic colors, use these instead of raw colors so an app stays consistent with the theme
    constexpr auto Accent     = ImVec4(0.47f, 0.55f, 1.00f, 1.00f);
    constexpr auto Positive   = ImVec4(0.30f, 0.79f, 0.50f, 1.00f);
    constexpr auto Negative   = ImVec4(0.96f, 0.37f, 0.37f, 1.00f);
    constexpr auto Warning    = ImVec4(0.94f, 0.69f, 0.27f, 1.00f);
    constexpr auto Muted      = ImVec4(0.56f, 0.60f, 0.68f, 1.00f);
    constexpr auto Text       = ImVec4(0.90f, 0.91f, 0.94f, 1.00f);
    constexpr auto CardBg     = ImVec4(0.12f, 0.14f, 0.18f, 1.00f);

    /**
     * @brief Series colors are meant for plots with several lines or bars that must stay distinguishable. They are
     *        balanced to have a similar brightness on the dark background.
     * @param index - The index of the series, wraps around when it is larger than the palette. Use a stable index
     *                (for example the position in a sorted list) so a series keeps its color everywhere.
     * @return A distinct color per series
     */
    ImVec4 seriesColor(int index);

}
