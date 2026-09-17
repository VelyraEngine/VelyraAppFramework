#include "TestPch.hpp"

#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>
#include <../include/VelyraAppFramework/LayoutEngine/LayoutEngine.hpp>
#include "../src/VelyraAppFramework/LayoutEngine/LayoutNode.hpp"

using namespace Velyra;
using namespace Velyra::App;

class TestLayoutEngine: public ::testing::Test {
};

class LayoutTester: public Layout {
public:
    explicit LayoutTester(LayoutID id, SP<Node> layout): Layout(id), m_Layout(layout) {}

    SP<Node> getLayout() override {
        return m_Layout;
    }

private:
    SP<Node> m_Layout;
};

TEST_F(TestLayoutEngine, UnknownRatioVerticalSmallLayout) {
    const auto layout = createLayout(
        verticalSplit(
            createPanel({.name = "A", .sizeRatio = 0.2f}),
            createPanel({.name = "B", .sizeRatio = 0.4f}),
            createPanel({.name = "C", .sizeRatio = 0.4f})
        )
    );
    LayoutTester layoutTester(1, layout);

    LayoutEngine engine;
    engine.registerLayout(layoutTester);

    auto& l = engine.get_m_Layouts().at(layoutTester.getID());
    l->ratioToPixel(ImVec2(0.0f, 0.0f), ImVec2(1000.0f, 1000.0f));

    ASSERT_EQ(l->getChildren().size(), 3);

    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.y, 200.0f);
    auto panelA = dynamic_cast<PanelImpl*>(l->getChildren()[0].get());
    ASSERT_NE(panelA, nullptr);
    EXPECT_EQ(panelA->getName(), "A");

    EXPECT_FLOAT_EQ(l->getChildren()[1]->size.y, 400.0f);
    auto panelB = dynamic_cast<PanelImpl*>(l->getChildren()[1].get());
    ASSERT_NE(panelB, nullptr);
    EXPECT_EQ(panelB->getName(), "B");

    EXPECT_FLOAT_EQ(l->getChildren()[2]->size.y, 400.0f);
    auto panelC = dynamic_cast<PanelImpl*>(l->getChildren()[2].get());
    ASSERT_NE(panelC, nullptr);
    EXPECT_EQ(panelC->getName(), "C");
}

TEST_F(TestLayoutEngine, UnknownRatioVerticalSmallLayoutUnknownRatios) {
    auto layout = createLayout(
        verticalSplit(
            createPanel({.name = "A", .sizeRatio = 0.4f}),
            createPanel({.name = "B"}), // Dont set ratio, should be 0.3 -> 300px
            createPanel({.name = "C"}) // Dont set ratio, should be 0.3 -> 300px
        )
    );
    LayoutTester layoutTester(1, layout);

    LayoutEngine engine;
    engine.registerLayout(layoutTester);

    auto& l = engine.get_m_Layouts().at(layoutTester.getID());
    l->ratioToPixel(ImVec2(0.0f, 0.0f), ImVec2(1000.0f, 1000.0f));

    ASSERT_EQ(l->getChildren().size(), 3);

    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.y, 400.0f);
    auto panelA = dynamic_cast<PanelImpl*>(l->getChildren()[0].get());
    ASSERT_NE(panelA, nullptr);
    EXPECT_EQ(panelA->getName(), "A");

    EXPECT_FLOAT_EQ(l->getChildren()[1]->size.y, 300.0f);
    auto panelB = dynamic_cast<PanelImpl*>(l->getChildren()[1].get());
    ASSERT_NE(panelB, nullptr);
    EXPECT_EQ(panelB->getName(), "B");

    EXPECT_FLOAT_EQ(l->getChildren()[2]->size.y, 300.0f);
    auto panelC = dynamic_cast<PanelImpl*>(l->getChildren()[2].get());
    ASSERT_NE(panelC, nullptr);
    EXPECT_EQ(panelC->getName(), "C");
}

