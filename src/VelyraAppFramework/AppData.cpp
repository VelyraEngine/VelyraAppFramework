#include <Pch.hpp>
#include <LoggerNames.hpp>

#include <VelyraAppFramework/AppData.hpp>
#include <VelyraAppFramework/Layout.hpp>
#include <VelyraAppFramework/Widgets/Panel.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>

namespace Velyra::App {

    AppData::AppData(const ProgramArgs& args, Core::Window& window, Core::Context& context, Settings& settings) :
    m_Logger(Utils::getLogger(VL_APP_APP_DATA_LOGGER)),
    m_ProgramArgs(args),
    m_Window(window),
    m_Context(context),
    m_Settings(settings) {

    }

    AppData::~AppData() = default;

    const ProgramArgs& AppData::getProgramArgs() const {
        return m_ProgramArgs;
    }

    Core::Window& AppData::getWindow() const {
        return m_Window;
    }

    Core::Context& AppData::getContext() const {
        return m_Context;
    }

    Settings& AppData::getSettings() const {
        return m_Settings;
    }

    void AppData::registerLayout(const SP<Layout>& layout) {
        const LayoutID layoutID = layout->getID();
        if (m_Layouts.contains(layoutID)) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Layout with ID {} already registered. Overwriting!", layoutID);
        }
        m_Layouts[layoutID] = layout;
        if (m_ActiveLayout == NO_LAYOUT) {
            m_ActiveLayout = layoutID;
        }
    }

    void AppData::unregisterLayout(const LayoutID layoutID) {
        if (m_Layouts.erase(layoutID) == 0) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Layout with ID {} not found!", layoutID);
            return;
        }
        if (m_ActiveLayout == layoutID) {
            m_ActiveLayout = NO_LAYOUT;
        }
    }

    void AppData::setActiveLayout(const LayoutID layoutID) {
        if (layoutID != NO_LAYOUT && !m_Layouts.contains(layoutID)) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Layout with ID {} not found. Register a layout for this ID first!", layoutID);
            return;
        }
        m_ActiveLayout = layoutID;
    }

    LayoutID AppData::getActiveLayout() const {
        return m_ActiveLayout;
    }

    SP<Layout> AppData::getLayout(const LayoutID layoutID) const {
        const auto it = m_Layouts.find(layoutID);
        return it != m_Layouts.end() ? it->second : nullptr;
    }

    void AppData::addPopup(const SP<Widgets::Popup> &popup) {
        if (m_Popups.find(popup->getID()) != m_Popups.end()) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Popup with ID {} already exists!", popup->getID());
            return;
        }
        m_Popups[popup->getID()] = popup;
        SPDLOG_LOGGER_INFO(m_Logger, "Added popup with ID {}", popup->getID());
    }

    void AppData::openPopup(const Widgets::PopupID popupID) {
        for (const auto& [id, popup] : m_Popups) {
            const bool isOpen = (id == popupID);
            popup->setOpen(isOpen);
        }
    }

    void AppData::closePopup(const Widgets::PopupID popupID) {
        auto it = m_Popups.find(popupID);
        if (it != m_Popups.end()) {
            it->second->setOpen(false);
        }
        else {
            SPDLOG_LOGGER_ERROR(m_Logger, "Popup with ID {} not found!", popupID);
        }
    }

    SP<Widgets::Popup> AppData::getPopup(const Widgets::PopupID popupID) const {
        auto it = m_Popups.find(popupID);
        if (it != m_Popups.end()) {
            return it->second;
        }
        SPDLOG_LOGGER_ERROR(m_Logger, "Popup with ID {} not found!", popupID);
        return nullptr;
    }

    void AppData::addPanel(const SP<Widgets::Panel> &panel) {
        if (m_Panels.find(panel->getID()) != m_Panels.end()) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Panel with ID {} already exists!", panel->getID());
            return;
        }
        m_Panels[panel->getID()] = panel;
        SPDLOG_LOGGER_INFO(m_Logger, "Added panel with ID {}", panel->getID());
    }

    void AppData::removePanel(const Widgets::PanelID panelID) {
        auto it = m_Panels.find(panelID);
        if (it != m_Panels.end()) {
            m_Panels.erase(it);
            SPDLOG_LOGGER_INFO(m_Logger, "Removed panel with ID {}", panelID);
        }
        else {
            SPDLOG_LOGGER_ERROR(m_Logger, "Panel with ID {} not found!", panelID);
        }
    }

    SP<Widgets::Panel> AppData::getPanel(Widgets::PanelID panelID) const {
        auto it = m_Panels.find(panelID);
        if (it != m_Panels.end()) {
            return it->second;
        }
        SPDLOG_LOGGER_ERROR(m_Logger, "Panel with ID {} not found!", panelID);
        return nullptr;
    }

    void AppData::drawPopups() {
        // Iterate over a copy, drawing a popup can add or remove popups, which invalidates iterators of m_Popups
        const std::vector<SP<Widgets::Popup>> popups(std::views::values(m_Popups).begin(), std::views::values(m_Popups).end());
        for (const auto& popup: popups) {
            // Skip popups that were removed by a popup drawn before
            if (m_Popups.contains(popup->getID())) {
                popup->draw();
            }
        }
    }

    void AppData::drawPanels() {
        // Iterate over a copy, drawing a panel can add or remove panels, which invalidates iterators of m_Panels
        const std::vector<SP<Widgets::Panel>> panels(std::views::values(m_Panels).begin(), std::views::values(m_Panels).end());
        for (const auto& panel: panels) {
            // Skip panels that were removed by a panel drawn before
            if (!m_Panels.contains(panel->getID())) {
                continue;
            }
            const bool panelStillOpen = panel->draw();
            if (!panelStillOpen) {
                m_Panels.erase(panel->getID());
            }
        }
    }

    void AppData::clear() {
        m_ActiveLayout = NO_LAYOUT;
        m_Layouts.clear();
        m_Panels.clear();
        m_Popups.clear();
    }
}
