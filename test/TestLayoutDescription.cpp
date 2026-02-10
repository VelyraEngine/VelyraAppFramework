#include "TestPch.hpp"

#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>

using namespace Velyra;
using namespace Velyra::App;

class TestLayoutDescription: public ::testing::Test {
};

TEST_F(TestLayoutDescription, CreatePanel) {
    const Panel panel{
        .name = "Test Panel",
        .resizable = false,
        .collapsible = true,
        .sizeRatio = 0.5f
    };

    auto panelNode = createPanel(panel);

    ASSERT_EQ(panelNode->type, NodeType::Panel);
    const auto* panelNodePtr = dynamic_cast<PanelNode*>(panelNode.get());
    ASSERT_EQ(panelNodePtr->desc.name, "Test Panel");
    ASSERT_FALSE(panelNodePtr->desc.resizable);
    ASSERT_TRUE(panelNodePtr->desc.collapsible);
    ASSERT_FLOAT_EQ(panelNodePtr->desc.sizeRatio, 0.5f);
}

TEST_F(TestLayoutDescription, Layout1) {
    auto layout = createLayout(
        horizontalSplit(
            verticalSplit(
                createPanel({.name = "A", .sizeRatio = 0.2f}),
                createPanel({.name = "B", .sizeRatio = 0.4f}),
                createPanel({.name = "C", .sizeRatio = 0.4f})
            ),
            createPanel({.name = "D", .sizeRatio = 0.6f}),
            verticalSplit(
                createPanel({.name = "E", .sizeRatio = 0.3f}),
                createPanel({.name =  "F", .sizeRatio = 0.7f})
            )
        )
    );

    auto layoutNode = dynamic_cast<Split*>(layout.get());
    ASSERT_EQ(layoutNode->type, NodeType::HorizontalSplit);
    ASSERT_EQ(layoutNode->children.size(), 3);

    auto firstChild = dynamic_cast<Split*>(layoutNode->children[0].get());
    ASSERT_EQ(firstChild->type, NodeType::VerticalSplit);
    ASSERT_EQ(firstChild->children.size(), 3);
    auto panelA = dynamic_cast<PanelNode*>(firstChild->children[0].get());
    ASSERT_EQ(panelA->type, NodeType::Panel);
    ASSERT_EQ(panelA->desc.name, "A");
    ASSERT_FLOAT_EQ(panelA->desc.sizeRatio, 0.2f);
    auto panelB = dynamic_cast<PanelNode*>(firstChild->children[1].get());
    ASSERT_EQ(panelB->type, NodeType::Panel);
    ASSERT_EQ(panelB->desc.name, "B");
    ASSERT_FLOAT_EQ(panelB->desc.sizeRatio, 0.4f);
    auto panelC = dynamic_cast<PanelNode*>(firstChild->children[2].get());
    ASSERT_EQ(panelC->type, NodeType::Panel);
    ASSERT_EQ(panelC->desc.name, "C");
    ASSERT_FLOAT_EQ(panelC->desc.sizeRatio, 0.4f);

    auto panelD = dynamic_cast<PanelNode*>(layoutNode->children[1].get());
    ASSERT_EQ(panelD->type, NodeType::Panel);
    ASSERT_EQ(panelD->desc.name, "D");
    ASSERT_FLOAT_EQ(panelD->desc.sizeRatio, 0.6f);

    auto secondChild = dynamic_cast<Split*>(layoutNode->children[2].get());
    ASSERT_EQ(secondChild->type, NodeType::VerticalSplit);
    ASSERT_EQ(secondChild->children.size(), 2);
    auto panelE = dynamic_cast<PanelNode*>(secondChild->children[0].get());
    ASSERT_EQ(panelE->type, NodeType::Panel);
    ASSERT_EQ(panelE->desc.name, "E");
    ASSERT_FLOAT_EQ(panelE->desc.sizeRatio, 0.3f);
    auto panelF = dynamic_cast<PanelNode*>(secondChild->children[1].get());
    ASSERT_EQ(panelF->type, NodeType::Panel);
    ASSERT_EQ(panelF->desc.name, "F");
    ASSERT_FLOAT_EQ(panelF->desc.sizeRatio, 0.7f);
}