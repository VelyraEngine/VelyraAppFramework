#include <VelyraAppFramework/LayoutEngine/LayoutNode.hpp>

#include "VelyraAppFramework/LoggerNames.hpp"

namespace Velyra::App {

    LayoutNode::LayoutNode(const NodeID nodeID, LayoutNode *parent):
    m_NodeID(nodeID),
    m_Parent(parent){

    }

    void LayoutNode::calculateUnknownRatio() {
        // First, determine how much ratio is known and how many children have unknown ratio
        float knownRatio = 0.0f;
        Size childrenWithUnknownRatio = 0;
        for (const auto& child : m_Children) {
            child->calculateUnknownRatio();
            if (child->ratio > DONT_CARE) {
                knownRatio += child->ratio;
            }
            else {
                childrenWithUnknownRatio++;
            }
        }

        // Then distribute the remaining ratio among the unknown children
        const float remainingRatio = 1.0f - knownRatio;
        if (childrenWithUnknownRatio > 0) {
            const float ratioPerUnknownChild = remainingRatio / static_cast<float>(childrenWithUnknownRatio);
            for (const auto& child : m_Children) {
                if (child->ratio < DONT_CARE + 0.001f) {
                    child->ratio = ratioPerUnknownChild;
                }
            }
        }
        if (m_Parent == nullptr) {
            ratio = 1.0f; // Root node always has ratio 1.0
        }
    }

    void LayoutNode::ratioToPixel(const ImVec2 &pixelPosition, const ImVec2 &pixelSize) {
        position = PIXEL_CEIL(pixelPosition);
        size = PIXEL_CEIL(pixelSize);
    }

    /////////////////////// PanelImpl Implementation ///////////////////////

    PanelImpl::PanelImpl(const NodeID nodeID, LayoutNode *parent, const Panel &desc):
    LayoutNode(nodeID, parent),
    m_Panel(desc){
        ratio = desc.sizeRatio;
    }

