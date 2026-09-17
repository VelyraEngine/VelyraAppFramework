#include <VelyraAppFramework/LayoutEngine/LayoutNode.hpp>

#include "VelyraAppFramework/LoggerNames.hpp"

namespace Velyra::App {

    namespace {

        inline bool PIXEL_EQUAL(const ImVec2& a, const ImVec2& b) {
            constexpr float epsilon = 1.0f; // 1 pixel tolerance
            return (std::abs(a.x - b.x) < epsilon) && (std::abs(a.y - b.y) < epsilon);
        }

        inline ImVec2 PIXEL_CEIL(const ImVec2& vec) {
            return {std::ceil(vec.x), std::ceil(vec.y)};
        }

        float clampPx(float px, const Sizing& s) {
            if (s.minPx > DONT_CARE && px < s.minPx) px = s.minPx;
            if (s.maxPx > DONT_CARE && px > s.maxPx) px = s.maxPx;
            return px;
        }

        /**
         * @brief Resolves the pixel size of every child along a split axis given the axis length and the children's
         *        sizings. Fixed-pixel children are reserved first, the remaining space is distributed among ratio
         *        children by weight (DONT_CARE ratios share the leftover equally), min/max clamps are honoured with a
         *        re-flow pass, and an over-constrained axis is shrunk proportionally to fit.
         */
        std::vector<float> solveSizes(const float axis, const std::vector<Sizing>& items) {
            const Size n = items.size();
            std::vector<float> out(n, 0.0f);
            std::vector<bool> locked(n, false);

            // 1. Reserve fixed-pixel children.
            float fixedTotal = 0.0f;
            for (Size i = 0; i < n; ++i) {
                if (items[i].mode == SizeMode::Pixels && items[i].value > DONT_CARE) {
                    const float px = clampPx(items[i].value, items[i]);
                    out[i] = px;
                    locked[i] = true;
                    fixedTotal += px;
                }
            }

            // 2. Resolve ratio weights, auto-distributing the unknown (DONT_CARE) ratios.
            float knownRatio = 0.0f;
            Size autoCount = 0;
            for (Size i = 0; i < n; ++i) {
                if (locked[i]) continue;
                if (items[i].value > DONT_CARE) knownRatio += items[i].value;
                else autoCount++;
            }
            const float autoRatio = (autoCount > 0) ? std::max(0.0f, 1.0f - knownRatio) / static_cast<float>(autoCount) : 0.0f;

            std::vector<float> weight(n, 0.0f);
            for (Size i = 0; i < n; ++i) {
                if (locked[i]) continue;
                weight[i] = (items[i].value > DONT_CARE) ? items[i].value : autoRatio;
            }

            // 3. Distribute the remaining space among ratio children, locking any that hit a clamp and re-flowing.
            bool changed = true;
            while (changed) {
                changed = false;
                const float remaining = std::max(0.0f, axis - fixedTotal);
                float weightSum = 0.0f;
                for (Size i = 0; i < n; ++i) if (!locked[i]) weightSum += weight[i];
                if (weightSum <= 0.0f) break;

                for (Size i = 0; i < n; ++i) {
                    if (locked[i]) continue;
                    const float px = remaining * (weight[i] / weightSum);
                    const float clamped = clampPx(px, items[i]);
                    out[i] = clamped;
                    if (std::abs(clamped - px) > 0.001f) {
                        locked[i] = true;
                        fixedTotal += clamped;
                        changed = true;
                        break; // re-flow the remaining unlocked children
                    }
                }
            }

            // 4. Shrink proportionally if clamps pushed the total past the axis length.
            float total = 0.0f;
            for (const float v : out) total += v;
            if (total > axis + 0.5f && total > 0.0f) {
                const float scale = axis / total;
                for (float& v : out) v *= scale;
            }
            return out;
        }

        float minSizeOf(const LayoutNode& child, const float axis) {
            if (child.sizing.minPx > DONT_CARE) return child.sizing.minPx;
            return axis * 0.05f;
        }

