#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <VelyraUtils/Logging/LoggingFwd.hpp>

#include <unordered_map>

namespace Velyra::App {

    class Application;
    class Settings;
    class Layout;

    namespace Widgets {
        class Popup;
        class Panel;
    }

    class VL_API AppData {
    public:
        AppData(const ProgramArgs& args, Core::Window& window, Core::Context& context, Settings& settings);

        ~AppData();

        const ProgramArgs& getProgramArgs() const;

        Core::Window& getWindow() const;

        Core::Context& getContext() const;

        Settings& getSettings() const;

        /**
         * @brief Registers a layout, so it can be made active with setActiveLayout(). AppData shares ownership of the
         *        layout until it is unregistered or the app layer is replaced. The first registered layout becomes the
         *        active layout.
         */
        void registerLayout(const SP<Layout>& layout);

        void unregisterLayout(LayoutID layoutID);

        void setActiveLayout(LayoutID layoutID);

        /**
         * @return The ID of the active layout, NO_LAYOUT if no layout is active
         */
        LayoutID getActiveLayout() const;

        /**
         * @return The registered layout with the given ID, nullptr if there is none
         */
        SP<Layout> getLayout(LayoutID layoutID) const;

        void addPopup(const SP<Widgets::Popup> &popup);

        void openPopup(Widgets::PopupID popupID);

        void closePopup(Widgets::PopupID popupID);

        SP<Widgets::Popup> getPopup(Widgets::PopupID popupID) const;

        void addPanel(const SP<Widgets::Panel> &panel);

        void removePanel(Widgets::PanelID panelID);

        SP<Widgets::Panel> getPanel(Widgets::PanelID panelID) const;

        const std::unordered_map<Widgets::PanelID, SP<Widgets::Panel>>& getPanels() const {
            return m_Panels;
        }

        const std::unordered_map<Widgets::PopupID, SP<Widgets::Popup>>& getPopups() const {
            return m_Popups;
        }

        /**
         * @brief Draws all popups. Popups may add or remove popups while being drawn, new popups are drawn from the
         *        next frame on.
         */
        void drawPopups();

        /**
         * @brief Draws all panels and removes the ones that were closed. Panels may add or remove panels while being
         *        drawn, new panels are drawn from the next frame on.
         */
        void drawPanels();

    private:
        friend Application;

        /**
         * @brief Removes all layouts, panels and popups, called when the app layer that registered them is detached.
         */
        void clear();

    private:
        Utils::LogPtr m_Logger;

        const ProgramArgs& m_ProgramArgs;

        Core::Window& m_Window;
        Core::Context& m_Context;
        Settings& m_Settings;

        LayoutID m_ActiveLayout = NO_LAYOUT;
        std::unordered_map<LayoutID, SP<Layout>> m_Layouts;

        std::unordered_map<Widgets::PanelID, SP<Widgets::Panel>> m_Panels;
        std::unordered_map<Widgets::PopupID, SP<Widgets::Popup>> m_Popups;
    };

}