    void PanelImpl::beginPanel(const std::string &name) {
        if (name != m_Panel.name) {
            return;
        }
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowPos(ImVec2(position.x + viewport->Pos.x, position.y + viewport->Pos.y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;
        if (!m_Panel.resizable) {
            window_flags |= ImGuiWindowFlags_NoResize;
        }
        if (!m_Panel.collapsible) {
            window_flags |= ImGuiWindowFlags_NoCollapse;
        }
        ImGui::Begin(m_Panel.name.c_str(), nullptr, window_flags);
    }

    void PanelImpl::endPanel(const std::string &name) {
        if (name != m_Panel.name) {
            return;
        }
        const ImVec2 newSize = ImGui::GetWindowSize();
        const ImVec2 newPos = ImGui::GetWindowPos();
        // Only handle resizes
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const ImVec2 absolutePos = ImVec2(position.x + viewport->Pos.x, position.y + viewport->Pos.y);
        if (!PIXEL_EQUAL(size, newSize) || !PIXEL_EQUAL(position, absolutePos)) {
            const Utils::LogPtr logger = Utils::getLogger(VL_APP_LAYOUT_LOGGER);
            SPDLOG_LOGGER_INFO(logger, "Panel '{}' resized from size ({}, {}) at position ({}, {}) to size ({}, {}) at position ({}, {})",
                                m_Panel.name, size.x, size.y, position.x, position.y, newSize.x, newSize.y, absolutePos.x, absolutePos.y);
            /*
             * delta_size.x > 0  => increased left      delta_pos.x > 0  => increased right
             * delta_size.x < 0  => decreased left      delta_pos.x < 0  => decreased left
             * delta_size.y > 0  => increased bottom    delta_pos.y > 0  => decreased top
             * delta_size.y < 0  => decreased bottom    delta_pos.y < 0  => increased top
             */
            const auto deltaSize = ImVec2(newSize.x - size.x, newSize.y - size.y);
            const auto deltaPos = ImVec2(newPos.x - position.x, newPos.y - position.y);

            ResizeDirection direction = ResizeDirection::None;
            if (std::fabs(deltaSize.x) > 1e-4f) {
                if (std::fabs(deltaPos.x) > 1e-4f) {
                    direction |= Left;
                }
                else {
                    direction |= Right;
                }
            }
            if (std::fabs(deltaSize.y) > 1e-4f) {
                if (std::fabs(deltaPos.y) > 1e-4f) {
                    direction |= Top;
                }
                else {
                    direction |= Bottom;
                }
            }

            // Call the parent and resize accordingly
            if (m_Parent) {
                m_Parent->localUpdate(m_NodeID, deltaSize, direction);
            }
        }
        ImGui::End();
    }

    /////////////////////// VerticalSplit Implementation ///////////////////////

    VerticalSplit::VerticalSplit(const NodeID nodeID, LayoutNode *parent):
    LayoutNode(nodeID, parent) {

    }

    void VerticalSplit::ratioToPixel(const ImVec2 &position, const ImVec2 &size) {
        LayoutNode::ratioToPixel(position, size);

        float currentY = position.y;
        for (const auto& child : m_Children) {
            const float childHeight = std::ceil(size.y * child->ratio);
            child->ratioToPixel(ImVec2(position.x, currentY), ImVec2(size.x, childHeight));
            currentY += childHeight;
        }
    }

    void VerticalSplit::localUpdate(const NodeID childID, const ImVec2& pixelDeltaSize, const ResizeDirection direction) {
if (hasFlag(direction, Left) or hasFlag(direction, Right)) {
            if (m_Parent) {
                m_Parent->localUpdate(m_NodeID, pixelDeltaSize, direction);
            }
        }
        // 1. Determine child idx
        Size child_idx = 0;
        bool has_previous_sibling = false;
        bool has_next_sibling = false;
        for (Size i = 0; i < m_Children.size(); i++) {
            if (m_Children[i]->m_NodeID == childID) {
                child_idx = i;
                has_previous_sibling = (i > 0);
                has_next_sibling = (i < m_Children.size() - 1);
            }
        }
        // 2. Determine how the ratio changed
        const float delta_ratio = pixelDeltaSize.y / size.y;

        // 3. update ratios
        if (hasFlag(direction, Top)) {
            if (has_previous_sibling) {
                // Increase current child ratio, decrease previous sibling ratio
                const auto& previous_sibling = m_Children[child_idx - 1];
                const auto& current_child = m_Children[child_idx];

                const float new_current_ratio = current_child->ratio + delta_ratio;
                const float new_previous_ratio = previous_sibling->ratio - delta_ratio;

                if (new_current_ratio >= 0.05f && new_previous_ratio >= 0.05f) {
                    current_child->ratio = new_current_ratio;
                    previous_sibling->ratio = new_previous_ratio;
                }
            }
            else if (m_Parent) {
                m_Parent->localUpdate(m_NodeID, pixelDeltaSize, direction);
            }
        }
        else if (hasFlag(direction, Bottom)) {
            if (has_next_sibling) {
                // Increase current child ratio, decrease next sibling ratio
                const auto& next_sibling = m_Children[child_idx + 1];
                const auto& current_child = m_Children[child_idx];

                const float new_current_ratio = current_child->ratio + delta_ratio;
                const float new_next_ratio = next_sibling->ratio - delta_ratio;

                if (new_current_ratio >= 0.05f && new_next_ratio >= 0.05f) {
                    current_child->ratio = new_current_ratio;
                    next_sibling->ratio = new_next_ratio;
                }
            }
            else if (m_Parent) {
                m_Parent->localUpdate(m_NodeID, pixelDeltaSize, direction);
            }
        }
    }

    /////////////////////// HorizontalSplit Implementation ///////////////////////

    HorizontalSplit::HorizontalSplit(const NodeID nodeID, LayoutNode *parent):
    LayoutNode(nodeID, parent) {

    }

    void HorizontalSplit::ratioToPixel(const ImVec2 &position, const ImVec2 &size) {
        LayoutNode::ratioToPixel(position, size);

        float currentX = position.x;
        for (const auto& child : m_Children) {
            const float childWidth = std::ceil(size.x * child->ratio);
            child->ratioToPixel(ImVec2(currentX, position.y), ImVec2(childWidth, size.y));
            currentX += childWidth;
        }
    }

    void HorizontalSplit::localUpdate(const NodeID childID, const ImVec2 &pixelDeltaSize, const ResizeDirection direction) {
 if (hasFlag(direction, Top) or hasFlag(direction, Bottom)) {
            if (m_Parent) {
                m_Parent->localUpdate(m_NodeID, pixelDeltaSize, direction);
            }
        }
        // 1. Determine child idx
        Size child_idx = 0;
        bool has_previous_sibling = false;
        bool has_next_sibling = false;
        for (Size i = 0; i < m_Children.size(); i++) {
            if (m_Children[i]->m_NodeID == childID) {
                child_idx = i;
                has_previous_sibling = (i > 0);
                has_next_sibling = (i < m_Children.size() - 1);
            }
        }
        // 2. Determine how the ratio changed
        float delta_ratio = pixelDeltaSize.x / size.x;

        // 3. update ratios
        if (hasFlag(direction, Left)) {
            if (has_previous_sibling) {
                // Increase current child ratio, decrease previous sibling ratio
                const auto& previous_sibling = m_Children[child_idx - 1];
                const auto& current_child = m_Children[child_idx];

                const float new_current_ratio = current_child->ratio + delta_ratio;
                const float new_previous_ratio = previous_sibling->ratio - delta_ratio;

                if (new_current_ratio >= 0.05f && new_previous_ratio >= 0.05f) {
                    current_child->ratio = new_current_ratio;
                    previous_sibling->ratio = new_previous_ratio;
                }
            }
            else if (m_Parent) {
                m_Parent->localUpdate(m_NodeID, pixelDeltaSize, direction);
            }
        }
        else if (hasFlag(direction, Right)) {
            if (has_next_sibling) {
                // Increase current child ratio, decrease next sibling ratio
                const auto& next_sibling = m_Children[child_idx + 1];
                const auto& current_child = m_Children[child_idx];

                const float new_current_ratio = current_child->ratio + delta_ratio;
                const float new_next_ratio = next_sibling->ratio - delta_ratio;

                if (new_current_ratio >= 0.05f && new_next_ratio >= 0.05f) {
                    current_child->ratio = new_current_ratio;
                    next_sibling->ratio = new_next_ratio;
                }
            }
            else if (m_Parent) {
                m_Parent->localUpdate(m_NodeID, pixelDeltaSize, direction);
            }
        }
    }
}
