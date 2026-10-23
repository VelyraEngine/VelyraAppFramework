#pragma once

#include <vector>
#include <string_view>
#include <VelyraUtils/IO/IncludeFileSystem.hpp>
#include <VelyraUtils/Types/Types.hpp>
#include <VelyraUtils/ExportUtils.hpp>

namespace Velyra::Core {
    class Window;
    class Context;
    struct Event;
}

namespace Velyra::App {
    namespace Widgets {
        using PanelID = UID;
        using PopupID = UID;
    }

    using ProgramArgs = std::vector<std::string_view>;

    using LayoutID = UID;

    constexpr LayoutID NO_LAYOUT = 0;

    VL_API fs::path getAppdataPath(const std::string& app_name);

    VL_API Widgets::PanelID generatePanelID();

    VL_API Widgets::PopupID generatePopupID();

    VL_API LayoutID generateLayoutID();

}