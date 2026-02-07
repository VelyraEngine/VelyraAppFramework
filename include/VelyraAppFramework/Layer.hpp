#pragma once

#include <VelyraAppFramework/AppData.hpp>
#include <VelyraUtils/Clock/Clock.hpp>

namespace Velyra::App {

    class VL_API Layer {
    public:
        explicit Layer(AppData& appData): m_AppData(appData) {}

        virtual ~Layer() = default;

        /**
         * @brief Called when the layer is attached to the application, can be used for initialization.
         *        The window and context are already created at this point, so they can be used for initialization as well.
         */
        virtual void onAttach(const UP<Core::Window>& /*window*/, const UP<Core::Context>& /*contex*/) {}

        /**
         * @brief Called when the layer is detached from the application, can be used for cleanup.
         */
        virtual void onDetach(const UP<Core::Window>& /*window*/, const UP<Core::Context>& /*context*/) {}

        /**
         * @brief Called once every frame, updates can be done here.
         */
        virtual void onUpdate(const UP<Core::Window>& /*window*/, const UP<Core::Context>& /*context*/, Duration /*deltaTime*/) {}

        /**
         * @brief Called once every frame, make ImGui calls here.
         */
        virtual void onImGui(const UP<Core::Window>& /*window*/, const UP<Core::Context>& /*context*/) {}

        /**
         * @brief Called when an event occurs, events can be handled here. Return true if the event was handled and
         *        should not be passed to other layers.
         */
        virtual bool onEvent(const UP<Core::Window>& /*window*/, const UP<Core::Context>& /*context*/, const Core::Event& /*event*/) { return false; }

    protected:
        AppData& m_AppData;
    };

}
