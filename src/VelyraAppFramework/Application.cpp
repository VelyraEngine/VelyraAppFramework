#include <Pch.hpp>

#include <VelyraAppFramework/Application.hpp>

namespace Velyra::App {

    Application::Application(const ApplicationDesc &appDesc, const ProgramArgs &programArgs):
    m_AppData(programArgs){
        SettingsDesc settingsDesc;
        settingsDesc.enableSave = appDesc.settingsEnableSave;
        const std::string settingsFileName = "settings_" + appDesc.applicationName + ".json";
        settingsDesc.settingsFilePath = get_appdata_path(appDesc.applicationName) / settingsFileName;

        m_AppData.settings = Settings(settingsDesc);

        Core::WindowDesc windowDesc;
        windowDesc.width = m_AppData.settings.windowSettings.width;
        windowDesc.height = m_AppData.settings.windowSettings.height;
        windowDesc.title = appDesc.applicationName;
        m_Window = Core::WindowFactory::createWindow(windowDesc);

        Core::ContextDesc contextDesc;
        contextDesc.api = m_AppData.settings.contextSettings.graphicsAPI;

        const UP<Core::Context>& context = m_Window->createContext(contextDesc);
        context->setVerticalSynchronisation(m_AppData.settings.contextSettings.enableVSync);

        Core::ImGuiContextDesc imGuiDesc;
        imGuiDesc.useDocking = m_AppData.settings.imGuiSettings.useDocking;
        imGuiDesc.useViewports = m_AppData.settings.imGuiSettings.useViewports;
        imGuiDesc.useImPlot = m_AppData.settings.imGuiSettings.useImPlot;
        context->createImGuiContext(imGuiDesc);
    }

    void Application::run() const {
        VL_PRECONDITION(m_Window->getContext() != nullptr, "Context is null")

        attachLayers();
        internalRun();
        detachLayers();
    }

    void Application::attachLayers() const {
        const UP<Core::Context>& context = m_Window->getContext();
        for (const auto& layer: m_LayerStack) {
            layer->onAttach(m_Window, context);
        }
    }

    void Application::internalRun() const {
        const UP<Core::Context>& context = m_Window->getContext();

        TimePoint lastFrameTime = getTime() - 1ms;
        while (m_Window->isOpen()) {
            processEvents();

            context->beginFrame();

            // onUpdate
            const TimePoint currentTime = getTime();
            const Duration deltaTime = currentTime - lastFrameTime;
            lastFrameTime = currentTime;
            for (const auto& layer: m_LayerStack) {
                layer->onUpdate(m_Window, context, deltaTime);
            }

            // onImGui
            m_AppData.layoutEngine.calculateLayout(0.0f, 0.0f, static_cast<float>(context->getClientWidth()), static_cast<float>(context->getClientHeight()));
            context->onImGuiBegin();
            for (const auto& layer: m_LayerStack) {
                layer->onImGui(m_Window, context);
            }
            context->onImGuiEnd();

            context->endFrame();

            context->swapBuffers();
        }

    }

    void Application::detachLayers() const {
        const UP<Core::Context>& context = m_Window->getContext();
        for (const auto& layer: m_LayerStack) {
            layer->onDetach(m_Window, context);
        }
    }

    void Application::processEvents() const {
        const UP<Core::Context>& context = m_Window->getContext();

        m_Window->pollEvents();
        while (m_Window->hasEvent()) {
            Core::Event event = m_Window->getNextEvent();
            Size layerIndex = 0;
            bool eventHandled = false;
            while (layerIndex < m_LayerStack.size() && !eventHandled) {
                eventHandled = m_LayerStack[layerIndex]->onEvent(m_Window, context, event);
                layerIndex++;
            }
            if (event.type == VL_EVENT_WINDOW_CLOSED) {
                m_Window->close();
            }
        }
    }
}
