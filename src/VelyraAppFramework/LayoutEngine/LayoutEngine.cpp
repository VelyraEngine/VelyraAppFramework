#include <Pch.hpp>

#include <VelyraAppFramework/LayoutEngine/LayoutEngine.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutNode.hpp>
#include "../LoggerNames.hpp"

namespace Velyra::App {

    LayoutEngine::LayoutEngine():
    m_Logger(Utils::getLogger(VL_APP_LAYOUT_LOGGER)){

    }

    void LayoutEngine::registerLayout(const std::string &name, const UP<Node>& layout) {
        m_Layouts[name] = createLayoutNode(layout, nullptr);
        m_Layouts[name]->calculateUnknownRatio();
    }

    void LayoutEngine::setActiveLayout(const std::string &name) {
        const auto layoutIt = m_Layouts.find(name);
        if (layoutIt == m_Layouts.end()) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Layout '{}' not found. Register first a layout for this name!", name);
            return;
        }
        m_ActiveLayout = name;
    }

    void LayoutEngine::beginPanel(const std::string &name) const {
        const auto layoutIt = m_Layouts.find(m_ActiveLayout);
        if (layoutIt == m_Layouts.end()) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Layout '{}' not found. Register first a layout for this name!", name);
            return;
        }
        layoutIt->second->beginPanel(name);
    }

    void LayoutEngine::endPanel(const std::string &name) const {
        const auto layoutIt = m_Layouts.find(m_ActiveLayout);
        if (layoutIt == m_Layouts.end()) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Layout '{}' not found. Register first a layout for this name!", name);
            return;
        }
        layoutIt->second->endPanel(name);
    }

    void LayoutEngine::calculateLayout(const float xPos, const float yPos, const float windowWidth, const float windowHeight) const {
        const auto layoutIt = m_Layouts.find(m_ActiveLayout);
        if (layoutIt == m_Layouts.end()) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Layout '{}' not found. Register first a layout for this name!", m_ActiveLayout);
            return;
        }
        layoutIt->second->ratioToPixel(ImVec2(xPos, yPos), ImVec2(windowWidth, windowHeight));
    }

    UP<LayoutNode> LayoutEngine::createLayoutNode(const UP<Node> &nodeDesc, LayoutNode *parent) {
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
