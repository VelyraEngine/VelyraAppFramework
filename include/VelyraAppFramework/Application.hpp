#pragma once

#include <VelyraAppFramework/AppData.hpp>
#include <VelyraAppFramework/Layer.hpp>
#include <VelyraCore/Window/Window.hpp>
#include "LayoutEngine/LayoutEngine.hpp"

namespace Velyra::App {

    class Layer;

    struct VL_API ApplicationDesc {
        bool settingsEnableSave = true;
        bool useImPlot = false;
        bool saveImGuiWindowData = false; // Creates imgui.ini to store window settings
        std::string applicationName;
    };

    class VL_API Application {
    public:
        Application(const ApplicationDesc& appDesc, const ProgramArgs& programArgs);

        ~Application();

        template<typename T, typename... Args>
        void createAppLayer(Args&&... args) {
            const UP<Core::Context>& context = m_Window->getContext();
            if (m_AppLayer) {
                m_AppLayer->onDetach(*m_Window, *context);
            }
            m_AppLayer = createUP<T>(m_AppData, std::forward<Args>(args)...);
            m_AppLayer->onAttach(*m_Window, *context);
        }

        void run();

    private:

        void processEvents() const;

    private:
        LayoutEngine m_LayoutEngine;
        AppData m_AppData;
        UP<Layer> m_AppLayer;
        UP<Core::Window> m_Window;
    };

}