        // Writes a new pixel size back into a child's live sizing, preserving whether it is pixel- or ratio-based.
        void writeBackSize(LayoutNode& child, const float newPx, const float axis) {
            if (child.sizing.mode == SizeMode::Pixels) {
                child.sizing.value = newPx;
            }
            else if (axis > 0.0f) {
                child.sizing.value = newPx / axis;
            }
        }

    }

    LayoutNode::LayoutNode(const NodeID nodeID, const NodeID reconcileKey, const NodeType type, LayoutNode *parent):
    m_NodeID(nodeID),
    m_ReconcileKey(reconcileKey),
    m_Type(type),
    m_Parent(parent){

    }

    void LayoutNode::ratioToPixel(const ImVec2 &pixelPosition, const ImVec2 &pixelSize) {
        position = PIXEL_CEIL(pixelPosition);
        size = PIXEL_CEIL(pixelSize);
    }

    /////////////////////// PanelImpl Implementation ///////////////////////

    PanelImpl::PanelImpl(const NodeID nodeID, const NodeID reconcileKey, LayoutNode *parent, const Panel &desc):
    LayoutNode(nodeID, reconcileKey, NodeType::Panel, parent),
    m_Panel(desc){

    }

    void PanelImpl::draw(Core::Window &window, Core::Context &context) {
        beginPanel();
        m_Panel.drawFunction(window, context);
        endPanel();
    }

