#include <Pch.hpp>

#include <VelyraAppFramework/Widgets/IPanel.hpp>

namespace Velyra::App::Widgets {

    IPanel::IPanel(AppData& appData, const std::string& name):
    m_AppData(appData),
    m_Name(name) {
    }

}
