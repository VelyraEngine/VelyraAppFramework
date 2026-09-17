#pragma once

#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>
#include <VelyraAppFramework/LayoutEngine/Layout.hpp>
#include <VelyraUtils/Logging/LoggingFwd.hpp>

#include <VelyraUtils/DevUtils/PrivateMemberGetter.hpp>

#include <unordered_map>
#include <unordered_set>

namespace Velyra::App {

    class LayoutNode;

    class VL_API LayoutEngine {
    public:
        LayoutEngine();

        ~LayoutEngine() = default;

        void registerLayout(Layout& layout);

        /**
         * @brief Re-pulls the description of an already registered layout and reconciles it against the live tree,
         *        preserving user-adjusted sizes and panel state for nodes that are still present.
         */
        void updateLayout(Layout& layout);

        /**
         * @brief Marks a registered layout as dirty so its description is re-pulled and reconciled on the next draw.
         */
        void markDirty(LayoutID layoutID);

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
        static NodeID reconcileKeyOf(const SP<Node>& desc);

        static Sizing sizingOf(const SP<Node>& desc);

        static UP<LayoutNode> buildNode(const SP<Node>& desc, LayoutNode* parent);

        static UP<LayoutNode> reconcileNode(UP<LayoutNode> old, const SP<Node>& desc, LayoutNode* parent);

        static void updateFromDesc(LayoutNode& node, const SP<Node>& desc);

        static void reconcileChildren(LayoutNode& parentNode, const SP<Node>& desc);

    private:
        Utils::LogPtr m_Logger;

        LayoutID m_ActiveLayout = 0;
        std::unordered_map<LayoutID, SP<LayoutNode>> m_Layouts;
        std::unordered_map<LayoutID, Layout*> m_LayoutSources;
        std::unordered_set<LayoutID> m_DirtyLayouts;

        VL_GET_PRIVATE_MEMBER(LayoutEngine, m_Layouts)
    };



}
