#include <VelyraAppFramework/Themes/Analyst.hpp>
#include <VelyraAppFramework/Themes/ThemeUtils.hpp>

namespace Velyra::App::Themes::Analyst {

    namespace {

        // Base colors of the graphite
        constexpr ImVec4 Background = rgb(13, 16, 20);
        constexpr ImVec4 Surface    = rgb(19, 23, 29);
        constexpr ImVec4 Raised     = rgb(27, 32, 40);
        constexpr ImVec4 Border     = rgb(42, 49, 60);

        constexpr const char* ColormapName = "Analyst";

        // Colors for lines on a dark background, similar to the traces of an oscilloscope
        constexpr ImVec4 Palette[] = {
            rgb(34, 199, 222),  // Cyan
            rgb(255, 184, 56),  // Amber
            rgb(63, 207, 115),  // Green
            rgb(236, 100, 200), // Magenta
            rgb(102, 150, 255), // Blue
            rgb(255, 126, 74),  // Orange
            rgb(190, 225, 70),  // Lime
            rgb(170, 130, 255), // Violet
            rgb(255, 99, 110),  // Coral
            rgb(190, 205, 225), // Steel
        };
        constexpr int PaletteSize = sizeof(Palette) / sizeof(Palette[0]);

        void applyImGui() {
            ImGuiStyle& style = ImGui::GetStyle();
            // The framework scaled all sizes for the DPI of the monitor, keep that scale for our sizes
            const float scale = style.FontScaleDpi > 0.0f ? style.FontScaleDpi : 1.0f;

            // Compact, so a lot of rows fit on the screen
            style.WindowPadding           = ImVec2(8.0f * scale, 8.0f * scale);
            style.FramePadding            = ImVec2(6.0f * scale, 3.0f * scale);
            style.CellPadding             = ImVec2(6.0f * scale, 2.0f * scale);
            style.ItemSpacing             = ImVec2(6.0f * scale, 4.0f * scale);
            style.ItemInnerSpacing        = ImVec2(4.0f * scale, 4.0f * scale);
            style.IndentSpacing           = 16.0f * scale;
            style.ScrollbarSize           = 10.0f * scale;
            style.GrabMinSize             = 8.0f * scale;

            // Nearly square, like the bezel of an instrument
            style.WindowRounding          = 0.0f; // Panels tile the window
            style.ChildRounding           = 3.0f * scale;
            style.FrameRounding           = 2.0f * scale;
            style.PopupRounding           = 3.0f * scale;
            style.ScrollbarRounding       = 2.0f * scale;
            style.GrabRounding            = 2.0f * scale;
            style.TabRounding             = 2.0f * scale;

            style.WindowBorderSize        = 1.0f;
            style.ChildBorderSize         = 1.0f;
            style.PopupBorderSize         = 1.0f;
            style.FrameBorderSize         = 1.0f; // Thin outline around inputs and buttons
            style.TabBorderSize           = 0.0f;
            style.TabBarBorderSize        = 1.0f;
            style.SeparatorTextBorderSize = 1.0f;
            style.SeparatorTextAlign      = ImVec2(0.0f, 0.5f);
            style.SeparatorTextPadding    = ImVec2(8.0f * scale, 2.0f * scale);
            style.WindowMenuButtonPosition = ImGuiDir_None;
            style.WindowTitleAlign        = ImVec2(0.0f, 0.5f);

            ImVec4* colors = style.Colors;

            colors[ImGuiCol_Text]                 = Text;
            colors[ImGuiCol_TextDisabled]         = Muted;
            colors[ImGuiCol_WindowBg]             = Background;
            colors[ImGuiCol_ChildBg]              = withAlpha(Surface, 0.0f);
            colors[ImGuiCol_PopupBg]              = Surface;
            colors[ImGuiCol_Border]               = Border;
            colors[ImGuiCol_BorderShadow]         = Transparent;
            colors[ImGuiCol_FrameBg]              = Raised;
            colors[ImGuiCol_FrameBgHovered]       = rgb(36, 43, 54);
            colors[ImGuiCol_FrameBgActive]        = rgb(43, 52, 66);
            colors[ImGuiCol_TitleBg]              = Surface;
            colors[ImGuiCol_TitleBgActive]        = Raised;
            colors[ImGuiCol_TitleBgCollapsed]     = Surface;
            colors[ImGuiCol_MenuBarBg]            = rgb(10, 12, 15);
            colors[ImGuiCol_ScrollbarBg]          = withAlpha(Background, 0.0f);
            colors[ImGuiCol_ScrollbarGrab]        = rgb(45, 53, 65);
            colors[ImGuiCol_ScrollbarGrabHovered] = rgb(60, 71, 88);
            colors[ImGuiCol_ScrollbarGrabActive]  = withAlpha(Accent, 0.70f);
            colors[ImGuiCol_CheckMark]            = Accent;
            colors[ImGuiCol_SliderGrab]           = withAlpha(Accent, 0.75f);
            colors[ImGuiCol_SliderGrabActive]     = Accent;
            colors[ImGuiCol_Button]               = Raised;
            colors[ImGuiCol_ButtonHovered]        = rgb(38, 46, 58);
            colors[ImGuiCol_ButtonActive]         = rgb(46, 56, 72);
            colors[ImGuiCol_Header]               = withAlpha(Accent, 0.16f);
            colors[ImGuiCol_HeaderHovered]        = withAlpha(Accent, 0.26f);
            colors[ImGuiCol_HeaderActive]         = withAlpha(Accent, 0.36f);
            colors[ImGuiCol_Separator]            = Border;
            colors[ImGuiCol_SeparatorHovered]     = withAlpha(Accent, 0.60f);
            colors[ImGuiCol_SeparatorActive]      = Accent;
            colors[ImGuiCol_ResizeGrip]           = withAlpha(Accent, 0.0f);
            colors[ImGuiCol_ResizeGripHovered]    = withAlpha(Accent, 0.50f);
            colors[ImGuiCol_ResizeGripActive]     = Accent;
            colors[ImGuiCol_InputTextCursor]      = Warning;
            colors[ImGuiCol_Tab]                  = Surface;
            colors[ImGuiCol_TabHovered]           = withAlpha(Accent, 0.28f);
            colors[ImGuiCol_TabSelected]          = Raised;
            colors[ImGuiCol_TabSelectedOverline]  = Accent;
            colors[ImGuiCol_TabDimmed]            = Surface;
            colors[ImGuiCol_TabDimmedSelected]    = Raised;
            colors[ImGuiCol_PlotLines]            = Accent;
            colors[ImGuiCol_PlotLinesHovered]     = Warning;
            colors[ImGuiCol_PlotHistogram]        = withAlpha(Accent, 0.70f);
            colors[ImGuiCol_PlotHistogramHovered] = Accent;
            colors[ImGuiCol_TableHeaderBg]        = rgb(22, 27, 34);
            colors[ImGuiCol_TableBorderStrong]    = Border;
            colors[ImGuiCol_TableBorderLight]     = rgb(30, 36, 45);
            colors[ImGuiCol_TableRowBg]           = Transparent;
            colors[ImGuiCol_TableRowBgAlt]        = ImVec4(1.0f, 1.0f, 1.0f, 0.03f); // A visible zebra helps to follow long rows
            colors[ImGuiCol_TextSelectedBg]       = withAlpha(Accent, 0.30f);
            colors[ImGuiCol_TreeLines]            = rgb(52, 61, 75);
            colors[ImGuiCol_TextLink]             = Accent;
            colors[ImGuiCol_DragDropTarget]       = Warning;
            colors[ImGuiCol_NavCursor]            = Accent;
            colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.0f, 0.0f, 0.0f, 0.62f);
        }

