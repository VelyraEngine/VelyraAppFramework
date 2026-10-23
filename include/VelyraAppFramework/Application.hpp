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

        /**
         * @brief Creates the app layer, replacing the current one. The layouts, panels and popups registered by the
         *        current layer are removed, since they usually reference the current layer.
         */
        template<typename T, typename... Args>
        void createAppLayer(Args&&... args) {
            detachAppLayer();
            m_AppLayer = createUP<T>(*m_AppData, std::forward<Args>(args)...);
            m_AppLayer->onAttach();
        }

        void setAppLayer(UP<Layer> layer) {
            detachAppLayer();
            m_AppLayer = std::move(layer);
            m_AppLayer->onAttach();
        }

        Layer& getAppLayer() const {
            return *m_AppLayer;
        }

        void run();

    private:

        void detachAppLayer();

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
