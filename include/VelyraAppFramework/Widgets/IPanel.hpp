#pragma once

#include <VelyraUtils/Types/Types.hpp>

namespace Velyra::Core {
    class Window;
    class Context;
}

namespace Velyra::App {
    class AppData;
}

namespace Velyra::App::Widgets {

    class IPanel {
    public:
        IPanel(AppData& appData, const std::string& name);

        virtual ~IPanel() = default;

    protected:

        virtual void drawContent(Core::Window& window, Core::Context& context) = 0;

    protected:
        AppData& m_AppData;
        const std::string m_Name;

    };

}