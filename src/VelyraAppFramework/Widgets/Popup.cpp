#include <../Shared/Pch.hpp>

#include <VelyraAppFramework/Styles.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>

namespace Velyra::App::Widgets {

    Popup::Popup(AppData& appData, const std::string &name, const PopupID popupID):
    IPanel(appData, name),
    m_PopupID(popupID){

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
            setOpen(false);
            reset();
        }
    }

    void Popup::setOpen(const bool open) {
        m_Open = open;
        if (open) {
            onOpen();
        }
        else {
            onClose();
        }
    }

    void Popup::drawConfirmationButtons(const std::string &confirmButtonLabel, const std::string &cancelButtonLabel, const bool valid) {

        ImGui::BeginDisabled(!valid);
        ImGui::PushStyleColor(ImGuiCol_Button, Styles::ColorGreen);
        if (ImGui::Button(confirmButtonLabel.c_str())) {
            onConfirm();
            reset();
            setOpen(false);
        }
        ImGui::PopStyleColor();
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, Styles::ColorRed);
        if (ImGui::Button(cancelButtonLabel.c_str())) {
            onCancel();
            reset();
            setOpen(false);
        }
        ImGui::PopStyleColor();

    }
}
