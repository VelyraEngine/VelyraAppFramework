#pragma once

#include <VelyraAppFramework/LayoutEngine/LayoutDefinitions.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>

namespace Velyra::App {

    class LayoutNode {
    public:
        explicit LayoutNode(NodeID nodeID, LayoutNode* parent);

        virtual ~LayoutNode() = default;

        void addChild(UP<LayoutNode> child) {
            m_Children.push_back(std::move(child));
        }

        const std::vector<UP<LayoutNode>>& getChildren() const { return m_Children; }

        /**
         * @brief The user may specify the ratio of some panels as DONT_CARE. Figure out the ratio for those panels
         *        based on the remaining ratio after accounting for the panels with known ratio. This should be called
         *        after creating the layout tree and before calculating the layout for the first time.
         */
        void calculateUnknownRatio();

        virtual void beginPanel(const std::string& name) {
            for (const auto& child : m_Children) {
                child->beginPanel(name);
            }
        }

        virtual void endPanel(const std::string& name) {
            for (const auto& child : m_Children) {
                child->endPanel(name);
            }
        }

        /**
         * @brief Converts the ratio-based position and size of this layout node to pixel-based position and size based
         *        on the given position and size of the entire layout box. This should be called every time the window
         *        is resized or when the layout is first calculated.
         * @param pixelPosition The position in pixels that this node should use as the top-left corner of its layout box.
         * @param pixelSize The size in pixels that this node should use as the size of its layout box.
         */
        virtual void ratioToPixel(const ImVec2& pixelPosition, const ImVec2& pixelSize);

        virtual void localUpdate(NodeID /*childID*/, const ImVec2& /*pixelDeltaSize*/, ResizeDirection /*direction*/) {}

    public:
        const NodeID m_NodeID;

        float ratio = DONT_CARE;
        ImVec2 position = ImVec2(0.0f, 0.0f);
        ImVec2 size = ImVec2(0.0f, 0.0f);

    protected:
        LayoutNode* m_Parent = nullptr;
        std::vector<UP<LayoutNode>> m_Children;
    };

    class PanelImpl: public LayoutNode {
    public:
        PanelImpl(NodeID nodeID, LayoutNode* parent, const Panel& desc);

        ~PanelImpl() override = default;

        void beginPanel(const std::string &name) override;

        void endPanel(const std::string &name) override;

        const std::string& getName() const { return m_Panel.name; }

    public:
        Panel m_Panel;

    };

    class VerticalSplit: public LayoutNode {
    public:
        VerticalSplit(NodeID nodeID, LayoutNode* parent);

        ~VerticalSplit() override = default;

        void ratioToPixel(const ImVec2& position, const ImVec2& size) override;

        void localUpdate(NodeID childID, const ImVec2& pixelDeltaSize, ResizeDirection direction) override;
    };

    class HorizontalSplit: public LayoutNode {
    public:
        HorizontalSplit(NodeID nodeID, LayoutNode* parent);

        ~HorizontalSplit() override = default;

        void ratioToPixel(const ImVec2& position, const ImVec2& size) override;

        void localUpdate(NodeID childID, const ImVec2& pixelDeltaSize, ResizeDirection direction) override;
    };

}