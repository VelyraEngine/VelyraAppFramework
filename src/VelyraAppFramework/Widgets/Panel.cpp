#include <Pch.hpp>

#include <VelyraAppFramework/AppData.hpp>
#include <VelyraAppFramework/Widgets/Panel.hpp>
#include <VelyraAppFramework/Styles.hpp>

namespace Velyra::App::Widgets {

    PanelID generatePanelID() {
        static PanelID currentID = 1; // Start from 1 to avoid using 0 as a valid ID (0 can be used to indicate "no panel")
        return currentID++;
    }

    Panel::Panel(AppData& appData, const std::string& name):
    m_AppData(appData),
    m_PanelID(generatePanelID()),
    m_Name(name) {
        m_ImGuiLabel = name + "###Panel" + std::to_string(m_PanelID);
    }

    bool Panel::draw(Core::Window& window, Core::Context& context) {
        ImGui::SetNextWindowSize(Styles::DefaultWindowSize, ImGuiCond_Once);
        ImGui::Begin(m_ImGuiLabel.c_str(), &m_Open);
        drawContent(window, context);
        ImGui::End();

        return m_Open;
    }

} // namespace Velyra::App::Widgets