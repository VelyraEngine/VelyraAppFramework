#include "TestPch.hpp"

#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutEngine.hpp>

using namespace Velyra;
using namespace Velyra::App;

class TestLayoutEngine: public ::testing::Test {
};

TEST_F(TestLayoutEngine, CalculateUnknownRatioVerticalSmallLayout) {
    const auto layout = createLayout(
        verticalSplit(
            createPanel({.name = "A", .sizeRatio = 0.2f}),
            createPanel({.name = "B", .sizeRatio = 0.4f}),
            createPanel({.name = "C", .sizeRatio = 0.4f})
        )
    );

    LayoutEngine engine;
    engine.registerLayout("TEST", layout);

    auto& l = engine.get_m_Layouts().at("TEST");

    EXPECT_FLOAT_EQ(l->ratio, 1.0f);

    EXPECT_FLOAT_EQ(l->getChildren()[0]->ratio, 0.2f);
    auto panelA = dynamic_cast<PanelImpl*>(l->getChildren()[0].get());
    ASSERT_NE(panelA, nullptr);
    EXPECT_EQ(panelA->getName(), "A");

    EXPECT_FLOAT_EQ(l->getChildren()[1]->ratio, 0.4f);
    auto panelB = dynamic_cast<PanelImpl*>(l->getChildren()[1].get());
    ASSERT_NE(panelB, nullptr);
    EXPECT_EQ(panelB->getName(), "B");

    EXPECT_FLOAT_EQ(l->getChildren()[2]->ratio, 0.4f);
    auto panelC = dynamic_cast<PanelImpl*>(l->getChildren()[2].get());
    ASSERT_NE(panelC, nullptr);
    EXPECT_EQ(panelC->getName(), "C");
}

TEST_F(TestLayoutEngine, CalculateUnknownRatioVerticalSmallLayoutUnknownRatios) {
    auto layout = createLayout(
        verticalSplit(
            createPanel({.name = "A", .sizeRatio = 0.4f}),
            createPanel({.name = "B"}), // Dont set ratio, should be 0.3f
            createPanel({.name = "C"}) // Dont set ratio, should be 0.3f
        )
    );

    LayoutEngine engine;
    engine.registerLayout("TEST", layout);

    auto& l = engine.get_m_Layouts().at("TEST");

    EXPECT_FLOAT_EQ(l->ratio, 1.0f);
    ASSERT_EQ(l->getChildren().size(), 3);

    EXPECT_FLOAT_EQ(l->getChildren()[0]->ratio, 0.4f);
    auto panelA = dynamic_cast<PanelImpl*>(l->getChildren()[0].get());
    ASSERT_NE(panelA, nullptr);
    EXPECT_EQ(panelA->getName(), "A");

    EXPECT_FLOAT_EQ(l->getChildren()[1]->ratio, 0.3f);
    auto panelB = dynamic_cast<PanelImpl*>(l->getChildren()[1].get());
    ASSERT_NE(panelB, nullptr);
    EXPECT_EQ(panelB->getName(), "B");

    EXPECT_FLOAT_EQ(l->getChildren()[2]->ratio, 0.3f);
    auto panelC = dynamic_cast<PanelImpl*>(l->getChildren()[2].get());
    ASSERT_NE(panelC, nullptr);
    EXPECT_EQ(panelC->getName(), "C");
}

TEST_F(TestLayoutEngine, CalculateUnknownRatioHorizontalSmallLayout) {
    const auto layout = createLayout(
        horizontalSplit(
            createPanel({.name = "A", .sizeRatio = 0.2f}),
            createPanel({.name = "B", .sizeRatio = 0.4f}),
            createPanel({.name = "C", .sizeRatio = 0.4f})
        )
    );

    LayoutEngine engine;
    engine.registerLayout("TEST", layout);

    auto& l = engine.get_m_Layouts().at("TEST");

    EXPECT_FLOAT_EQ(l->ratio, 1.0f);

    EXPECT_FLOAT_EQ(l->getChildren()[0]->ratio, 0.2f);
    auto panelA = dynamic_cast<PanelImpl*>(l->getChildren()[0].get());
    ASSERT_NE(panelA, nullptr);
    EXPECT_EQ(panelA->getName(), "A");

    EXPECT_FLOAT_EQ(l->getChildren()[1]->ratio, 0.4f);
    auto panelB = dynamic_cast<PanelImpl*>(l->getChildren()[1].get());
    ASSERT_NE(panelB, nullptr);
    EXPECT_EQ(panelB->getName(), "B");

    EXPECT_FLOAT_EQ(l->getChildren()[2]->ratio, 0.4f);
    auto panelC = dynamic_cast<PanelImpl*>(l->getChildren()[2].get());
    ASSERT_NE(panelC, nullptr);
    EXPECT_EQ(panelC->getName(), "C");
}

