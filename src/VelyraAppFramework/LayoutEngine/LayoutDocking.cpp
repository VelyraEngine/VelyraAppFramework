#include <Pch.hpp>

#include "LayoutDocking.hpp"

#include <VelyraCore/ImGui/imgui_internal.h>

#include <unordered_set>

namespace Velyra::App {

    namespace {

        enum class Placement {
            None,     // Never placed, or its dock node no longer exists
            Docked,   // Docked in the dockspace
            Floating  // Moved out of the dockspace by the user
        };

        /**
         * @brief Where the window of a panel is right now. Only a window that exists this session counts as floating:
         *        an ini file without a dock node for the window (e.g. written without the dockspace) places it again.
         */
        Placement findPlacement(const std::string& name, const ImGuiDockNode* root, ImGuiID& dockID) {
            dockID = 0;
            if (const ImGuiWindow* window = ImGui::FindWindowByName(name.c_str())) {
                if (window->DockId == 0) {
                    return Placement::Floating;
                }
                dockID = window->DockId;
            }
            else if (const ImGuiWindowSettings* settings = ImGui::FindWindowSettingsByID(ImHashStr(name.c_str()))) {
                dockID = settings->DockId;
            }
            if (dockID == 0) {
                return Placement::None;
            }
            ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockID);
            if (node == nullptr) {
                dockID = 0;
                return Placement::None;
            }
            if (ImGui::DockNodeGetRootNode(node) != root) {
                // Docked in a floating dock node together with other windows
                dockID = 0;
                return Placement::Floating;
            }
            return Placement::Docked;
        }

        int depthOf(const ImGuiDockNode* node) {
            int depth = 0;
            for (; node->ParentNode != nullptr; node = node->ParentNode) {
                depth++;
            }
            return depth;
        }

        ImGuiDockNode* commonAncestor(ImGuiDockNode* a, ImGuiDockNode* b) {
            int depthA = depthOf(a);
            int depthB = depthOf(b);
            for (; depthA > depthB; depthA--) { a = a->ParentNode; }
            for (; depthB > depthA; depthB--) { b = b->ParentNode; }
            while (a != b) {
                a = a->ParentNode;
                b = b->ParentNode;
            }
            return a;
        }

        class DockPlacer {
        public:
            DockPlacer(const std::vector<LayoutNode>& nodes, const ImGuiID dockSpaceID, ImGuiDockNode* root, const bool placeAll):
            m_Nodes(nodes),
            m_DockSpaceID(dockSpaceID),
            m_Root(root),
            m_DockIDs(nodes.size(), 0),
            m_PlacedCount(nodes.size(), 0) {
                // Children are always stored after their parent, so walking backwards visits children first
                for (Size i = m_Nodes.size(); i-- > 0;) {
                    if (m_Nodes[i].type == NodeType::Panel) {
                        const std::string& name = m_Nodes[i].getPanel().name;
                        m_TreeWindows.insert(ImHashStr(name.c_str()));
                        if (!placeAll && findPlacement(name, m_Root, m_DockIDs[i]) != Placement::None) {
                            m_PlacedCount[i] = 1;
                        }
                    }
                    if (m_Nodes[i].parent != NO_PARENT) {
                        m_PlacedCount[m_Nodes[i].parent] += m_PlacedCount[i];
                    }
                }
            }

            bool needsPlacement() const {
                Size panelCount = 0;
                for (const LayoutNode& node: m_Nodes) {
                    panelCount += node.type == NodeType::Panel ? 1 : 0;
                }
                return !m_Nodes.empty() && m_PlacedCount[0] < panelCount;
            }

            void place() {
                placeNew(0);
                ImGui::DockBuilderFinish(m_DockSpaceID);
            }

        private:
            /**
             * @brief Places every subtree below the given node whose panels all need a place
             */
            void placeNew(const Size nodeIndex) {
                if (m_PlacedCount[nodeIndex] == 0) {
                    placeSubtree(nodeIndex, findTarget(nodeIndex));
                    return;
                }
                const LayoutNode& node = m_Nodes[nodeIndex];
                for (Size i = node.firstChild; i < node.firstChild + node.childCount; i++) {
                    placeNew(i);
                }
            }

