#include <VelyraAppFramework/Themes/Dusk.hpp>
#include <VelyraAppFramework/Themes/ThemeUtils.hpp>

namespace Velyra::App::Themes::Dusk {

    namespace {

        constexpr const char* ColormapName = "Dusk";

        constexpr ImVec4 Palette[] = {
            rgb(153, 133, 255), // Violet
            rgb(64, 196, 186),  // Teal
            rgb(242, 178, 72),  // Amber
            rgb(240, 110, 170), // Pink
            rgb(92, 170, 245),  // Sky
            rgb(160, 210, 90),  // Lime
            rgb(245, 130, 80),  // Orange
            rgb(110, 120, 240), // Indigo
            rgb(200, 150, 230), // Lilac
            rgb(80, 200, 130),  // Green
        };
        constexpr int PaletteSize = sizeof(Palette) / sizeof(Palette[0]);

        void applyImGui() {
            ImGuiStyle& style = ImGui::GetStyle();
            // The framework scaled all sizes for the DPI of the monitor, keep that scale for our sizes
            const float scale = style.FontScaleDpi > 0.0f ? style.FontScaleDpi : 1.0f;

            style.WindowPadding           = ImVec2(15.0f * scale, 13.0f * scale);
            style.FramePadding            = ImVec2(8.5f * scale, 4.5f * scale);
            style.CellPadding             = ImVec2(8.5f * scale, 4.5f * scale);
            style.ItemSpacing             = ImVec2(8.0f * scale, 7.5f * scale);
            style.ItemInnerSpacing        = ImVec2(6.0f * scale, 4.0f * scale);
            style.ScrollbarSize           = 11.5f * scale;
            style.GrabMinSize             = 10.0f * scale;

            style.WindowRounding          = 0.0f; // Panels tile the window
            style.ChildRounding           = 7.0f * scale;
            style.FrameRounding           = 5.0f * scale;
            style.PopupRounding           = 7.0f * scale;
            style.ScrollbarRounding       = 6.0f * scale;
            style.GrabRounding            = 5.0f * scale;
            style.TabRounding             = 5.0f * scale;

            style.WindowBorderSize        = 1.0f;
            style.ChildBorderSize         = 1.0f;
            style.PopupBorderSize         = 1.0f;
            style.FrameBorderSize         = 0.0f;
            style.SeparatorTextBorderSize = 1.0f;
            style.SeparatorTextPadding    = ImVec2(0.0f, 3.0f * scale);
            style.WindowMenuButtonPosition = ImGuiDir_None;
            style.WindowTitleAlign        = ImVec2(0.5f, 0.5f);

            ImVec4* colors = style.Colors;
            const ImVec4 background = rgb(19, 21, 28);
            const ImVec4 surface    = rgb(26, 29, 38);
            const ImVec4 raised     = rgb(34, 38, 50);
            const ImVec4 border     = rgb(44, 48, 61);

            colors[ImGuiCol_Text]                 = Text;
            colors[ImGuiCol_TextDisabled]         = Muted;
            colors[ImGuiCol_WindowBg]             = background;
            colors[ImGuiCol_ChildBg]              = withAlpha(surface, 0.0f);
            colors[ImGuiCol_PopupBg]              = surface;
            colors[ImGuiCol_Border]               = border;
            colors[ImGuiCol_BorderShadow]         = Transparent;
            colors[ImGuiCol_FrameBg]              = raised;
            colors[ImGuiCol_FrameBgHovered]       = rgb(45, 50, 66);
            colors[ImGuiCol_FrameBgActive]        = rgb(53, 59, 77);
            colors[ImGuiCol_TitleBg]              = surface;
            colors[ImGuiCol_TitleBgActive]        = raised;
            colors[ImGuiCol_TitleBgCollapsed]     = surface;
            colors[ImGuiCol_MenuBarBg]            = rgb(15, 16, 21);
            colors[ImGuiCol_ScrollbarBg]          = withAlpha(background, 0.0f);
            colors[ImGuiCol_ScrollbarGrab]        = rgb(52, 57, 73);
            colors[ImGuiCol_ScrollbarGrabHovered] = rgb(66, 73, 93);
            colors[ImGuiCol_ScrollbarGrabActive]  = rgb(80, 88, 112);
            colors[ImGuiCol_CheckMark]            = Accent;
            colors[ImGuiCol_SliderGrab]           = withAlpha(Accent, 0.8f);
            colors[ImGuiCol_SliderGrabActive]     = Accent;
            colors[ImGuiCol_Button]               = raised;
            colors[ImGuiCol_ButtonHovered]        = rgb(49, 55, 72);
            colors[ImGuiCol_ButtonActive]         = rgb(59, 66, 87);
            colors[ImGuiCol_Header]               = withAlpha(Accent, 0.20f);
            colors[ImGuiCol_HeaderHovered]        = withAlpha(Accent, 0.30f);
            colors[ImGuiCol_HeaderActive]         = withAlpha(Accent, 0.40f);
            colors[ImGuiCol_Separator]            = border;
            colors[ImGuiCol_SeparatorHovered]     = withAlpha(Accent, 0.6f);
            colors[ImGuiCol_SeparatorActive]      = Accent;
            colors[ImGuiCol_ResizeGrip]           = withAlpha(Accent, 0.0f);
            colors[ImGuiCol_ResizeGripHovered]    = withAlpha(Accent, 0.5f);
            colors[ImGuiCol_ResizeGripActive]     = Accent;
            colors[ImGuiCol_Tab]                  = surface;
            colors[ImGuiCol_TabHovered]           = withAlpha(Accent, 0.30f);
            colors[ImGuiCol_TabSelected]          = raised;
            colors[ImGuiCol_TabSelectedOverline]  = Accent;
            colors[ImGuiCol_TabDimmed]            = surface;
            colors[ImGuiCol_TabDimmedSelected]    = raised;
            colors[ImGuiCol_PlotHistogram]        = withAlpha(Accent, 0.75f);
            colors[ImGuiCol_TableHeaderBg]        = rgb(28, 32, 42);
            colors[ImGuiCol_TableBorderStrong]    = border;
            colors[ImGuiCol_TableBorderLight]     = rgb(34, 38, 49);
            colors[ImGuiCol_TableRowBg]           = Transparent;
            colors[ImGuiCol_TableRowBgAlt]        = ImVec4(1.0f, 1.0f, 1.0f, 0.022f);
            colors[ImGuiCol_TextSelectedBg]       = withAlpha(Accent, 0.35f);
            colors[ImGuiCol_TextLink]             = Accent;
            colors[ImGuiCol_NavCursor]            = Accent;
            colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.0f, 0.0f, 0.0f, 0.58f);
        }

