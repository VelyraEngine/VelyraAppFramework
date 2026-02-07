#include <Pch.hpp>

#include <VelyraAppFramework/Application.hpp>

namespace Velyra::App {

    Application::Application(const ApplicationDesc &appDesc, const ProgramArgs &programArgs):
    m_AppData(programArgs){
        m_Window = Core::WindowFactory::createWindow(appDesc.windowSettings);

        const UP<Core::Context>& context = m_Window->createContext(appDesc.contextSettings);
        context->createImGuiContext(appDesc.imGuiSettings);

        m_AppData.settings = Settings(appDesc.settings);
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
            context->onImGuiBegin();
            for (const auto& layer: m_LayerStack) {
                layer->onImGui(m_Window, context);
            }
            context->onImGuiEnd();

            context->endFrame();
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
