#pragma once

#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>
#include <VelyraUtils/Logging/LoggingFwd.hpp>

#include <VelyraUtils/DevUtils/PrivateMemberGetter.hpp>

#include <string_view>
#include <unordered_map>

namespace Velyra::App {

    struct LayoutNode;

    /**
     * @brief Calculates and draws a layout tree. The tree is meant to be submitted every frame, so it can change freely
     *        between frames. The only state kept between frames are the ratios of nodes the user resized, these are
     *        identified by the path of names from the root to the node (see NodeID).
     */
    class VL_API LayoutEngine {
    public:
        LayoutEngine();

        ~LayoutEngine();

        /**
         * @brief Calculates the layout of the given tree for the given layout box and draws all its panels. Must be
         *        called between the ImGui begin and end of a frame.
         * @param root The root of the layout tree
         * @param xPos The x position of the layout box, relative to the work area of the main viewport
         * @param yPos The y position of the layout box, relative to the work area of the main viewport
         * @param width The width of the layout box
         * @param height The height of the layout box
         */
        void draw(const Node& root, float xPos, float yPos, float width, float height);

        /**
         * @brief Draws the given layout tree in the work area of the main viewport (the window minus the main menu
         *        bar). Must be called between the ImGui begin and end of a frame.
         */
        void draw(const Node& root);

        /**
         * @brief Only calculates the position and size of every node of the given tree, without drawing. This is done
         *        by draw() as well. The tree does not need to outlive this call.
         */
        void calculate(const Node& root, float xPos, float yPos, float width, float height);

        /**
         * @brief Resizes a panel of the last calculated layout, as if the user dragged one of its borders. Takes effect
         *        on the next calculate() or draw().
         * @param panelName The name of the panel to resize
         * @param deltaWidth The change in width in pixels
         * @param deltaHeight The change in height in pixels
         * @param direction The borders that were dragged, e.g. Left | Bottom
         */
        void resizePanel(std::string_view panelName, float deltaWidth, float deltaHeight, ResizeDirection direction);

        /**
         * @brief Forgets all resizes done by the user, so all nodes go back to the ratios of the layout description.
         */
        void resetResizes();

    private:
        struct NodeState {
            float ratio = DONT_CARE; // Ratio after the user resized the node
            float descRatio = DONT_CARE; // Ratio of the description at the time of resizing, if the description changes, the resize is discarded
        };

        void calculateChildren(Size nodeIndex);

        void distributeRatios(Size firstChild, Size childCount);

        void calculateChildRects(Size nodeIndex);

        void drawPanel(Size nodeIndex);

        void resizeNode(Size nodeIndex, float deltaPixels, bool horizontal, bool towardsStart);

    private:
        Utils::LogPtr m_Logger;

        // Flattened tree of the last calculate() call, the children of a node are stored contiguously
        std::vector<LayoutNode> m_Nodes;
        std::unordered_map<NodeID, NodeState> m_NodeStates;

        VL_GET_PRIVATE_MEMBER(LayoutEngine, m_Nodes)
        VL_GET_PRIVATE_MEMBER(LayoutEngine, m_NodeStates)
    };

}
