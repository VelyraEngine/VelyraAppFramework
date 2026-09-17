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

    void defaultDrawFunction(Core::Window& window, Core::Context& context);

    struct VL_API Panel{
        std::string name;
        DrawFunction drawFunction = defaultDrawFunction;
        bool resizable = true;
        bool collapsible = false;
        float sizeRatio = DONT_CARE; // Legacy convenience: relative size ratio. Ignored when 'sizing' is set.
        Sizing sizing = {};          // Full sizing (ratio/pixels + min/max clamps). Overrides sizeRatio.
        NodeID key = 0;              // Stable reconciliation key (0 = derived from name).
    };

    template<typename OBJECT>
    DrawFunction bindDraw(OBJECT* object, void(OBJECT::*memberFunction)(Core::Window&, Core::Context&)) {
        return [object, memberFunction](Core::Window& window, Core::Context& context) {
            (object->*memberFunction)(window, context);
        };
    }

    struct VL_API PanelNode: public Node {
        const Panel desc;

        explicit PanelNode(const Panel& desc_);

        ~PanelNode() override = default;
    };

    struct VL_API Split: public Node {
        std::vector<SP<Node>> children;
        Sizing sizing = {};  // Sizing of this split within its parent split (auto ratio by default).
        NodeID key = 0;      // Stable reconciliation key (0 = structural, matched by position).

        template<typename... NODES>
        explicit Split(const NodeType type_, NODES&&... nodes): Node(type_) {
            (children.push_back(std::forward<NODES>(nodes)), ...);
        }

        ~Split() override = default;
    };

    // Optional leading descriptor for splits. Passing a braced-init-list (e.g. {.sizing = pixels(40)})
    // binds only to this overload, so existing split calls without a descriptor stay unambiguous.
    struct VL_API SplitDesc {
        float sizeRatio = DONT_CARE;
        Sizing sizing = {};
        NodeID key = 0;
    };

    inline UP<Node> createPanel(const Panel& desc) {
        return createUP<PanelNode>(desc);
    }

    inline SP<Node> createLayout(SP<Node> root) {
        return root;
    }

    template<typename... NODES>
    UP<Node> horizontalSplit(NODES&&... nodes) {
        return createUP<Split>(NodeType::HorizontalSplit, std::forward<NODES>(nodes)...);
    }

    template<typename... NODES>
    UP<Node> horizontalSplit(const SplitDesc& desc, NODES&&... nodes) {
        auto split = createUP<Split>(NodeType::HorizontalSplit, std::forward<NODES>(nodes)...);
        split->sizing = effectiveSizing(desc.sizing, desc.sizeRatio);
        split->key = desc.key;
        return split;
    }

    template<typename... NODES>
    UP<Node> verticalSplit(NODES&&... nodes) {
        return createUP<Split>(NodeType::VerticalSplit, std::forward<NODES>(nodes)...);
    }

    template<typename... NODES>
    UP<Node> verticalSplit(const SplitDesc& desc, NODES&&... nodes) {
        auto split = createUP<Split>(NodeType::VerticalSplit, std::forward<NODES>(nodes)...);
        split->sizing = effectiveSizing(desc.sizing, desc.sizeRatio);
        split->key = desc.key;
        return split;
    }

}