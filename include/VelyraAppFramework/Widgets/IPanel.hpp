#pragma once

#include <VelyraAppFramework/AppInclude.hpp>

namespace Velyra::App {
    class AppData;
}

namespace Velyra::App::Widgets {

    class IPanel {
    public:
        IPanel(AppData& appData, const std::string& name);

        virtual ~IPanel() = default;

    protected:

        virtual void drawContent() = 0;

    protected:
        AppData& m_AppData;
        const std::string m_Name;

    };

}