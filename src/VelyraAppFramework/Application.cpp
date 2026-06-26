#include <Pch.hpp>

#include <VelyraAppFramework/Application.hpp>

namespace Velyra::App {

    SettingsDesc createSettingsDesc(const ApplicationDesc& appDesc) {
        SettingsDesc settingsDesc;
        settingsDesc.enableSave = appDesc.settingsEnableSave;
        settingsDesc.applicationName = appDesc.applicationName;
        const std::string settingsFileName = "settings_" + appDesc.applicationName + ".json";
        settingsDesc.settingsFilePath = getAppdataPath(appDesc.applicationName) / settingsFileName;
        return settingsDesc;
    }

    Application::Application(const ApplicationDesc &appDesc, const ProgramArgs &programArgs):
    m_AppData(programArgs, createSettingsDesc(appDesc)){
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
        imGuiDesc.useDocking = m_AppData.settings.contextSettings.useDocking;
        imGuiDesc.useViewports = m_AppData.settings.contextSettings.useViewports;
        imGuiDesc.useImPlot = appDesc.useImPlot;
        context->createImGuiContext(imGuiDesc);

        if (!appDesc.saveImGuiWindowData) {
            ImGui::GetIO().IniFilename = nullptr;
        }
    }

    Application::~Application() {
        const UP<Core::Context>& context = m_Window->getContext();
        if (m_AppLayer) {
            m_AppLayer->onDetach(*m_Window, *context);
        }

    }

    void Application::run() {
        VL_PRECONDITION(m_Window->getContext() != nullptr, "Context is null")

        const UP<Core::Context>& context = m_Window->getContext();

        TimePoint lastFrameTime = getTime() - 1ms;
        while (m_Window->isOpen()) {
            processEvents();

            context->beginFrame();

            // onUpdate
            const TimePoint currentTime = getTime();
            const Duration deltaTime = currentTime - lastFrameTime;
            lastFrameTime = currentTime;
            if (m_AppLayer) {
                m_AppLayer->onUpdate(*m_Window, *context, deltaTime);

                // onImGui
                context->onImGuiBegin();
                m_AppLayer->mainMenuBar(*m_Window, *context);
                m_AppData.layoutEngine.draw(
                    0.0f, 0.0f,
                    static_cast<float>(context->getClientWidth()), static_cast<float>(context->getClientHeight()),
                    *m_Window, *context
                );
                m_AppLayer->onImGui(*m_Window, *context);
                // Draw panels before popups so that popups are always on top
                m_AppData.drawPanels(*m_Window, *context);
                // Draw popups on top of everything else
                m_AppData.drawPopups(*m_Window, *context);

                context->onImGuiEnd();
            }
            context->endFrame();

            context->swapBuffers();
        }
    }

    void Application::processEvents() const {
        const UP<Core::Context>& context = m_Window->getContext();

        m_Window->pollEvents();
        while (m_Window->hasEvent()) {
            Core::Event event = m_Window->getNextEvent();

            if (m_AppLayer) {
                m_AppLayer->onEvent(*m_Window, *context, event);
            }

            if (event.type == VL_EVENT_WINDOW_CLOSED) {
                m_Window->close();
            }
        }
    }
}
