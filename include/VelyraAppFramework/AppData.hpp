#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <VelyraAppFramework/Settings.hpp>

#include <VelyraAppFramework/LayoutEngine/LayoutEngine.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>
#include <VelyraAppFramework/Widgets/Panel.hpp>

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

        void addPopup(const SP<Widgets::Popup> &popup) {
            m_Popups.push_back(popup);
        }

        void addPanel(const SP<Widgets::Panel> &panel) {
            m_Panels[panel->getPanelID()] = panel;
        }

        Size getPanelCount() const {
            return m_Panels.size();
        }

    private:
        friend Application;
        friend Widgets::Panel;

        Widgets::PanelID m_ToRemovePanelID = 0;
        std::unordered_map<Widgets::PanelID, SP<Widgets::Panel>> m_Panels;

        std::vector<SP<Widgets::Popup>> m_Popups;
    };

}