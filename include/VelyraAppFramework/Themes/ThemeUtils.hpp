#pragma once

#include <VelyraCore/ImGui/ImGui.hpp>

/**
 * @brief Small helpers that are shared by the themes, they are also handy to write the colors of an app on top of a theme.
 */
namespace Velyra::App::Themes {

    /**
     * @return The color for 8 bit channels, the alpha is in the range [0, 1]
     */
    constexpr ImVec4 rgb(const int r, const int g, const int b, const float a = 1.0f) {
        return {static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, a};
    }

    /**
     * @return The color with the alpha replaced
     */
    constexpr ImVec4 withAlpha(const ImVec4 color, const float alpha) {
        return {color.x, color.y, color.z, alpha};
    }

    constexpr ImVec4 Transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    /**
     * @brief Registers the palette as a named ImPlot colormap, plots color their series from the active colormap
     *        (ImPlotStyle::Colormap). A colormap name can only be registered once per ImPlot context, so when the
     *        name is already known (for example because a theme is applied a second time) the existing colormap is
     *        returned instead of registering it again. Needs a current ImPlot context.
     * @param name - The name of the colormap, it is shown in the ImPlot colormap selector.
     * @param palette - The colors, the data is copied by ImPlot.
     * @param count - The number of colors in the palette.
     * @return The index of the colormap, assign it to ImPlotStyle::Colormap to use it.
     */
    inline ImPlotColormap registerColormap(const char* name, const ImVec4* palette, const int count) {
        const ImPlotColormap existing = ImPlot::GetColormapIndex(name);
        if (existing >= 0) {
            return existing;
        }
        return ImPlot::AddColormap(name, palette, count);
    }

}