TEST_F(TestLayoutEngine, CalculateUnknownRatioHorizontalSmallLayoutUnknownRatios) {
    auto layout = createLayout(
        verticalSplit(
            createPanel({.name = "A", .sizeRatio = 0.4f}),
            createPanel({.name = "B"}), // Dont set ratio, should be 0.3f
            createPanel({.name = "C"}) // Dont set ratio, should be 0.3f
        )
    );

    LayoutEngine engine;
    engine.registerLayout("TEST", layout);

    auto& l = engine.get_m_Layouts().at("TEST");

    EXPECT_FLOAT_EQ(l->ratio, 1.0f);
    ASSERT_EQ(l->getChildren().size(), 3);

    EXPECT_FLOAT_EQ(l->getChildren()[0]->ratio, 0.4f);
    auto panelA = dynamic_cast<PanelImpl*>(l->getChildren()[0].get());
    ASSERT_NE(panelA, nullptr);
    EXPECT_EQ(panelA->getName(), "A");

    EXPECT_FLOAT_EQ(l->getChildren()[1]->ratio, 0.3f);
    auto panelB = dynamic_cast<PanelImpl*>(l->getChildren()[1].get());
    ASSERT_NE(panelB, nullptr);
    EXPECT_EQ(panelB->getName(), "B");

    EXPECT_FLOAT_EQ(l->getChildren()[2]->ratio, 0.3f);
    auto panelC = dynamic_cast<PanelImpl*>(l->getChildren()[2].get());
    ASSERT_NE(panelC, nullptr);
    EXPECT_EQ(panelC->getName(), "C");
}

TEST_F(TestLayoutEngine, CalculateUnknownRatioNestedLayout) {
    const auto layout = createLayout(
        verticalSplit(
            horizontalSplit(
                createPanel({.name = "A", .sizeRatio = 0.4f}),
                createPanel({.name = "B"}) // Dont set ratio, should be 0.6f
            ),
            horizontalSplit(
                createPanel({.name = "C", .sizeRatio = 0.5f}),
                createPanel({.name = "D", .sizeRatio = 0.5f})
            )
        )
    );

    LayoutEngine engine;
    engine.registerLayout("TEST", layout);

    auto& l = engine.get_m_Layouts().at("TEST");

    EXPECT_FLOAT_EQ(l->ratio, 1.0f);
    ASSERT_EQ(l->getChildren().size(), 2);

    // First horizontal split
    auto horizontalSplit1 = dynamic_cast<HorizontalSplit*>(l->getChildren()[0].get());
    ASSERT_NE(horizontalSplit1, nullptr);
    EXPECT_FLOAT_EQ(horizontalSplit1->ratio, 0.5f);
    EXPECT_FLOAT_EQ(horizontalSplit1->getChildren()[0]->ratio, 0.4f);
    auto panelA = dynamic_cast<PanelImpl*>(horizontalSplit1->getChildren()[0].get());
    ASSERT_NE(panelA, nullptr);
    EXPECT_EQ(panelA->getName(), "A");
    EXPECT_FLOAT_EQ(horizontalSplit1->getChildren()[1]->ratio, 0.6f);
    auto panelB = dynamic_cast<PanelImpl*>(horizontalSplit1->getChildren()[1].get());
    ASSERT_NE(panelB, nullptr);
    EXPECT_EQ(panelB->getName(), "B");

    // Second horizontal split
    auto horizontalSplit2 = dynamic_cast<HorizontalSplit*>(l->getChildren()[1].get());
    ASSERT_NE(horizontalSplit2, nullptr);
    EXPECT_FLOAT_EQ(horizontalSplit2->ratio, 0.5f);
    EXPECT_FLOAT_EQ(horizontalSplit2->getChildren()[0]->ratio, 0.5f);
    auto panelC = dynamic_cast<PanelImpl*>(horizontalSplit2->getChildren()[0].get());
    ASSERT_NE(panelC, nullptr);
    EXPECT_EQ(panelC->getName(), "C");
    EXPECT_FLOAT_EQ(horizontalSplit2->getChildren()[1]->ratio, 0.5f);
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

    LayoutEngine engine;
    engine.registerLayout("TEST", layout);
    engine.setActiveLayout("TEST");

    engine.calculateLayout(10.0f, 10.0f, 1000.0f, 1000.0f);

    auto& l = engine.get_m_Layouts().at("TEST");
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

    LayoutEngine engine;
    engine.registerLayout("TEST", layout);
    engine.setActiveLayout("TEST");

    engine.calculateLayout(10.0f, 10.0f, 1000.0f, 1000.0f);

    auto& l = engine.get_m_Layouts().at("TEST");
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

    LayoutEngine engine;
    engine.registerLayout("TEST", layout);
    engine.setActiveLayout("TEST");

    engine.calculateLayout(10.0f, 10.0f, 1000.0f, 1000.0f);

    auto& l = engine.get_m_Layouts().at("TEST");

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
