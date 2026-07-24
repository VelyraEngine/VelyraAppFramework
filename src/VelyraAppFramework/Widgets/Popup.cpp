#include <Pch.hpp>

#include <VelyraAppFramework/Widgets/Popup.hpp>
#include <VelyraAppFramework/Styles.hpp>

namespace Velyra::App::Widgets {

    Popup::Popup(AppData& appData, const std::string &name, const PopupID popupID):
    IPanel(appData, name),
    m_PopupID(popupID){

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

    void Popup::drawConfirmationButtons(Core::Window &window, Core::Context &context, const std::string &confirmButtonLabel,
        const std::string &cancelButtonLabel, const bool valid) {

        ImGui::BeginDisabled(!valid);
        ImGui::PushStyleColor(ImGuiCol_Button, Styles::ColorGreen);
        if (ImGui::Button(confirmButtonLabel.c_str())) {
            onConfirm(window, context);
            reset();
            setOpen(false);
        }
        ImGui::PopStyleColor();
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, Styles::ColorRed);
        if (ImGui::Button(cancelButtonLabel.c_str())) {
            onCancel(window, context);
            reset();
            setOpen(false);
        }
        ImGui::PopStyleColor();

    }
}
