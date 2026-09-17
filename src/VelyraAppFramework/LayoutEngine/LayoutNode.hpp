#pragma once

#include <VelyraCore/ImGui/ImGui.hpp>

#include <VelyraAppFramework/LayoutEngine/LayoutDefinitions.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>

namespace Velyra::App {

    class LayoutEngine;

    class LayoutNode {
    public:
        LayoutNode(NodeID nodeID, NodeID reconcileKey, NodeType type, LayoutNode* parent);

        virtual ~LayoutNode() = default;

        void addChild(UP<LayoutNode> child) {
            m_Children.push_back(std::move(child));
        }

        const std::vector<UP<LayoutNode>>& getChildren() const { return m_Children; }

        NodeType getType() const { return m_Type; }

        NodeID getReconcileKey() const { return m_ReconcileKey; }

        virtual void draw(Core::Window& window, Core::Context& context) {
            for (const auto& child : m_Children) {
                child->draw(window, context);
            }
        }

        /**
         * @brief Converts the ratio/pixel-based sizing of this node's children into pixel-based positions and sizes,
         *        based on the given position and size of this node's layout box. This should be called every time the
         *        window is resized or when the layout is first calculated.
         * @param pixelPosition The position in pixels that this node should use as the top-left corner of its layout box.
         * @param pixelSize The size in pixels that this node should use as the size of its layout box.
         */
        virtual void ratioToPixel(const ImVec2& pixelPosition, const ImVec2& pixelSize);

        virtual void localUpdate(NodeID /*childID*/, const ImVec2& /*pixelDeltaSize*/, ResizeDirection /*direction*/) {}

    public:
        const NodeID m_NodeID;

        Sizing sizing = {};         // Live sizing of this node within its parent (may be user-adjusted at runtime).
        Sizing lastDescSizing = {}; // Last sizing supplied by the description, used to detect code-driven changes.
        ImVec2 position = ImVec2(0.0f, 0.0f);
        ImVec2 size = ImVec2(0.0f, 0.0f);

    protected:
        friend class LayoutEngine;

        const NodeID m_ReconcileKey;
        const NodeType m_Type;
        LayoutNode* m_Parent = nullptr;
        std::vector<UP<LayoutNode>> m_Children;
    };

    class PanelImpl: public LayoutNode {
    public:
        PanelImpl(NodeID nodeID, NodeID reconcileKey, LayoutNode* parent, const Panel& desc);

        ~PanelImpl() override = default;

        void draw(Core::Window &window, Core::Context &context) override;

        const std::string& getName() const { return m_Panel.name; }

    public:
        Panel m_Panel;

    private:
        void beginPanel() const;

        void endPanel() const;

    };

    class VerticalSplit: public LayoutNode {
    public:
        VerticalSplit(NodeID nodeID, NodeID reconcileKey, LayoutNode* parent);

        ~VerticalSplit() override = default;

        void ratioToPixel(const ImVec2& position, const ImVec2& size) override;

        void localUpdate(NodeID childID, const ImVec2& pixelDeltaSize, ResizeDirection direction) override;
    };

    class HorizontalSplit: public LayoutNode {
    public:
        HorizontalSplit(NodeID nodeID, NodeID reconcileKey, LayoutNode* parent);

        ~HorizontalSplit() override = default;

        void ratioToPixel(const ImVec2& position, const ImVec2& size) override;

        void localUpdate(NodeID childID, const ImVec2& pixelDeltaSize, ResizeDirection direction) override;
    };

}