TEST_F(TestLayoutEngine, UnknownRatioHorizontalSmallLayout) {
    const auto layout = createLayout(
        horizontalSplit(
            createPanel({.name = "A", .sizeRatio = 0.2f}),
            createPanel({.name = "B", .sizeRatio = 0.4f}),
            createPanel({.name = "C", .sizeRatio = 0.4f})
        )
    );
    LayoutTester layoutTester(1, layout);

    LayoutEngine engine;
    engine.registerLayout(layoutTester);

    auto& l = engine.get_m_Layouts().at(layoutTester.getID());
    l->ratioToPixel(ImVec2(0.0f, 0.0f), ImVec2(1000.0f, 1000.0f));

    ASSERT_EQ(l->getChildren().size(), 3);

    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.x, 200.0f);
    auto panelA = dynamic_cast<PanelImpl*>(l->getChildren()[0].get());
    ASSERT_NE(panelA, nullptr);
    EXPECT_EQ(panelA->getName(), "A");

    EXPECT_FLOAT_EQ(l->getChildren()[1]->size.x, 400.0f);
    auto panelB = dynamic_cast<PanelImpl*>(l->getChildren()[1].get());
    ASSERT_NE(panelB, nullptr);
    EXPECT_EQ(panelB->getName(), "B");

    EXPECT_FLOAT_EQ(l->getChildren()[2]->size.x, 400.0f);
    auto panelC = dynamic_cast<PanelImpl*>(l->getChildren()[2].get());
    ASSERT_NE(panelC, nullptr);
    EXPECT_EQ(panelC->getName(), "C");
}

TEST_F(TestLayoutEngine, UnknownRatioNestedLayout) {
    const auto layout = createLayout(
        verticalSplit(
            horizontalSplit(
                createPanel({.name = "A", .sizeRatio = 0.4f}),
                createPanel({.name = "B"}) // Dont set ratio, should be 0.6 -> 600px
            ),
            horizontalSplit(
                createPanel({.name = "C", .sizeRatio = 0.5f}),
                createPanel({.name = "D", .sizeRatio = 0.5f})
            )
        )
    );
    LayoutTester layoutTester(1, layout);

    LayoutEngine engine;
    engine.registerLayout(layoutTester);

    auto& l = engine.get_m_Layouts().at(layoutTester.getID());
    l->ratioToPixel(ImVec2(0.0f, 0.0f), ImVec2(1000.0f, 1000.0f));

    ASSERT_EQ(l->getChildren().size(), 2);

    // First horizontal split fills the top half (auto/auto -> 500px each)
    auto horizontalSplit1 = dynamic_cast<HorizontalSplit*>(l->getChildren()[0].get());
    ASSERT_NE(horizontalSplit1, nullptr);
    EXPECT_FLOAT_EQ(horizontalSplit1->size.y, 500.0f);
    EXPECT_FLOAT_EQ(horizontalSplit1->getChildren()[0]->size.x, 400.0f);
    auto panelA = dynamic_cast<PanelImpl*>(horizontalSplit1->getChildren()[0].get());
    ASSERT_NE(panelA, nullptr);
    EXPECT_EQ(panelA->getName(), "A");
    EXPECT_FLOAT_EQ(horizontalSplit1->getChildren()[1]->size.x, 600.0f);
    auto panelB = dynamic_cast<PanelImpl*>(horizontalSplit1->getChildren()[1].get());
    ASSERT_NE(panelB, nullptr);
    EXPECT_EQ(panelB->getName(), "B");

    // Second horizontal split fills the bottom half
    auto horizontalSplit2 = dynamic_cast<HorizontalSplit*>(l->getChildren()[1].get());
    ASSERT_NE(horizontalSplit2, nullptr);
    EXPECT_FLOAT_EQ(horizontalSplit2->size.y, 500.0f);
    EXPECT_FLOAT_EQ(horizontalSplit2->getChildren()[0]->size.x, 500.0f);
    auto panelC = dynamic_cast<PanelImpl*>(horizontalSplit2->getChildren()[0].get());
    ASSERT_NE(panelC, nullptr);
    EXPECT_EQ(panelC->getName(), "C");
    EXPECT_FLOAT_EQ(horizontalSplit2->getChildren()[1]->size.x, 500.0f);
    auto panelD = dynamic_cast<PanelImpl*>(horizontalSplit2->getChildren()[1].get());
    ASSERT_NE(panelD, nullptr);
    EXPECT_EQ(panelD->getName(), "D");
}

