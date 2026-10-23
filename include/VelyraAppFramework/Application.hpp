#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <VelyraAppFramework/Settings.hpp>
#include <VelyraAppFramework/Layer.hpp>
#include <VelyraUtils/Logging/LoggingFwd.hpp>

namespace Velyra::App {

    class Layer;
    class AppData;

    struct VL_API ApplicationDesc {
        bool useImPlot = false;
        std::string applicationName;
        SettingsDesc settings;
    };

    class VL_API Application {
    public:
        Application(const ApplicationDesc& appDesc, const ProgramArgs& programArgs);

        ~Application();

        template<typename T, typename... Args>
        void createAppLayer(Args&&... args) {
            if (m_AppLayer) {
                m_AppLayer->onDetach();
            }
            m_AppLayer = createUP<T>(*m_AppData, std::forward<Args>(args)...);
            m_AppLayer->onAttach();
        }

        void setAppLayer(UP<Layer> layer) {
            if (m_AppLayer) {
                m_AppLayer->onDetach();
            }
            m_AppLayer = std::move(layer);
            m_AppLayer->onAttach();
        }

        Layer& getAppLayer() const {
            return *m_AppLayer;
        }

        void run();

    private:

        void createWindow(const ApplicationDesc& appDesc);

        void createContext() const;

        void createImGuiContext(const ApplicationDesc& appDesc) const;

        void createAppData(const ProgramArgs& programArgs);

        void processEvents() const;

        void destroyAppData();

        void destroyImGuiContext() const;

        void destroyContext() const;

        void destroyWindow();

    private:
        Utils::LogPtr m_Logger;

        Settings m_Settings;
        UP<AppData> m_AppData;
        UP<Layer> m_AppLayer;
        UP<Core::Window> m_Window;
    };

}
