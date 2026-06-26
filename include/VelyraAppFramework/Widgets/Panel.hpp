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

    using PanelID = UID;

    class Panel {
    public:
        Panel(AppData& appData, const std::string& name);

        virtual ~Panel() = default;

        /**
         * @brief Draws the panel
         * @param window
         * @param context
         * @return false if the panel is closed, true if it is still open
         */
        bool draw(Core::Window& window, Core::Context& context);

        PanelID getPanelID() const { return m_PanelID; }

        const std::string& getName() const { return m_Name; }

        PanelID getID() const { return m_PanelID; }

    protected:
        virtual void drawContent(Core::Window& window, Core::Context& context) = 0;

    protected:
        AppData& m_AppData;
        const PanelID m_PanelID;
        const std::string m_Name;

        bool m_Open = true;

    private:
        std::string m_ImGuiLabel;


    };

}