        void applyImPlot() {
            const float scale = ImGui::GetStyle().FontScaleDpi > 0.0f ? ImGui::GetStyle().FontScaleDpi : 1.0f;

            ImPlotStyle& plotStyle = ImPlot::GetStyle();
            // Plot times are UTC and written as ISO 8601, so a timestamp in a plot can be compared with a log line
            plotStyle.UseLocalTime   = false;
            plotStyle.UseISO8601     = true;
            plotStyle.Use24HourClock = true;
            plotStyle.PlotPadding    = ImVec2(8.0f * scale, 8.0f * scale);
            plotStyle.LegendPadding  = ImVec2(8.0f * scale, 6.0f * scale);
            plotStyle.MinorAlpha     = 0.18f;
            // FitPadding is deliberately left at its default of zero. ImPlot::PlotShaded with a reference of
            // +-INFINITY uses the current axis limit as its reference, and on an auto fitting axis that limit is part
            // of the fit. With a padding it grows by that padding every frame and the axis keeps on scrolling.

            // Colors the series of the plots from the palette of the theme, unless a series sets its own color
            plotStyle.Colormap = registerColormap(ColormapName, Palette, PaletteSize);

            // The plot area is darker than the panel around it, like the screen of an oscilloscope
            plotStyle.Colors[ImPlotCol_FrameBg]       = Transparent;
            plotStyle.Colors[ImPlotCol_PlotBg]        = rgb(9, 11, 15, 0.92f);
            plotStyle.Colors[ImPlotCol_PlotBorder]    = Border;
            plotStyle.Colors[ImPlotCol_LegendBg]      = withAlpha(Surface, 0.88f);
            plotStyle.Colors[ImPlotCol_LegendBorder]  = Border;
            plotStyle.Colors[ImPlotCol_AxisText]      = rgb(158, 170, 184);
            plotStyle.Colors[ImPlotCol_AxisGrid]      = withAlpha(rgb(140, 155, 175), 0.14f);
            plotStyle.Colors[ImPlotCol_AxisTick]      = withAlpha(rgb(140, 155, 175), 0.45f);
            plotStyle.Colors[ImPlotCol_Selection]     = Warning;
            plotStyle.Colors[ImPlotCol_Crosshairs]    = withAlpha(Warning, 0.60f);
        }

    }

    void apply(const Core::Context& context) {
        applyImGui();

        if (context.isImPlotEnabled()) {
            applyImPlot();
        }
    }

}
