#pragma once

#include <VelyraAppFramework/AppInclude.hpp>

namespace Velyra::App {
    class AppData;
}

namespace Velyra::App::Widgets {

    using PanelID = UID;

    class Panel {
    public:
        Panel(AppData& appData, const std::string& name);

        virtual ~Panel() = default;

        void draw();

        PanelID getPanelID() const { return m_PanelID; }

        const std::string& getName() const { return m_Name; }

    protected:
        virtual void drawContent() = 0;

    protected:
        AppData& m_AppData;
        const PanelID m_PanelID;
        const std::string m_Name;

        bool m_Open = true;

    private:
        std::string m_ImGuiLabel;


    };

}
