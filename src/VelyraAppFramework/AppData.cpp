#include <Pch.hpp>

#include <VelyraAppFramework/AppData.hpp>
#include "../../include/VelyraAppFramework/LayoutEngine/LayoutEngine.hpp"

namespace Velyra::App {

    AppData::AppData(const ProgramArgs& args, const SettingsDesc& settingsDesc, LayoutEngine& layoutEngine) :
    programArgs(args),
    settings(settingsDesc),
    m_LayoutEngine(layoutEngine){
    }

    void AppData::registerLayout(Layout& layout) const {
        m_LayoutEngine.registerLayout(layout);
    }

    void AppData::setActiveLayout(const LayoutID layoutID) const {
        m_LayoutEngine.setActiveLayout(layoutID);
    }

    LayoutID AppData::getActiveLayout() const {
        return m_LayoutEngine.getActiveLayout();
    }

    void AppData::addPopup(const SP<Widgets::Popup> &popup) {
        m_Popups[popup->getID()] = popup;
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
    }

    void AppData::drawPopups(Core::Window &window, Core::Context &context) {
        for (const auto &popup: m_Popups | std::views::values) {
            popup->draw(window, context);
        }
    }

    void AppData::drawPanels(Core::Window &window, Core::Context &context) {
        Widgets::PanelID toRemove = 0;
        for (const auto &panel: m_Panels | std::views::values) {
            if (!panel->draw(window, context)) {
                toRemove = panel->getPanelID();
            }
        }
        if (toRemove != 0) {
            m_Panels.erase(toRemove);
        }
    }
}
