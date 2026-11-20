#pragma once

#include "LayoutNode.hpp"

#include <vector>

namespace Velyra::App {

    /**
     * @brief Docks the panels of a calculated layout tree that do not have a place in the dockspace yet. Panels that
     *        are docked or that the user made floating are left alone, so the tree only decides the first place of a
     *        panel:
     *        - A subtree whose panels all need a place is built as a whole, with the split directions and ratios of
     *          the description. The child with the largest ratio keeps the node that was split, so the central node of
     *          the dockspace ends up in the largest panel.
     *        - The root of the tree is built into the central node.
     *        - When the central node only holds windows that are not part of the tree anymore (e.g. the previous
     *          document), the subtree is built there, so documents that replace each other share the same place.
     *        - Otherwise the subtree is docked next to its closest docked sibling, by splitting the smallest dock node
     *          that holds that sibling.
     *        Must be called after the dockspace was submitted and before the panels are.
     * @param nodes The nodes of LayoutEngine::calculate()
     * @param dockSpaceID The ID of a dockspace that was submitted this frame
     * @param placeAll Places all panels as if they never had a place, used after the dockspace was cleared
     * @return true when at least one panel was placed
     */
    bool dockNewPanels(const std::vector<LayoutNode>& nodes, ImGuiID dockSpaceID, bool placeAll);

}