TEST_F(TestLayoutEngine, RatioToPixelVerticalSmallLayout) {
    const auto layout = createLayout(
        verticalSplit(
            createPanel({.name = "A", .sizeRatio = 0.2f}),
            createPanel({.name = "B", .sizeRatio = 0.4f}),
            createPanel({.name = "C", .sizeRatio = 0.4f})
        )
    );

    LayoutTester layoutTester(1, layout);

    LayoutEngine engine;
    engine.registerLayout(layoutTester);
    engine.setActiveLayout(layoutTester.getID());

    engine.get_m_Layouts().at(layoutTester.getID())->ratioToPixel(ImVec2(10.0f, 10.0f), ImVec2(1000.0f, 1000.0f));

    auto& l = engine.get_m_Layouts().at(layoutTester.getID());
    EXPECT_FLOAT_EQ(l->getChildren()[0]->position.x, 10.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[0]->position.y, 10.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.x, 1000.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.y, 200.0f);

    EXPECT_FLOAT_EQ(l->getChildren()[1]->position.x, 10.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[1]->position.y, 210.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[1]->size.x, 1000.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[1]->size.y, 400.0f);

    EXPECT_FLOAT_EQ(l->getChildren()[2]->position.x, 10.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[2]->position.y, 610.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[2]->size.x, 1000.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[2]->size.y, 400.0f);
}

TEST_F(TestLayoutEngine, RatioToPixelHorizontalSmallLayout) {
    const auto layout = createLayout(
        horizontalSplit(
            createPanel({.name = "A", .sizeRatio = 0.2f}),
            createPanel({.name = "B", .sizeRatio = 0.4f}),
            createPanel({.name = "C", .sizeRatio = 0.4f})
        )
    );

    LayoutTester layoutTester(1, layout);

    LayoutEngine engine;
    engine.registerLayout(layoutTester);
    engine.setActiveLayout(layoutTester.getID());

    engine.get_m_Layouts().at(layoutTester.getID())->ratioToPixel(ImVec2(10.0f, 10.0f), ImVec2(1000.0f, 1000.0f));

    auto& l = engine.get_m_Layouts().at(layoutTester.getID());
    EXPECT_FLOAT_EQ(l->getChildren()[0]->position.x, 10.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[0]->position.y, 10.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.x, 200.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.y, 1000.0f);

    EXPECT_FLOAT_EQ(l->getChildren()[1]->position.x, 210.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[1]->position.y, 10.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[1]->size.x, 400.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[1]->size.y, 1000.0f);

    EXPECT_FLOAT_EQ(l->getChildren()[2]->position.x, 610.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[2]->position.y, 10.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[2]->size.x, 400.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[2]->size.y, 1000.0f);
}

TEST_F(TestLayoutEngine, RatioToPixelNestedLayout) {
    const auto layout = createLayout(
        verticalSplit(
            horizontalSplit(
                createPanel({.name = "A", .sizeRatio = 0.4f}),
                createPanel({.name = "B", .sizeRatio = 0.6f})
            ),
            horizontalSplit(
                createPanel({.name = "C", .sizeRatio = 0.5f}),
                createPanel({.name = "D", .sizeRatio = 0.5f})
            )
        )
    );

    LayoutTester layoutTester(1, layout);

    LayoutEngine engine;
    engine.registerLayout(layoutTester);
    engine.setActiveLayout(layoutTester.getID());

    engine.get_m_Layouts().at(layoutTester.getID())->ratioToPixel(ImVec2(10.0f, 10.0f), ImVec2(1000.0f, 1000.0f));

    auto& l = engine.get_m_Layouts().at(layoutTester.getID());

    // First horizontal split
    auto horizontalSplit1 = dynamic_cast<HorizontalSplit*>(l->getChildren()[0].get());
    ASSERT_NE(horizontalSplit1, nullptr);
    EXPECT_FLOAT_EQ(horizontalSplit1->getChildren()[0]->position.x, 10.0f);
    EXPECT_FLOAT_EQ(horizontalSplit1->getChildren()[0]->position.y, 10.0f);
    EXPECT_FLOAT_EQ(horizontalSplit1->getChildren()[0]->size.x, 400.0f);
    EXPECT_FLOAT_EQ(horizontalSplit1->getChildren()[0]->size.y, 500.0f);
    EXPECT_FLOAT_EQ(horizontalSplit1->getChildren()[1]->position.x, 410.0f);
    EXPECT_FLOAT_EQ(horizontalSplit1->getChildren()[1]->position.y, 10.0f);
    EXPECT_FLOAT_EQ(horizontalSplit1->getChildren()[1]->size.x, 600.0f);
    EXPECT_FLOAT_EQ(horizontalSplit1->getChildren()[1]->size.y, 500.0f);

    // Second horizontal split
    auto horizontalSplit2 = dynamic_cast<HorizontalSplit*>(l->getChildren()[1].get());
    ASSERT_NE(horizontalSplit2, nullptr);
    EXPECT_FLOAT_EQ(horizontalSplit2->getChildren()[0]->position.x, 10.0f);
    EXPECT_FLOAT_EQ(horizontalSplit2->getChildren()[0]->position.y, 510.0f);
    EXPECT_FLOAT_EQ(horizontalSplit2->getChildren()[0]->size.x, 500.0f);
    EXPECT_FLOAT_EQ(horizontalSplit2->getChildren()[0]->size.y, 500.0f);
    EXPECT_FLOAT_EQ(horizontalSplit2->getChildren()[1]->position.x, 510.0f);
    EXPECT_FLOAT_EQ(horizontalSplit2->getChildren()[1]->position.y, 510.0f);
    EXPECT_FLOAT_EQ(horizontalSplit2->getChildren()[1]->size.x, 500.0f);
    EXPECT_FLOAT_EQ(horizontalSplit2->getChildren()[1]->size.y, 500.0f);
}

