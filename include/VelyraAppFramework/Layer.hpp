#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <VelyraUtils/Clock/Clock.hpp>

namespace Velyra::App {

    class AppData;

    class VL_API Layer {
    public:
        explicit Layer(AppData& appData);

        virtual ~Layer();

        /**
         * @brief Called when the layer is attached to the application, can be used for initialization.
         *        The window and context are already created at this point, so they can be used for initialization as well.
         */
        virtual void onAttach();

        /**
         * @brief Called when the layer is detached from the application, can be used for cleanup.
         */
        virtual void onDetach();

        /**
         * @brief Called once every frame, updates can be done here.
         * @param deltaTime The time since the last frame, can be used for time-based updates.
         */
        virtual void onUpdate(Duration deltaTime);

        /**
         * @brief Called once every frame, make ImGui calls here to draw the main menu bar
         */
        virtual void mainMenuBar();

        /**
         * @brief Called once every frame, make ImGui calls here.
         */
        virtual void onImGui();

        /**
         * @brief Called when an event occurs, events can be handled here.
         */
        virtual void onEvent(const Core::Event& event);

    protected:
        AppData& m_AppData;
    };

}
