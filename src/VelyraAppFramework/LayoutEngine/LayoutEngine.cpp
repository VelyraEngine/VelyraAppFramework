#include <Pch.hpp>
#include <LoggerNames.hpp>

#include <VelyraAppFramework/LayoutEngine/LayoutEngine.hpp>
#include "LayoutNode.hpp"

namespace Velyra::App {

    // A node can never be resized smaller than this ratio of its parent
    constexpr float MIN_RATIO = 0.05f;

    inline NodeID hashCombine(const NodeID seed, const NodeID value) {
        return seed ^ (value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2));
    }

    /**
     * @brief Named nodes are identified by their name, unnamed splits by their index within the parent. Together with
     *        the ID of the parent, this gives every node an ID that is stable between frames.
     */
    inline NodeID generateNodeID(const NodeID parentID, const NodeID nameHash, const Size index) {
        return hashCombine(parentID, nameHash != 0 ? nameHash : index);
    }

    /**
     * @brief Copies everything needed after calculation from the description into the node
     */
    inline void initNode(LayoutNode& node, const Node& desc) {
        node.desc = &desc;
        node.type = desc.type;
        const std::string& name = desc.type == NodeType::Panel
            ? static_cast<const PanelNode&>(desc).desc.name
            : static_cast<const Split&>(desc).desc.name;
        node.nameHash = name.empty() ? 0 : hashName(name);
        node.descRatio = desc.type == NodeType::Panel
            ? static_cast<const PanelNode&>(desc).desc.sizeRatio
            : static_cast<const Split&>(desc).desc.sizeRatio;
    }

    inline bool PIXEL_EQUAL(const ImVec2& a, const ImVec2& b) {
        constexpr float epsilon = 1.0f; // 1 pixel tolerance
        return (std::abs(a.x - b.x) < epsilon) && (std::abs(a.y - b.y) < epsilon);
    }

    LayoutEngine::LayoutEngine():
    m_Logger(Utils::getLogger(VL_APP_LAYOUT_LOGGER)){

    }

    LayoutEngine::~LayoutEngine() = default;

    void LayoutEngine::draw(const Node& root, const float xPos, const float yPos, const float width, const float height) {
        calculate(root, xPos, yPos, width, height);

        for (Size i = 0; i < m_Nodes.size(); i++) {
            if (m_Nodes[i].type == NodeType::Panel) {
                drawPanel(i);
            }
        }
    }

    void LayoutEngine::draw(const Node& root) {
        // The framebuffer is in physical pixels (larger on HiDPI) and includes the menu bar, so use ImGui's work area.
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        draw(root, 0.0f, 0.0f, viewport->WorkSize.x, viewport->WorkSize.y);
    }

    void LayoutEngine::calculate(const Node& root, const float xPos, const float yPos, const float width, const float height) {
        m_Nodes.clear();

        LayoutNode& rootNode = m_Nodes.emplace_back();
        initNode(rootNode, root);
        rootNode.id = generateNodeID(0, rootNode.nameHash, 0);
        rootNode.ratio = 1.0f;
        rootNode.position = ImVec2(std::round(xPos), std::round(yPos));
        rootNode.size = ImVec2(std::round(width), std::round(height));

        calculateChildren(0);
        checkDuplicatePanels();
    }

    void LayoutEngine::resizePanel(const std::string_view panelName, const float deltaWidth, const float deltaHeight, const ResizeDirection direction) {
        const NodeID nameHash = hashName(panelName);
        for (Size i = 0; i < m_Nodes.size(); i++) {
            if (m_Nodes[i].type != NodeType::Panel || m_Nodes[i].nameHash != nameHash) {
                continue;
            }
            if (std::fabs(deltaWidth) > 1e-4f && (hasFlag(direction, Left) || hasFlag(direction, Right))) {
                resizeNode(i, deltaWidth, true, hasFlag(direction, Left));
            }
            if (std::fabs(deltaHeight) > 1e-4f && (hasFlag(direction, Top) || hasFlag(direction, Bottom))) {
                resizeNode(i, deltaHeight, false, hasFlag(direction, Top));
            }
            return;
        }
        SPDLOG_LOGGER_ERROR(m_Logger, "Cannot resize panel '{}', it is not part of the current layout", panelName);
    }

    void LayoutEngine::resetResizes() {
        m_NodeStates.clear();
    }

    void LayoutEngine::calculateChildren(const Size nodeIndex) {
        if (m_Nodes[nodeIndex].type == NodeType::Panel) {
            return;
        }
        // Careful, m_Nodes can reallocate while adding children, so only access nodes by index from here on
        const Split& split = m_Nodes[nodeIndex].getSplit();
        const NodeID parentID = m_Nodes[nodeIndex].id;
        const Size firstChild = m_Nodes.size();

        for (Size i = 0; i < split.children.size(); i++) {
            // Null children are skipped, this allows to conditionally leave out nodes in the description
            if (split.children[i] == nullptr) {
                continue;
            }
            LayoutNode& child = m_Nodes.emplace_back();
            initNode(child, *split.children[i]);
            child.id = generateNodeID(parentID, child.nameHash, i);
            child.parent = nodeIndex;
            child.ratio = child.descRatio;

            // Use the ratio the user resized to, unless the description itself changed since then
            const auto stateIt = m_NodeStates.find(child.id);
            if (stateIt != m_NodeStates.end() && stateIt->second.descRatio == child.ratio) {
                child.ratio = stateIt->second.ratio;
            }
        }
        m_Nodes[nodeIndex].firstChild = firstChild;
        m_Nodes[nodeIndex].childCount = m_Nodes.size() - firstChild;

        distributeRatios(firstChild, m_Nodes[nodeIndex].childCount);
        calculateChildRects(nodeIndex);

        for (Size i = firstChild; i < firstChild + m_Nodes[nodeIndex].childCount; i++) {
            calculateChildren(i);
        }
    }

    void LayoutEngine::checkDuplicatePanels() {
        m_PanelNameHashes.clear();
        for (const auto& node : m_Nodes) {
            if (node.type == NodeType::Panel) {
                m_PanelNameHashes.push_back(node.nameHash);
            }
        }
        std::sort(m_PanelNameHashes.begin(), m_PanelNameHashes.end());
        for (Size i = 1; i < m_PanelNameHashes.size(); i++) {
            const NodeID nameHash = m_PanelNameHashes[i];
            if (nameHash != m_PanelNameHashes[i - 1] || !m_ReportedDuplicates.insert(nameHash).second) {
                continue;
            }
            // The description is still valid here, look up the name for the error message
            for (const auto& node : m_Nodes) {
                if (node.type == NodeType::Panel && node.nameHash == nameHash) {
                    SPDLOG_LOGGER_ERROR(m_Logger, "Multiple panels are named '{}', panel names must be unique within a layout", node.getPanel().name);
                    break;
                }
            }
        }
    }

    void LayoutEngine::distributeRatios(const Size firstChild, const Size childCount) {
        if (childCount == 0) {
            return;
        }
        // First, determine how much ratio is known and how many children have an unknown ratio
        float knownRatio = 0.0f;
        Size unknownCount = 0;
        for (Size i = firstChild; i < firstChild + childCount; i++) {
            if (m_Nodes[i].ratio > 0.0f) {
                knownRatio += m_Nodes[i].ratio;
            }
            else {
                unknownCount++;
            }
        }

        // Then distribute the remaining ratio among the unknown children. If nothing remains, the unknown children get
        // the average size of the known ones.
        if (unknownCount > 0) {
            const float remainingRatio = 1.0f - knownRatio;
            const float unknownRatio = remainingRatio > MIN_RATIO
                ? remainingRatio / static_cast<float>(unknownCount)
                : knownRatio / static_cast<float>(childCount - unknownCount);
            for (Size i = firstChild; i < firstChild + childCount; i++) {
                if (m_Nodes[i].ratio <= 0.0f) {
                    m_Nodes[i].ratio = unknownRatio;
                }
            }
        }

        // Finally, normalize so all siblings fill the parent exactly, even when nodes were added or removed
        float totalRatio = 0.0f;
        for (Size i = firstChild; i < firstChild + childCount; i++) {
            totalRatio += m_Nodes[i].ratio;
        }
        for (Size i = firstChild; i < firstChild + childCount; i++) {
            m_Nodes[i].ratio /= totalRatio;
        }
    }

    void LayoutEngine::calculateChildRects(const Size nodeIndex) {
        const LayoutNode& node = m_Nodes[nodeIndex];
        const bool horizontal = node.type == NodeType::HorizontalSplit;
        const float start = horizontal ? node.position.x : node.position.y;
        const float length = horizontal ? node.size.x : node.size.y;

        // Round the edges instead of the sizes, so the children never overlap or leave a gap
        float cumulativeRatio = 0.0f;
        float childStart = start;
        for (Size i = node.firstChild; i < node.firstChild + node.childCount; i++) {
            cumulativeRatio += m_Nodes[i].ratio;
            const bool isLast = (i == node.firstChild + node.childCount - 1);
            const float childEnd = isLast ? start + length : std::round(start + cumulativeRatio * length);

            if (horizontal) {
                m_Nodes[i].position = ImVec2(childStart, node.position.y);
                m_Nodes[i].size = ImVec2(childEnd - childStart, node.size.y);
            }
            else {
                m_Nodes[i].position = ImVec2(node.position.x, childStart);
                m_Nodes[i].size = ImVec2(node.size.x, childEnd - childStart);
            }
            childStart = childEnd;
        }
    }

    void LayoutEngine::drawPanel(const Size nodeIndex) {
        const LayoutNode& node = m_Nodes[nodeIndex];
        const Panel& panel = node.getPanel();
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowPos(ImVec2(node.position.x + viewport->WorkPos.x, node.position.y + viewport->WorkPos.y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(node.size, ImGuiCond_Always);
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking;
        if (!panel.resizable) {
            windowFlags |= ImGuiWindowFlags_NoResize;
        }
        if (!panel.collapsible) {
            windowFlags |= ImGuiWindowFlags_NoCollapse;
        }
        ImGui::Begin(panel.name.c_str(), nullptr, windowFlags);
        panel.drawFunction();

        // Convert ImGui's absolute window position back into our local layout space, so that comparisons are done in
        // the same coordinate system.
        const ImVec2 newSize = ImGui::GetWindowSize();
        const ImVec2 newPos = ImGui::GetWindowPos();
        const ImVec2 localNewPos = ImVec2(newPos.x - viewport->WorkPos.x, newPos.y - viewport->WorkPos.y);
        ImGui::End();

        // Only handle actual resizes
        if (PIXEL_EQUAL(node.size, newSize) && PIXEL_EQUAL(node.position, localNewPos)) {
            return;
        }
        // If the size changed and the position as well, the left/top border was dragged, otherwise the right/bottom one
        const auto deltaSize = ImVec2(newSize.x - node.size.x, newSize.y - node.size.y);
        const auto deltaPos = ImVec2(localNewPos.x - node.position.x, localNewPos.y - node.position.y);
        if (std::fabs(deltaSize.x) > 1e-4f) {
            resizeNode(nodeIndex, deltaSize.x, true, std::fabs(deltaPos.x) > 1e-4f);
        }
        if (std::fabs(deltaSize.y) > 1e-4f) {
            resizeNode(nodeIndex, deltaSize.y, false, std::fabs(deltaPos.y) > 1e-4f);
        }
    }

    void LayoutEngine::resizeNode(const Size nodeIndex, const float deltaPixels, const bool horizontal, const bool towardsStart) {
        // Walk up the tree until we find a split along the resize axis that has a sibling on the dragged border
        const NodeType splitType = horizontal ? NodeType::HorizontalSplit : NodeType::VerticalSplit;
        Size childIndex = nodeIndex;
        Size parentIndex = m_Nodes[nodeIndex].parent;
        while (parentIndex != NO_PARENT) {
            const LayoutNode& parent = m_Nodes[parentIndex];
            const Size localIndex = childIndex - parent.firstChild;
            const bool hasNeighbor = towardsStart ? localIndex > 0 : localIndex + 1 < parent.childCount;

            if (parent.type == splitType && hasNeighbor) {
                const Size neighborIndex = towardsStart ? childIndex - 1 : childIndex + 1;
                const float parentLength = horizontal ? parent.size.x : parent.size.y;
                const float deltaRatio = deltaPixels / parentLength;
                const float newChildRatio = m_Nodes[childIndex].ratio + deltaRatio;
                const float newNeighborRatio = m_Nodes[neighborIndex].ratio - deltaRatio;
                if (newChildRatio < MIN_RATIO || newNeighborRatio < MIN_RATIO) {
                    return;
                }
                m_Nodes[childIndex].ratio = newChildRatio;
                m_Nodes[neighborIndex].ratio = newNeighborRatio;

                // Store the ratios of all siblings, so siblings with an unknown ratio do not move either
                for (Size i = parent.firstChild; i < parent.firstChild + parent.childCount; i++) {
                    m_NodeStates[m_Nodes[i].id] = NodeState{m_Nodes[i].ratio, m_Nodes[i].descRatio};
                }
                return;
            }
            childIndex = parentIndex;
            parentIndex = parent.parent;
        }
    }
}
