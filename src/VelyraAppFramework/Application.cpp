#include <Pch.hpp>
#include <LoggerNames.hpp>

#include <VelyraAppFramework/Application.hpp>
#include <VelyraAppFramework/Layer.hpp>
#include <VelyraAppFramework/Layout.hpp>
#include <VelyraAppFramework/AppData.hpp>
#include "FrameworkSettings.hpp"

namespace Velyra::App {

    SettingsDesc processSettingsDesc(const ApplicationDesc& appDesc) {
        SettingsDesc settings = appDesc.settings;

        // If the settings file path is empty, use a default path in the appdata folder of the application
        if (settings.settingsFile.empty()) {
            const std::string settingsFileName = "settings_" + appDesc.applicationName + ".json";
            const fs::path defaultSettingsPath = getAppdataPath(appDesc.applicationName) / settingsFileName;
            settings.settingsFile = defaultSettingsPath;
        }
        return settings;
    }

    Application::Application(const ApplicationDesc &appDesc, const ProgramArgs &programArgs):
    m_Logger(Utils::getLogger(VL_APP_APPLICATION_LOGGER)),
    m_Settings(processSettingsDesc(appDesc)) {
        createWindow(appDesc);
        createContext();
        createImGuiContext(appDesc);
        createAppData(programArgs);
    }

    Application::~Application() {
        // The layer can own resources of the context, so it must be destroyed before the context is
        detachAppLayer();
        destroyAppData();
        destroyImGuiContext();
        destroyContext();
        destroyWindow();
    }

    AppData& Application::getAppData() const {
        VL_PRECONDITION(m_AppData != nullptr, "AppData is null");
        return *m_AppData;
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
                m_AppLayer->onUpdate(deltaTime);

                // onImGui
                context->onImGuiBegin();
                m_AppLayer->mainMenuBar();

                const LayoutID activeLayoutID = m_AppData->getActiveLayout();
                if (activeLayoutID != NO_LAYOUT) {
                    SP<Layout> activeLayout = m_AppData->getLayout(activeLayoutID);
                    if (activeLayout) {
                        activeLayout->draw();
                    }
                }
                m_AppLayer->onImGui();
                // Draw panels before popups so that popups are always on top
                m_AppData->drawPanels();
                // Draw popups on top of everything else
                m_AppData->drawPopups();

                context->onImGuiEnd();
            }
            context->endFrame();

            context->swapBuffers();
        }
    }

    void Application::detachAppLayer() {
        if (m_AppLayer == nullptr) {
            return;
        }
        m_AppLayer->onDetach();
        m_AppLayer.reset();
        m_AppData->clear();
    }

    void Application::createWindow(const ApplicationDesc& appDesc) {
        const auto windowSettings = m_Settings.getSetting<WindowSettings>("WindowSettings");

        Core::WindowDesc windowDesc;
        windowDesc.width = windowSettings.width;
        windowDesc.height = windowSettings.height;
        windowDesc.title = appDesc.applicationName;
        m_Window = Core::WindowFactory::createWindow(windowDesc);

        VL_POSTCONDITION(m_Window != nullptr, "Failed to create window");
    }

    void Application::createContext() const {
        VL_PRECONDITION(m_Window != nullptr, "Failed to create window");

        const auto contextSettings = m_Settings.getSetting<ContextSettings>("ContextSettings");

        Core::ContextDesc contextDesc;
        contextDesc.api = contextSettings.graphicsAPI;

        const UP<Core::Context>& context = m_Window->createContext(contextDesc);
        context->setVerticalSynchronisation(contextSettings.enableVSync);
    }

    void Application::createImGuiContext(const ApplicationDesc& appDesc) const {
        VL_PRECONDITION(m_Window != nullptr, "Failed to create window");
        VL_PRECONDITION(m_Window->getContext() != nullptr, "Failed to create context");

        const UP<Core::Context>& context = m_Window->getContext();

        const auto imGuiSettings = m_Settings.getSetting<ImGuiSettings>("ImGuiSettings");

        Core::ImGuiContextDesc imGuiDesc;
        imGuiDesc.useDocking = imGuiSettings.useDocking;
        imGuiDesc.useViewports = imGuiSettings.useViewports;
        imGuiDesc.useImPlot = appDesc.useImPlot;
        context->createImGuiContext(imGuiDesc);

        if (!imGuiSettings.saveImGuiWindowData) {
            ImGui::GetIO().IniFilename = nullptr;
        }
    }

    void Application::createAppData(const ProgramArgs& programArgs) {
        VL_PRECONDITION(m_Window != nullptr, "Failed to create window");
        VL_PRECONDITION(m_Window->getContext() != nullptr, "Failed to create context");

        m_AppData = createUP<AppData>(programArgs, *m_Window, *m_Window->getContext(), m_Settings);
    }

    void Application::processEvents() const {
        m_Window->pollEvents();
        while (m_Window->hasEvent()) {
            Core::Event event = m_Window->getNextEvent();

            if (m_AppLayer) {
                m_AppLayer->onEvent(event);
            }

            if (event.type == VL_EVENT_WINDOW_CLOSED) {
                m_Window->close();
            }
        }
    }

    void Application::destroyAppData() {
        m_AppData.reset();
    }

    void Application::destroyImGuiContext() {
        VL_PRECONDITION(m_Window != nullptr, "Failed to create window");
        VL_PRECONDITION(m_Window->getContext() != nullptr, "Failed to create context");

        const UP<Core::Context>& context = m_Window->getContext();
        ImGuiSettings imGuiSettings;
        imGuiSettings.useDocking = ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable;
        imGuiSettings.useViewports = ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable;
        imGuiSettings.saveImGuiWindowData = ImGui::GetIO().IniFilename != nullptr;
        m_Settings.setSetting<ImGuiSettings>("ImGuiSettings", imGuiSettings);

        context->DestroyImGuiContext();
    }

    void Application::destroyContext() {
        VL_PRECONDITION(m_Window != nullptr, "Failed to create window");
        VL_PRECONDITION(m_Window->getContext() != nullptr, "Failed to create context");

        const UP<Core::Context>& context = m_Window->getContext();
        ContextSettings settings;
        settings.enableVSync = context->isVerticalSynchronisationEnabled();
        settings.graphicsAPI = context->getType();
        m_Settings.setSetting<ContextSettings>("ContextSettings", settings);

        m_Window->destroyContext();
    }

    void Application::destroyWindow() {
        WindowSettings windowSettings;
        windowSettings.width = m_Window->getWidth();
        windowSettings.height = m_Window->getHeight();

        m_Settings.setSetting<WindowSettings>("WindowSettings", windowSettings);
        m_Window.reset();
    }

}