            ImGuiDockNode* centralNode() const {
                ImGuiDockNode* central = m_Root->CentralNode != nullptr ? m_Root->CentralNode : m_Root;
                return central->IsLeafNode() ? central : nullptr;
            }

            /**
             * @brief True when the central node holds no window of the tree, it only holds windows that are not
             *        submitted anymore or nothing at all
             */
            bool isCentralNodeFree() const {
                const ImGuiDockNode* central = centralNode();
                if (central == nullptr || m_CentralNodeTaken) {
                    return false;
                }
                for (const ImGuiWindow* window: central->Windows) {
                    if (m_TreeWindows.contains(window->ID)) {
                        return false;
                    }
                }
                return true;
            }

            /**
             * @brief The dock node in which the given subtree must be built
             */
            ImGuiID findTarget(const Size nodeIndex) {
                const Size parentIndex = m_Nodes[nodeIndex].parent;
                if (parentIndex == NO_PARENT || isCentralNodeFree()) {
                    m_CentralNodeTaken = true;
                    const ImGuiDockNode* central = centralNode();
                    return central != nullptr ? central->ID : m_DockSpaceID;
                }

                // Next to the closest sibling that is docked, looking both ways
                const LayoutNode& parent = m_Nodes[parentIndex];
                const Size localIndex = nodeIndex - parent.firstChild;
                for (Size distance = 1; distance < parent.childCount; distance++) {
                    for (const bool before: {true, false}) {
                        if (before ? distance > localIndex : localIndex + distance >= parent.childCount) {
                            continue;
                        }
                        const Size siblingIndex = before ? nodeIndex - distance : nodeIndex + distance;
                        if (ImGuiDockNode* siblingNode = dockNodeOfSubtree(siblingIndex)) {
                            // A sibling before the subtree means the subtree goes after it
                            return splitNextTo(nodeIndex, siblingIndex, siblingNode->ID, before);
                        }
                    }
                }
                // No docked sibling, e.g. they all float: at the border of the whole dockspace
                return splitNextTo(nodeIndex, parentIndex, m_DockSpaceID, localIndex > 0);
            }

            ImGuiID splitNextTo(const Size nodeIndex, const Size siblingIndex, const ImGuiID siblingNodeID, const bool after) {
                const bool horizontal = m_Nodes[m_Nodes[nodeIndex].parent].type == NodeType::HorizontalSplit;
                const ImGuiDir direction = horizontal ? (after ? ImGuiDir_Right : ImGuiDir_Left) : (after ? ImGuiDir_Down : ImGuiDir_Up);
                const float ratio = m_Nodes[nodeIndex].ratio;
                const float siblingRatio = siblingIndex == m_Nodes[nodeIndex].parent ? 1.0f : m_Nodes[siblingIndex].ratio;
                ImGuiID remaining = 0;
                return split(siblingNodeID, direction, ratio / (ratio + siblingRatio), remaining);
            }

            /**
             * @brief The smallest dock node that holds all docked panels of the given subtree, nullptr if none of them
             *        is docked
             */
            ImGuiDockNode* dockNodeOfSubtree(const Size nodeIndex) const {
                const LayoutNode& node = m_Nodes[nodeIndex];
                if (node.type == NodeType::Panel) {
                    return m_DockIDs[nodeIndex] != 0 ? ImGui::DockBuilderGetNode(m_DockIDs[nodeIndex]) : nullptr;
                }
                ImGuiDockNode* result = nullptr;
                for (Size i = node.firstChild; i < node.firstChild + node.childCount; i++) {
                    if (ImGuiDockNode* childNode = dockNodeOfSubtree(i)) {
                        result = result == nullptr ? childNode : commonAncestor(result, childNode);
                    }
                }
                return result;
            }

