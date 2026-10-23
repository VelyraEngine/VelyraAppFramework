#pragma once

#include <VelyraAppFramework/LayoutEngine/LayoutEngine.hpp>

namespace Velyra::App {

    class Application;

    /**
     * @brief A layout of panels that can be registered in AppData. The framework draws the active layout every frame,
     *        between Layer::mainMenuBar() and Layer::onImGui().
     */
    class VL_API Layout {
    public:
        explicit Layout(LayoutID id = generateLayoutID());

        virtual ~Layout();

        LayoutID getID() const { return m_ID; }

        /**
         * @brief Called every frame while this layout is active, so the returned tree may change between frames.
         * @return The root of the layout tree, nullptr to draw nothing
         */
        virtual SP<Node> getLayout() = 0;

        /**
         * @brief Forgets all resizes done by the user, so all panels go back to the ratios returned by getLayout().
         */
        void resetResizes();

    private:
        friend Application;

        void draw();

    private:
        const LayoutID m_ID;

        // Every layout has its own engine, so the resizes of the user are remembered per layout
        LayoutEngine m_LayoutEngine;
    };

}
