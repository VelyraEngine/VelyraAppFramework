#pragma once

#include <VelyraAppFramework/AppData.hpp>
#include <VelyraAppFramework/Layer.hpp>

namespace Velyra::App {

    struct VL_API ApplicationDesc {
        bool enableMainMenuBar = false; // If true, shifts the UI down to make room for the ImGui main menu bar

        bool settingsEnableSave = true;
        std::string applicationName;
    };

    class VL_API Application {
    public:
        Application(const ApplicationDesc& appDesc, const ProgramArgs& programArgs);

        ~Application() = default;

        void run() const;

        template<typename T, typename... Args>
        void pushLayer(Args&&... args) {
            m_LayerStack.emplace_back(std::make_unique<T>(m_AppData, std::forward<Args>(args)...));
        }

    private:

        void attachLayers() const;

        void internalRun() const;

        void detachLayers() const;

        void processEvents() const;

    private:
        AppData m_AppData;
        std::vector<UP<Layer>> m_LayerStack;
        UP<Core::Window> m_Window;
    };

}
