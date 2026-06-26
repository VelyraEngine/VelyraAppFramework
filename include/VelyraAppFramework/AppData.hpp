#pragma once

#include <VelyraAppFramework/Settings.hpp>

#include <VelyraAppFramework/Widgets/Panel.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>
#include <VelyraAppFramework/LayoutEngine/Layout.hpp>

namespace Velyra::App {

    class Application;
    class LayoutEngine;

    class VL_API AppData {
    public:
        const ProgramArgs& programArgs;
        Settings settings;

    public:
        AppData(const ProgramArgs& args, const SettingsDesc& settingsDesc, LayoutEngine& layoutEngine);

        void registerLayout(Layout& layout) const;

        void setActiveLayout(LayoutID layoutID) const;

        LayoutID getActiveLayout() const;

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

        LayoutEngine& m_LayoutEngine;

        std::unordered_map<Widgets::PanelID, SP<Widgets::Panel>> m_Panels;
        std::unordered_map<Widgets::PopupID, SP<Widgets::Popup>> m_Popups;
    };

}