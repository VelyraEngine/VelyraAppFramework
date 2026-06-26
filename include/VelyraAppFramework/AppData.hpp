#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <VelyraAppFramework/Settings.hpp>

#include <VelyraAppFramework/LayoutEngine/LayoutEngine.hpp>
#include <VelyraAppFramework/Widgets/Panel.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>

namespace Velyra::App {

    class Application;

    class VL_API AppData {
    public:
        const ProgramArgs& programArgs;
        Settings settings;
        LayoutEngine layoutEngine;

    public:
        AppData(const ProgramArgs& args, const SettingsDesc& settingsDesc) : programArgs(args), settings(settingsDesc) {
        }

        void addPopup(const SP<Widgets::Popup> &popup);

        void openPopup(Widgets::PopupID popupID);

        void closePopup(Widgets::PopupID popupID);

        void addPanel(const SP<Widgets::Panel> &panel) {
            m_Panels[panel->getPanelID()] = panel;
        }

        Size getPanelCount() const {
            return m_Panels.size();
        }

    private:

        void drawPopups(Core::Window& window, Core::Context& context);

        void drawPanels(Core::Window& window, Core::Context& context);

    private:
        friend Application;

        std::unordered_map<Widgets::PanelID, SP<Widgets::Panel>> m_Panels;
        std::unordered_map<Widgets::PopupID, SP<Widgets::Popup>> m_Popups;
    };

}