// A layout source whose described tree can be swapped between frames, to exercise reconciliation.
class MutableLayout: public Layout {
public:
    explicit MutableLayout(const LayoutID id): Layout(id) {}

    SP<Node> getLayout() override { return m_Layout; }

    SP<Node> m_Layout;
};

TEST_F(TestLayoutEngine, SplitDescRatioMakesMiddleColumnWidest) {
    const auto layout = createLayout(
        horizontalSplit(
            verticalSplit({.sizeRatio = 0.25f}, createPanel({.name = "L"})),
            verticalSplit({.sizeRatio = 0.5f}, createPanel({.name = "M"})),
            verticalSplit({.sizeRatio = 0.25f}, createPanel({.name = "R"}))
        )
    );
    LayoutTester layoutTester(1, layout);

    LayoutEngine engine;
    engine.registerLayout(layoutTester);

    auto& l = engine.get_m_Layouts().at(layoutTester.getID());
    l->ratioToPixel(ImVec2(0.0f, 0.0f), ImVec2(1000.0f, 1000.0f));

    ASSERT_EQ(l->getChildren().size(), 3);
    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.x, 250.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[1]->size.x, 500.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[2]->size.x, 250.0f);
}

TEST_F(TestLayoutEngine, SplitDescRatioMiddleWithAutoSiblings) {
    // Only the middle column gets an explicit ratio; the two siblings auto-share the remainder.
    const auto layout = createLayout(
        horizontalSplit(
            verticalSplit(createPanel({.name = "L"})),
            verticalSplit({.sizeRatio = 0.5f}, createPanel({.name = "M"})),
            verticalSplit(createPanel({.name = "R"}))
        )
    );
    LayoutTester layoutTester(1, layout);

    LayoutEngine engine;
    engine.registerLayout(layoutTester);

    auto& l = engine.get_m_Layouts().at(layoutTester.getID());
    l->ratioToPixel(ImVec2(0.0f, 0.0f), ImVec2(1000.0f, 1000.0f));

    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.x, 250.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[1]->size.x, 500.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[2]->size.x, 250.0f);
}

TEST_F(TestLayoutEngine, FixedPixelRowThenAutoFill) {
    const auto layout = createLayout(
        verticalSplit(
            createPanel({.name = "Bar", .sizing = pixels(40.0f)}),
            createPanel({.name = "Body"})
        )
    );
    LayoutTester layoutTester(1, layout);

    LayoutEngine engine;
    engine.registerLayout(layoutTester);

    auto& l = engine.get_m_Layouts().at(layoutTester.getID());
    l->ratioToPixel(ImVec2(0.0f, 0.0f), ImVec2(1000.0f, 1000.0f));

    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.y, 40.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[1]->size.y, 960.0f);
}

TEST_F(TestLayoutEngine, MinPixelClampEnforced) {
    const auto layout = createLayout(
        verticalSplit(
            createPanel({.name = "A", .sizing = ratio(0.05f, 100.0f)}), // 0.05 -> 50px, clamped up to 100px
            createPanel({.name = "B", .sizeRatio = 0.95f})
        )
    );
    LayoutTester layoutTester(1, layout);

    LayoutEngine engine;
    engine.registerLayout(layoutTester);

    auto& l = engine.get_m_Layouts().at(layoutTester.getID());
    l->ratioToPixel(ImVec2(0.0f, 0.0f), ImVec2(1000.0f, 1000.0f));

    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.y, 100.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[1]->size.y, 900.0f);
}

