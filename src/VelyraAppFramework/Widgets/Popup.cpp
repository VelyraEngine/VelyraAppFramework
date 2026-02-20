#include <Pch.hpp>

#include <VelyraAppFramework/Widgets/Popup.hpp>

namespace Velyra::App::Widgets {

    Popup::Popup(const std::string &name):
    m_Name(name){

    }

    void Popup::draw() {
        if (!m_Open) {
            return;
        }
        ImGui::OpenPopup(m_Name.c_str());
        bool isOpen = true;
        if (ImGui::BeginPopupModal(m_Name.c_str(), &isOpen)) {
            drawContent();
            ImGui::EndPopup();
        }
        if (!isOpen) {
            m_Open = false;
            reset();
        }
    }
}
