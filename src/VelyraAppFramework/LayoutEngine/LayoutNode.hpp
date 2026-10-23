#pragma once

#include <VelyraCore/ImGui/ImGui.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutDefinitions.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>

#include <limits>

namespace Velyra::App {

    constexpr Size NO_PARENT = std::numeric_limits<Size>::max();

    inline NodeID hashName(const std::string_view name) {
        return std::hash<std::string_view>{}(name);
    }

    /**
     * @brief A node of the layout tree after calculation. Everything needed after the calculation is copied from the
     *        description, since the description tree is owned by the caller and usually only lives for one frame.
     */
    struct LayoutNode {
        NodeID id = 0;
        NodeType type = NodeType::Panel;
        NodeID nameHash = 0; // 0 if the node has no name
        float descRatio = DONT_CARE; // The ratio as given in the description

        // Only valid during calculate() and draw()!
        const Node* desc = nullptr;

        Size parent = NO_PARENT;
        Size firstChild = 0;
        Size childCount = 0;

        float ratio = DONT_CARE; // Resolved ratio compared to its siblings, all siblings sum up to 1
        ImVec2 position = ImVec2(0.0f, 0.0f);
        ImVec2 size = ImVec2(0.0f, 0.0f);

        const Panel& getPanel() const { return static_cast<const PanelNode*>(desc)->desc; }

        const Split& getSplit() const { return *static_cast<const Split*>(desc); }
    };

}
