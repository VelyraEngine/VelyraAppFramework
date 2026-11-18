#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <VelyraCore/ImGui/ImGui.hpp>

/**
 * @brief Analyst is a dark, dense theme for tools that inspect a lot of data: logs, tables, signals and diffs. It looks
 *        like the screen of an instrument: a near black graphite base, a cyan accent for the interface and amber that
 *        is reserved for "this is what you are looking for" (search matches, selections, cursors). Spacing is compact
 *        so more rows fit on the screen, corners are nearly square and frames have a thin border.
 *
 *        Besides the ImGui style the theme registers an ImPlot colormap, so plots color their series with a palette
 *        that is tuned for lines on a dark background without any extra work.
 */
namespace Velyra::App::Themes::Analyst {

    /**
     * @brief Applies the colors and sizes of the theme. Call once after the ImGui context is created. Sizes are scaled
     *        by the DPI scale the framework applied (ImGuiStyle::FontScaleDpi), so the theme looks the same on every
     *        monitor. The ImPlot style and colormap are only touched when ImPlot is enabled on the context.
     * @param context - The context the ImGui (and optionally ImPlot) context was created for.
     */
    void apply(const Core::Context& context);

    // Semantic colors, use these instead of raw colors so an app stays consistent with the theme
    constexpr auto Accent        = ImVec4(0.13f, 0.78f, 0.87f, 1.00f); // Cyan
    constexpr auto Positive      = ImVec4(0.25f, 0.81f, 0.45f, 1.00f); // Phosphor green
    constexpr auto Negative      = ImVec4(0.95f, 0.33f, 0.33f, 1.00f);
    constexpr auto Warning       = ImVec4(1.00f, 0.72f, 0.22f, 1.00f); // Amber
    constexpr auto Info          = ImVec4(0.40f, 0.65f, 1.00f, 1.00f);
    constexpr auto Muted         = ImVec4(0.49f, 0.54f, 0.60f, 1.00f);
    constexpr auto Text          = ImVec4(0.82f, 0.85f, 0.89f, 1.00f); // A bit dimmer than white, easier on the eyes for long sessions
    constexpr auto CardBg        = ImVec4(0.075f, 0.09f, 0.114f, 1.00f);

    // Highlights for the thing the user is looking for, translucent so the text below stays readable
    constexpr auto Highlight     = ImVec4(1.00f, 0.84f, 0.32f, 0.42f); // Selected rows, current search match
    constexpr auto SoftHighlight = ImVec4(1.00f, 0.84f, 0.32f, 0.18f); // Hovered rows, other search matches
    constexpr auto LineNumber    = ImVec4(0.36f, 0.41f, 0.47f, 1.00f);

    // Diff backgrounds, the change within a modified line gets the stronger color
    constexpr auto DiffAdded     = ImVec4(0.25f, 0.81f, 0.45f, 0.18f);
    constexpr auto DiffRemoved   = ImVec4(0.95f, 0.33f, 0.33f, 0.18f);
    constexpr auto DiffModified  = ImVec4(1.00f, 0.72f, 0.22f, 0.13f);
    constexpr auto DiffChange    = ImVec4(1.00f, 0.72f, 0.22f, 0.38f);
    constexpr auto DiffMissing   = ImVec4(0.50f, 0.55f, 0.60f, 0.07f); // The empty side of an added or removed line

}
