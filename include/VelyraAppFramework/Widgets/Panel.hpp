#pragma once

#include <VelyraAppFramework/Widgets/IPanel.hpp>

namespace Velyra::App::Widgets {

    using PanelID = UID;

    class Panel: public IPanel {
    public:
        Panel(AppData& appData, const std::string& name);

        ~Panel() override = default;

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
        const PanelID m_PanelID;

        bool m_Open = true;

    private:
        std::string m_ImGuiLabel;


    };

}
