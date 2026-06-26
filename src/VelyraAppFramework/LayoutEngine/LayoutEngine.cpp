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
        m_Layouts[layoutID] = createLayoutNode(layout.getLayout(), nullptr);
        m_Layouts[layoutID]->calculateUnknownRatio();
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

    UP<LayoutNode> LayoutEngine::createLayoutNode(const SP<Node> &nodeDesc, LayoutNode *parent) {
        switch (nodeDesc->type) {
            case NodeType::Panel: {
                const auto panelDesc = static_cast<PanelNode*>(nodeDesc.get());
                return createUP<PanelImpl>(panelDesc->id, parent, panelDesc->desc);
            }
            case NodeType::HorizontalSplit: {
                const auto splitDesc = static_cast<Split*>(nodeDesc.get());
                auto splitNode = createUP<HorizontalSplit>(splitDesc->id, parent);
                for (const auto& childDesc : splitDesc->children) {
                    splitNode->addChild(createLayoutNode(childDesc, splitNode.get()));
                }
                return splitNode;
            }
            case NodeType::VerticalSplit: {
                const auto splitDesc = static_cast<Split*>(nodeDesc.get());
                auto splitNode = createUP<VerticalSplit>(splitDesc->id, parent);
                for (const auto& childDesc : splitDesc->children) {
                    splitNode->addChild(createLayoutNode(childDesc, splitNode.get()));
                }
                return splitNode;
            }
        }
        VL_THROW("Unknown node type in layout description!");
    }
}
