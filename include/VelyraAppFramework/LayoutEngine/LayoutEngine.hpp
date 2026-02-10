#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutNode.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>

#include <VelyraUtils/DevUtils/PrivateMemberGetter.hpp>

#include <unordered_map>

namespace Velyra::App {

    class VL_API LayoutEngine {
    public:
        LayoutEngine();

        ~LayoutEngine() = default;

        void registerLayout(const std::string& name, const UP<Node>& layout);

        void setActiveLayout(const std::string& name);

        /**
         * @brief Calculate the current active layout based on the given window dimensions and position. This should
         *        be called every frame before rendering the panels.
         * @param xPos The x position of the entire layout box (usually the window position)
         * @param yPos The y position of the entire layout box (usually the window position)
         * @param windowWidth The width of the entire layout box (usually the window width)
         * @param windowHeight The height of the entire layout box (usually the window height)
         */
        void calculateLayout(float xPos, float yPos, float windowWidth, float windowHeight) const;

        void beginPanel(const std::string& name) const;

        void endPanel(const std::string& name) const;

        const std::string& getActiveLayout() const { return m_ActiveLayout; }

    private:

        UP<LayoutNode> createLayoutNode(const UP<Node>& nodeDesc, LayoutNode* parent);

    private:
        Utils::LogPtr m_Logger;

        std::string m_ActiveLayout;
        std::unordered_map<std::string, UP<LayoutNode>> m_Layouts;

        VL_GET_PRIVATE_MEMBER(LayoutEngine, m_Layouts);
    };



}
