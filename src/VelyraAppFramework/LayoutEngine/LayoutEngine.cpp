#include <Pch.hpp>

#include "../../../include/VelyraAppFramework/LayoutEngine/LayoutEngine.hpp"
#include "LayoutNode.hpp"
#include "../LoggerNames.hpp"

namespace Velyra::App {

    LayoutEngine::LayoutEngine():
    m_Logger(Utils::getLogger(VL_APP_LAYOUT_LOGGER)){

    }

    void LayoutEngine::registerLayout(Layout &layout) {
        const LayoutID layoutID = layout.getID();
        if (m_Layouts.find(layoutID) != m_Layouts.end()) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Layout with ID '{}' already registered. Overwriting!", layoutID);
        }
        m_LayoutSources[layoutID] = &layout;
        m_Layouts[layoutID] = SP<LayoutNode>(buildNode(layout.getLayout(), nullptr));
        m_DirtyLayouts.erase(layoutID);
    }

    void LayoutEngine::updateLayout(Layout &layout) {
        const LayoutID layoutID = layout.getID();
        const auto layoutIt = m_Layouts.find(layoutID);
        if (layoutIt == m_Layouts.end()) {
            registerLayout(layout);
            return;
        }
        m_LayoutSources[layoutID] = &layout;

        const SP<Node> desc = layout.getLayout();
        SP<LayoutNode>& root = layoutIt->second;
        if (!root || root->getType() != desc->type || root->getReconcileKey() != reconcileKeyOf(desc)) {
            root = SP<LayoutNode>(buildNode(desc, nullptr));
        }
        else {
            updateFromDesc(*root, desc);
            if (desc->type != NodeType::Panel) {
                reconcileChildren(*root, desc);
            }
        }
        m_DirtyLayouts.erase(layoutID);
    }

    void LayoutEngine::markDirty(const LayoutID layoutID) {
        if (m_LayoutSources.find(layoutID) == m_LayoutSources.end()) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Cannot mark layout '{}' dirty: not registered.", layoutID);
            return;
        }
        m_DirtyLayouts.insert(layoutID);
    }

    void LayoutEngine::setActiveLayout(const LayoutID layoutID) {
        const auto layoutIt = m_Layouts.find(layoutID);
        if (layoutIt == m_Layouts.end()) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Layout with ID '{}' not found. Register first a layout for this ID!", layoutID);
            return;
        }
        m_ActiveLayout = layoutID;
    }

    void LayoutEngine::draw(const float xPos, const float yPos, const float windowWidth, const float windowHeight,
        Core::Window &window, Core::Context &context) {
        // Reconcile the active layout first if it was invalidated since the last frame.
        if (m_DirtyLayouts.count(m_ActiveLayout) > 0) {
            const auto sourceIt = m_LayoutSources.find(m_ActiveLayout);
            if (sourceIt != m_LayoutSources.end() && sourceIt->second != nullptr) {
                updateLayout(*sourceIt->second);
            }
            m_DirtyLayouts.erase(m_ActiveLayout);
        }

        const auto layoutIt = m_Layouts.find(m_ActiveLayout);
        if (layoutIt == m_Layouts.end()) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Layout '{}' not found. Register first a layout for this ID!", m_ActiveLayout);
            return;
        }
        // First calculate the layout based on the current window size and position
        layoutIt->second->ratioToPixel(ImVec2(xPos, yPos), ImVec2(windowWidth, windowHeight));

        // Then draw the panels, traversing the tree
        layoutIt->second->draw(window, context);
    }

    NodeID LayoutEngine::reconcileKeyOf(const SP<Node> &desc) {
        if (desc->type == NodeType::Panel) {
            const auto* panelDesc = static_cast<PanelNode*>(desc.get());
            return panelDesc->desc.key != 0 ? panelDesc->desc.key : std::hash<std::string>{}(panelDesc->desc.name);
        }
        const auto* splitDesc = static_cast<Split*>(desc.get());
        return splitDesc->key;
    }

    Sizing LayoutEngine::sizingOf(const SP<Node> &desc) {
        if (desc->type == NodeType::Panel) {
            const auto* panelDesc = static_cast<PanelNode*>(desc.get());
            return effectiveSizing(panelDesc->desc.sizing, panelDesc->desc.sizeRatio);
        }
        const auto* splitDesc = static_cast<Split*>(desc.get());
        return splitDesc->sizing;
    }

    UP<LayoutNode> LayoutEngine::buildNode(const SP<Node> &desc, LayoutNode *parent) {
        const NodeID key = reconcileKeyOf(desc);
        const Sizing sizing = sizingOf(desc);
        switch (desc->type) {
            case NodeType::Panel: {
                const auto* panelDesc = static_cast<PanelNode*>(desc.get());
                auto node = createUP<PanelImpl>(panelDesc->id, key, parent, panelDesc->desc);
                node->sizing = sizing;
                node->lastDescSizing = sizing;
                return node;
            }
            case NodeType::HorizontalSplit: {
                const auto* splitDesc = static_cast<Split*>(desc.get());
                auto node = createUP<HorizontalSplit>(splitDesc->id, key, parent);
                node->sizing = sizing;
                node->lastDescSizing = sizing;
                for (const auto& childDesc : splitDesc->children) {
                    node->addChild(buildNode(childDesc, node.get()));
                }
                return node;
            }
            case NodeType::VerticalSplit: {
                const auto* splitDesc = static_cast<Split*>(desc.get());
                auto node = createUP<VerticalSplit>(splitDesc->id, key, parent);
                node->sizing = sizing;
                node->lastDescSizing = sizing;
                for (const auto& childDesc : splitDesc->children) {
                    node->addChild(buildNode(childDesc, node.get()));
                }
                return node;
            }
        }
        VL_THROW("Unknown node type in layout description!");
    }

    UP<LayoutNode> LayoutEngine::reconcileNode(UP<LayoutNode> old, const SP<Node> &desc, LayoutNode *parent) {
        const NodeType type = desc->type;
        const NodeID key = reconcileKeyOf(desc);
        if (old && old->getType() == type && old->getReconcileKey() == key) {
            old->m_Parent = parent;
            updateFromDesc(*old, desc);
            if (type != NodeType::Panel) {
                reconcileChildren(*old, desc);
            }
            return old;
        }
        return buildNode(desc, parent);
    }

    void LayoutEngine::updateFromDesc(LayoutNode &node, const SP<Node> &desc) {
        const Sizing descSizing = sizingOf(desc);
        // Adopt the description's sizing only when it actually changed in code; otherwise keep the user-adjusted value.
        if (!sizingEqual(descSizing, node.lastDescSizing)) {
            node.sizing = descSizing;
        }
        node.lastDescSizing = descSizing;

        if (desc->type == NodeType::Panel) {
            const auto* panelDesc = static_cast<PanelNode*>(desc.get());
            static_cast<PanelImpl&>(node).m_Panel = panelDesc->desc;
        }
    }

    void LayoutEngine::reconcileChildren(LayoutNode &parentNode, const SP<Node> &desc) {
        const auto* splitDesc = static_cast<Split*>(desc.get());

        std::vector<UP<LayoutNode>> pool = std::move(parentNode.m_Children);
        parentNode.m_Children.clear();
        std::vector<bool> used(pool.size(), false);

        for (const auto& childDesc : splitDesc->children) {
            const NodeType type = childDesc->type;
            const NodeID key = reconcileKeyOf(childDesc);

            Size matchIdx = pool.size();
            if (key != 0) {
                // Keyed children match only against a node with the same key and type.
                for (Size i = 0; i < pool.size(); ++i) {
                    if (!used[i] && pool[i] && pool[i]->getType() == type && pool[i]->getReconcileKey() == key) {
                        matchIdx = i;
                        break;
                    }
                }
            }
            else {
                // Structural children match the first unused same-type node by position.
                for (Size i = 0; i < pool.size(); ++i) {
                    if (!used[i] && pool[i] && pool[i]->getType() == type && pool[i]->getReconcileKey() == 0) {
                        matchIdx = i;
                        break;
                    }
                }
            }

            UP<LayoutNode> matched;
            if (matchIdx < pool.size()) {
                matched = std::move(pool[matchIdx]);
                used[matchIdx] = true;
            }
            parentNode.addChild(reconcileNode(std::move(matched), childDesc, &parentNode));
        }
    }
}