        void applyImPlot() {
            const float scale = ImGui::GetStyle().FontScaleDpi > 0.0f ? ImGui::GetStyle().FontScaleDpi : 1.0f;

            const ImVec4 background = rgb(19, 21, 28);
            const ImVec4 surface    = rgb(26, 29, 38);
            const ImVec4 border     = rgb(44, 48, 61);

            ImPlotStyle& plotStyle = ImPlot::GetStyle();
            // Plot times are treated as UTC, so the axes show the same dates regardless of the time zone
            plotStyle.UseLocalTime   = false;
            plotStyle.UseISO8601     = false;
            plotStyle.Use24HourClock = true;
            plotStyle.PlotPadding    = ImVec2(6.0f * scale, 6.0f * scale);
            plotStyle.LegendPadding  = ImVec2(8.0f * scale, 6.0f * scale);

            // Colors the series of the plots from the palette of the theme, unless a series sets its own color
            plotStyle.Colormap = registerColormap(ColormapName, Palette, PaletteSize);

            plotStyle.Colors[ImPlotCol_FrameBg]     = withAlpha(background, 0.0f);
            plotStyle.Colors[ImPlotCol_PlotBg]       = withAlpha(surface, 0.58f);
            plotStyle.Colors[ImPlotCol_PlotBorder]   = withAlpha(border, 0.0f);
            plotStyle.Colors[ImPlotCol_LegendBg]     = withAlpha(surface, 0.85f);
            plotStyle.Colors[ImPlotCol_LegendBorder] = border;
            plotStyle.Colors[ImPlotCol_AxisGrid]     = withAlpha(rgb(120, 128, 155), 0.17f);
        }

    }

    ImVec4 seriesColor(const int index) {
        return Palette[((index % PaletteSize) + PaletteSize) % PaletteSize];
    }

    void apply(const Core::Context& context) {
        applyImGui();

        if (context.isImPlotEnabled()) {
            applyImPlot();
        }
    }

}
