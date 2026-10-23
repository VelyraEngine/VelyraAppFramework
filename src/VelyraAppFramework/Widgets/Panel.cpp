#include <Pch.hpp>

#include <VelyraAppFramework/AppData.hpp>
#include <VelyraAppFramework/Styles.hpp>
#include <VelyraAppFramework/Widgets/Panel.hpp>

namespace Velyra::App::Widgets {

    Panel::Panel(AppData& appData, const std::string& name, const PanelID panelID):
    IPanel(appData, name),
    m_PanelID(panelID) {
        m_ImGuiLabel = name + "###Panel" + std::to_string(m_PanelID);
    }

    bool Panel::draw() {
        ImGui::SetNextWindowSize(Styles::DefaultWindowSize, ImGuiCond_Once);
        ImGui::Begin(m_ImGuiLabel.c_str(), &m_Open);
        drawContent();
        ImGui::End();

        return m_Open;
    }

} // namespace Velyra::App::Widgets