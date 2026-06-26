#include <Pch.hpp>

#include <VelyraAppFramework/Widgets/Popup.hpp>

namespace Velyra::App::Widgets {

    Popup::Popup(AppData& appData, const std::string &name, const PopupID popupID):
    m_AppData(appData),
    m_PopupID(popupID),
    m_Name(name){

    }

    void Popup::draw(Core::Window& window, Core::Context& context) {
        if (!m_Open) {
            return;
        }
        ImGui::OpenPopup(m_Name.c_str());
        bool isOpen = true;
        if (ImGui::BeginPopupModal(m_Name.c_str(), &isOpen)) {
            drawContent(window, context);
            ImGui::EndPopup();
        }
        if (!isOpen) {
            m_Open = false;
            reset();
        }
    }
}
