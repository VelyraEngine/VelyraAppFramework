#pragma once

#include <VelyraAppFramework/LayoutEngine/LayoutDefinitions.hpp>

#include <string>
#include <vector>

namespace Velyra::App {

    enum class NodeType {
        Panel,
        HorizontalSplit, // Children are placed next to each other, from left to right
        VerticalSplit    // Children are placed below each other, from top to bottom
    };

    struct VL_API Node {
        const NodeType type;

        explicit Node(NodeType type_);

        virtual ~Node() = default;
    };

    void defaultDrawFunction();

    struct VL_API Panel{
        std::string name; // Must be unique within the layout, it is used as the ImGui window name and to identify the panel between frames
        DrawFunction drawFunction = defaultDrawFunction;
        bool resizable = true;
        bool collapsible = false;
        float sizeRatio = DONT_CARE; // Relative size ratio compared to other nodes in the same split
    };

    struct VL_API SplitDesc {
        std::string name{}; // Optional, if empty the index within the parent is used to identify the split between frames
        float sizeRatio = DONT_CARE; // Relative size ratio compared to other nodes in the same split
    };

    template<typename OBJECT>
    DrawFunction bindDraw(OBJECT* object, void(OBJECT::*memberFunction)()) {
        return [object, memberFunction]() {
            (object->*memberFunction)();
        };
    }

    struct VL_API PanelNode: public Node {
        const Panel desc;

        explicit PanelNode(const Panel& desc_);

        ~PanelNode() override = default;
    };

    struct VL_API Split: public Node {
        const SplitDesc desc;
        std::vector<SP<Node>> children;

        template<typename... NODES>
        explicit Split(const NodeType type_, const SplitDesc& desc_, NODES&&... nodes): Node(type_), desc(desc_) {
            children.reserve(sizeof...(NODES));
            (children.push_back(std::forward<NODES>(nodes)), ...);
        }

        ~Split() override = default;
    };

    template<typename T>
    concept LayoutNodeType = std::convertible_to<T, SP<Node>>;

    inline UP<Node> createPanel(const Panel& desc) {
        return createUP<PanelNode>(desc);
    }

    inline SP<Node> createLayout(SP<Node> root) {
        return root;
    }

    template<LayoutNodeType... NODES>
    UP<Node> horizontalSplit(const SplitDesc& desc, NODES&&... nodes) {
        return createUP<Split>(NodeType::HorizontalSplit, desc, std::forward<NODES>(nodes)...);
    }

    template<LayoutNodeType... NODES>
    UP<Node> horizontalSplit(NODES&&... nodes) {
        return horizontalSplit(SplitDesc{}, std::forward<NODES>(nodes)...);
    }

    template<LayoutNodeType... NODES>
    UP<Node> verticalSplit(const SplitDesc& desc, NODES&&... nodes) {
        return createUP<Split>(NodeType::VerticalSplit, desc, std::forward<NODES>(nodes)...);
    }

    template<LayoutNodeType... NODES>
    UP<Node> verticalSplit(NODES&&... nodes) {
        return verticalSplit(SplitDesc{}, std::forward<NODES>(nodes)...);
    }

}