    void PanelImpl::beginPanel() const {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowPos(ImVec2(position.x + viewport->WorkPos.x, position.y + viewport->WorkPos.y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking;
        if (!m_Panel.resizable) {
            window_flags |= ImGuiWindowFlags_NoResize;
        }
        if (!m_Panel.collapsible) {
            window_flags |= ImGuiWindowFlags_NoCollapse;
        }
        ImGui::Begin(m_Panel.name.c_str(), nullptr, window_flags);
    }

    void PanelImpl::endPanel() const {
        const ImVec2 newSize = ImGui::GetWindowSize();
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const ImVec2 newPos = ImGui::GetWindowPos();

        // Convert ImGui's absolute window position back into our local layout space
        // so that comparisons are done in the same coordinate system.
        const ImVec2 localNewPos = ImVec2(newPos.x - viewport->WorkPos.x, newPos.y - viewport->WorkPos.y);

        // Only handle actual resizes / layout changes
        if (!PIXEL_EQUAL(size, newSize) || !PIXEL_EQUAL(position, localNewPos)) {
            /*
             * delta_size.x > 0  => increased left      delta_pos.x > 0  => increased right
             * delta_size.x < 0  => decreased left      delta_pos.x < 0  => decreased left
             * delta_size.y > 0  => increased bottom    delta_pos.y > 0  => decreased top
             * delta_size.y < 0  => decreased bottom    delta_pos.y < 0  => increased top
             */
            const auto deltaSize = ImVec2(newSize.x - size.x, newSize.y - size.y);
            const auto deltaPos = ImVec2(localNewPos.x - position.x, localNewPos.y - position.y);

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

    VerticalSplit::VerticalSplit(const NodeID nodeID, const NodeID reconcileKey, LayoutNode *parent):
    LayoutNode(nodeID, reconcileKey, NodeType::VerticalSplit, parent) {

    }

    void VerticalSplit::ratioToPixel(const ImVec2 &pos, const ImVec2 &newSize) {
        LayoutNode::ratioToPixel(pos, newSize);

        std::vector<Sizing> items;
        items.reserve(m_Children.size());
        for (const auto& child : m_Children) items.push_back(child->sizing);
        const std::vector<float> heights = solveSizes(newSize.y, items);

        float currentY = pos.y;
        for (Size i = 0; i < m_Children.size(); ++i) {
            const float childHeight = std::ceil(heights[i]);
            m_Children[i]->ratioToPixel(ImVec2(pos.x, currentY), ImVec2(newSize.x, childHeight));
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
        // 2. The drag delta along this split's axis
        const float axis = size.y;
        const float deltaPx = pixelDeltaSize.y;

        // 3. Move the border, preserving each neighbour's sizing mode and honouring its minimum size
        if (hasFlag(direction, Top)) {
            if (has_previous_sibling) {
                const auto& previous_sibling = m_Children[child_idx - 1];
                const auto& current_child = m_Children[child_idx];

                const float new_current_px = current_child->size.y + deltaPx;
                const float new_previous_px = previous_sibling->size.y - deltaPx;

                if (new_current_px >= minSizeOf(*current_child, axis) && new_previous_px >= minSizeOf(*previous_sibling, axis)) {
                    writeBackSize(*current_child, new_current_px, axis);
                    writeBackSize(*previous_sibling, new_previous_px, axis);
                }
            }
            else if (m_Parent) {
                m_Parent->localUpdate(m_NodeID, pixelDeltaSize, direction);
            }
        }
        else if (hasFlag(direction, Bottom)) {
            if (has_next_sibling) {
                const auto& next_sibling = m_Children[child_idx + 1];
                const auto& current_child = m_Children[child_idx];

                const float new_current_px = current_child->size.y + deltaPx;
                const float new_next_px = next_sibling->size.y - deltaPx;

                if (new_current_px >= minSizeOf(*current_child, axis) && new_next_px >= minSizeOf(*next_sibling, axis)) {
                    writeBackSize(*current_child, new_current_px, axis);
                    writeBackSize(*next_sibling, new_next_px, axis);
                }
            }
            else if (m_Parent) {
                m_Parent->localUpdate(m_NodeID, pixelDeltaSize, direction);
            }
        }
    }

    /////////////////////// HorizontalSplit Implementation ///////////////////////

    HorizontalSplit::HorizontalSplit(const NodeID nodeID, const NodeID reconcileKey, LayoutNode *parent):
    LayoutNode(nodeID, reconcileKey, NodeType::HorizontalSplit, parent) {

    }

    void HorizontalSplit::ratioToPixel(const ImVec2 &pos, const ImVec2 &newSize) {
        LayoutNode::ratioToPixel(pos, newSize);

        std::vector<Sizing> items;
        items.reserve(m_Children.size());
        for (const auto& child : m_Children) items.push_back(child->sizing);
        const std::vector<float> widths = solveSizes(newSize.x, items);

        float currentX = pos.x;
        for (Size i = 0; i < m_Children.size(); ++i) {
            const float childWidth = std::ceil(widths[i]);
            m_Children[i]->ratioToPixel(ImVec2(currentX, pos.y), ImVec2(childWidth, newSize.y));
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
        // 2. The drag delta along this split's axis
        const float axis = size.x;
        const float deltaPx = pixelDeltaSize.x;

        // 3. Move the border, preserving each neighbour's sizing mode and honouring its minimum size
        if (hasFlag(direction, Left)) {
            if (has_previous_sibling) {
                const auto& previous_sibling = m_Children[child_idx - 1];
                const auto& current_child = m_Children[child_idx];

                const float new_current_px = current_child->size.x + deltaPx;
                const float new_previous_px = previous_sibling->size.x - deltaPx;

                if (new_current_px >= minSizeOf(*current_child, axis) && new_previous_px >= minSizeOf(*previous_sibling, axis)) {
                    writeBackSize(*current_child, new_current_px, axis);
                    writeBackSize(*previous_sibling, new_previous_px, axis);
                }
            }
            else if (m_Parent) {
                m_Parent->localUpdate(m_NodeID, pixelDeltaSize, direction);
            }
        }
        else if (hasFlag(direction, Right)) {
            if (has_next_sibling) {
                const auto& next_sibling = m_Children[child_idx + 1];
                const auto& current_child = m_Children[child_idx];

                const float new_current_px = current_child->size.x + deltaPx;
                const float new_next_px = next_sibling->size.x - deltaPx;

                if (new_current_px >= minSizeOf(*current_child, axis) && new_next_px >= minSizeOf(*next_sibling, axis)) {
                    writeBackSize(*current_child, new_current_px, axis);
                    writeBackSize(*next_sibling, new_next_px, axis);
                }
            }
            else if (m_Parent) {
                m_Parent->localUpdate(m_NodeID, pixelDeltaSize, direction);
            }
        }
    }
}
