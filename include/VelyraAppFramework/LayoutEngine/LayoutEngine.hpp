#pragma once

#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>
#include <VelyraAppFramework/LayoutEngine/Layout.hpp>

#include <VelyraUtils/DevUtils/PrivateMemberGetter.hpp>

#include <unordered_map>

namespace Velyra::App {

    class LayoutNode;

    class VL_API LayoutEngine {
    public:
        LayoutEngine();

        ~LayoutEngine() = default;

        void registerLayout(Layout& layout);

        void setActiveLayout(LayoutID layoutID);

        LayoutID getActiveLayout() const { return m_ActiveLayout; }

        /**
         * @brief Calculate the current active layout based on the given window dimensions and position. With these new
         *        dimensions, the layout will draw all active panels in the correct position and size.
         * @param xPos The x position of the entire layout box (usually the window position)
         * @param yPos The y position of the entire layout box (usually the window position)
         * @param windowWidth The width of the entire layout box (usually the window width)
         * @param windowHeight The height of the entire layout box (usually the window height)
         */
        void draw(float xPos, float yPos, float windowWidth, float windowHeight, Core::Window& window, Core::Context& context);

    private:
        static UP<LayoutNode> createLayoutNode(const SP<Node>& nodeDesc, LayoutNode* parent);

    private:
        Utils::LogPtr m_Logger;

        LayoutID m_ActiveLayout = 0;
        std::unordered_map<LayoutID, SP<LayoutNode>> m_Layouts;

        VL_GET_PRIVATE_MEMBER(LayoutEngine, m_Layouts);
    };



}