TEST_F(TestLayoutEngine, MaxPixelClampRedistributes) {
    const auto layout = createLayout(
        verticalSplit(
            createPanel({.name = "A", .sizing = ratio(0.5f, DONT_CARE, 200.0f)}), // 0.5 -> 500px, clamped down to 200px
            createPanel({.name = "B", .sizeRatio = 0.5f})
        )
    );
    LayoutTester layoutTester(1, layout);

    LayoutEngine engine;
    engine.registerLayout(layoutTester);

    auto& l = engine.get_m_Layouts().at(layoutTester.getID());
    l->ratioToPixel(ImVec2(0.0f, 0.0f), ImVec2(1000.0f, 1000.0f));

    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.y, 200.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[1]->size.y, 800.0f);
}

TEST_F(TestLayoutEngine, ReconcilePreservesUserAdjustedSize) {
    MutableLayout layout(1);
    layout.m_Layout = createLayout(
        verticalSplit(
            createPanel({.name = "A", .sizeRatio = 0.5f}),
            createPanel({.name = "B", .sizeRatio = 0.5f})
        )
    );

    LayoutEngine engine;
    engine.registerLayout(layout);

    auto& l = engine.get_m_Layouts().at(layout.getID());
    l->ratioToPixel(ImVec2(0.0f, 0.0f), ImVec2(1000.0f, 1000.0f));
    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.y, 500.0f);

    // Simulate a user drag by adjusting the live sizing directly.
    l->getChildren()[0]->sizing.value = 0.7f;
    l->getChildren()[1]->sizing.value = 0.3f;

    // Re-supply the identical description: user-adjusted sizes must survive reconciliation.
    layout.m_Layout = createLayout(
        verticalSplit(
            createPanel({.name = "A", .sizeRatio = 0.5f}),
            createPanel({.name = "B", .sizeRatio = 0.5f})
        )
    );
    engine.updateLayout(layout);
    l->ratioToPixel(ImVec2(0.0f, 0.0f), ImVec2(1000.0f, 1000.0f));
    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.y, 700.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[1]->size.y, 300.0f);

    // Change the ratio in code: the new value is adopted over the user's adjustment.
    layout.m_Layout = createLayout(
        verticalSplit(
            createPanel({.name = "A", .sizeRatio = 0.2f}),
            createPanel({.name = "B", .sizeRatio = 0.8f})
        )
    );
    engine.updateLayout(layout);
    l->ratioToPixel(ImVec2(0.0f, 0.0f), ImVec2(1000.0f, 1000.0f));
    EXPECT_FLOAT_EQ(l->getChildren()[0]->size.y, 200.0f);
    EXPECT_FLOAT_EQ(l->getChildren()[1]->size.y, 800.0f);
}

TEST_F(TestLayoutEngine, ReconcileAddsRemovesAndPreservesIdentity) {
    MutableLayout layout(1);
    layout.m_Layout = createLayout(
        horizontalSplit(
            createPanel({.name = "A"}),
            createPanel({.name = "B"})
        )
    );

    LayoutEngine engine;
    engine.registerLayout(layout);

    auto& l = engine.get_m_Layouts().at(layout.getID());
    ASSERT_EQ(l->getChildren().size(), 2);
    const LayoutNode* aBefore = l->getChildren()[0].get();

    // Add a new panel C: A and B are reused, C is created.
    layout.m_Layout = createLayout(
        horizontalSplit(
            createPanel({.name = "A"}),
            createPanel({.name = "B"}),
            createPanel({.name = "C"})
        )
    );
    engine.updateLayout(layout);
    ASSERT_EQ(l->getChildren().size(), 3);
    EXPECT_EQ(l->getChildren()[0].get(), aBefore);
    auto* panelC = dynamic_cast<PanelImpl*>(l->getChildren()[2].get());
    ASSERT_NE(panelC, nullptr);
    EXPECT_EQ(panelC->getName(), "C");

    // Remove B: only A and C remain, A still the same instance.
    layout.m_Layout = createLayout(
        horizontalSplit(
            createPanel({.name = "A"}),
            createPanel({.name = "C"})
        )
    );
    engine.updateLayout(layout);
    ASSERT_EQ(l->getChildren().size(), 2);
    EXPECT_EQ(l->getChildren()[0].get(), aBefore);
    EXPECT_EQ(dynamic_cast<PanelImpl*>(l->getChildren()[0].get())->getName(), "A");
    EXPECT_EQ(dynamic_cast<PanelImpl*>(l->getChildren()[1].get())->getName(), "C");
}
