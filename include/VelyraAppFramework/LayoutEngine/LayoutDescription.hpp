#pragma once

#include <VelyraAppFramework/LayoutEngine/LayoutDefinitions.hpp>

namespace Velyra::App {

    enum class NodeType {
        Panel,
        HorizontalSplit,
        VerticalSplit
    };

    struct VL_API Node {
        NodeID id = 0;
        NodeType type;

        explicit Node(NodeType type_);

        virtual ~Node() = default;
    };

    struct VL_API Panel{
        std::string name;
        bool resizable = true;
        bool collapsible = false;
        float sizeRatio = DONT_CARE; // Relative size ratio compared to other panels in the same split
    };

    struct VL_API PanelNode: public Node {
        const Panel desc;

        explicit PanelNode(const Panel& desc_);

        ~PanelNode() override = default;
    };

    struct VL_API Split: public Node {
        std::vector<UP<Node>> children;

        template<typename... NODES>
        explicit Split(const NodeType type, NODES... nodes): Node(type) {
            (children.push_back(std::move(nodes)), ...);
        }

        ~Split() override = default;
    };

    inline UP<Node> createPanel(const Panel& desc) {
        return createUP<PanelNode>(desc);
    }

    inline UP<Node> createLayout(UP<Node> root) {
        return root;
    }

    template<typename... NODES>
    UP<Node> horizontalSplit(NODES... nodes) {
        return createUP<Split>(NodeType::HorizontalSplit, std::move(nodes)...);
    }

    template<typename... NODES>
    UP<Node> verticalSplit(NODES... nodes) {
        return createUP<Split>(NodeType::VerticalSplit, std::move(nodes)...);
    }

}