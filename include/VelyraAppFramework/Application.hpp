#pragma once

#include <VelyraAppFramework/Settings/Settings.hpp>
#include <VelyraAppFramework/AppInclude.hpp>
#include <VelyraAppFramework/Layer.hpp>
#include <VelyraUtils/Logging/LoggingFwd.hpp>

namespace Velyra::App {

    class Layer;
    class AppData;

    struct VL_API ApplicationDesc {
        bool useImPlot = false;
        /**
         * @brief Covers the main window with an ImGui dockspace. Layouts are then drawn as dockable windows: the layout
         *        tree only gives the default place of every panel, after that the user can move, tab and undock them.
         */
        bool useDockSpace = false;
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

        AppData& getAppData() const;

        Layer& getAppLayer() const {
            return *m_AppLayer;
        }

        void run() const;

    private:

        void detachAppLayer();

        void createWindow(const ApplicationDesc& appDesc);

        void createContext() const;

        void createImGuiContext(const ApplicationDesc& appDesc);

        void createAppData(const ProgramArgs& programArgs);

        void processEvents() const;

        void drawActiveLayout(U32 dockSpaceID) const;

        void destroyAppData();

        void destroyImGuiContext();

        void destroyContext();

        void destroyWindow();

    private:
        Utils::LogPtr m_Logger;

        bool m_UseDockSpace = false;
        std::string m_ImGuiIniFile; // ImGui keeps a pointer to the file name, so it must outlive the ImGui context

        Settings m_Settings;
        UP<AppData> m_AppData;
        UP<Layer> m_AppLayer;
        UP<Core::Window> m_Window;
    };

}