            /**
             * @brief Builds the subtree in the given dock node, the child with the largest ratio inherits the node
             */
            void placeSubtree(const Size nodeIndex, const ImGuiID dockID) {
                const LayoutNode& node = m_Nodes[nodeIndex];
                if (node.type == NodeType::Panel) {
                    ImGui::DockBuilderDockWindow(node.getPanel().name.c_str(), dockID);
                    m_DockIDs[nodeIndex] = dockID;
                    for (Size i = nodeIndex; i != NO_PARENT; i = m_Nodes[i].parent) {
                        m_PlacedCount[i]++;
                    }
                    return;
                }
                if (node.childCount == 0) {
                    return;
                }
                const Size first = node.firstChild;
                const Size last = node.firstChild + node.childCount - 1;
                Size inheritor = first;
                for (Size i = first; i <= last; i++) {
                    if (m_Nodes[i].ratio > m_Nodes[inheritor].ratio) {
                        inheritor = i;
                    }
                }

                const bool horizontal = node.type == NodeType::HorizontalSplit;
                ImGuiID remaining = dockID;
                float remainingRatio = 1.0f;
                for (Size i = first; i < inheritor; i++) {
                    const ImGuiID childID = split(remaining, horizontal ? ImGuiDir_Left : ImGuiDir_Up, m_Nodes[i].ratio / remainingRatio, remaining);
                    remainingRatio -= m_Nodes[i].ratio;
                    placeSubtree(i, childID);
                }
                for (Size i = last; i > inheritor; i--) {
                    const ImGuiID childID = split(remaining, horizontal ? ImGuiDir_Right : ImGuiDir_Down, m_Nodes[i].ratio / remainingRatio, remaining);
                    remainingRatio -= m_Nodes[i].ratio;
                    placeSubtree(i, childID);
                }
                placeSubtree(inheritor, remaining);
            }

            /**
             * @brief Splits a dock node, the new node at the given direction gets the given ratio. The node at the
             *        other side inherits the windows of the split node.
             * @param remaining Receives the node at the other side
             * @return The node at the given direction
             */
            ImGuiID split(const ImGuiID dockID, const ImGuiDir direction, const float ratio, ImGuiID& remaining) {
                ImGuiID atDirection = 0;
                ImGui::DockBuilderSplitNode(dockID, direction, std::clamp(ratio, 0.05f, 0.95f), &atDirection, &remaining);

                // ImGui only moves the windows that are submitted. Windows that are not (e.g. the panels of another
                // document) still refer to the split node and ImGui would make them float when they come back.
                ImGuiContext& g = *ImGui::GetCurrentContext();
                for (ImGuiWindow* window: g.Windows) {
                    if (window->DockNode == nullptr && window->DockId == dockID) {
                        window->DockId = remaining;
                    }
                }
                for (ImGuiWindowSettings* settings = g.SettingsWindows.begin(); settings != nullptr; settings = g.SettingsWindows.next_chunk(settings)) {
                    if (settings->DockId == dockID) {
                        settings->DockId = remaining;
                    }
                }
                for (ImGuiID& id: m_DockIDs) {
                    if (id == dockID) {
                        id = remaining;
                    }
                }
                return atDirection;
            }

        private:
            const std::vector<LayoutNode>& m_Nodes;
            const ImGuiID m_DockSpaceID;
            ImGuiDockNode* m_Root;

            std::vector<ImGuiID> m_DockIDs; // Per node, the dock node of a docked panel
            std::vector<Size> m_PlacedCount; // Per node, the number of panels in its subtree that have a place
            std::unordered_set<ImGuiID> m_TreeWindows;
            bool m_CentralNodeTaken = false;
        };

    }

    bool dockNewPanels(const std::vector<LayoutNode>& nodes, const ImGuiID dockSpaceID, const bool placeAll) {
        ImGuiDockNode* root = ImGui::DockBuilderGetNode(dockSpaceID);
        if (root == nullptr || nodes.empty()) {
            return false;
        }
        DockPlacer placer(nodes, dockSpaceID, root, placeAll);
        if (!placer.needsPlacement()) {
            return false;
        }
        placer.place();
        return true;
    }

}
