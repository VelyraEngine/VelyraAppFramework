#include "TestPch.hpp"

#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutEngine.hpp>
#include "../src/VelyraAppFramework/LayoutEngine/LayoutNode.hpp"

using namespace Velyra;
using namespace Velyra::App;

class TestLayoutEngine: public ::testing::Test {
protected:
    static LayoutNode findNode(const LayoutEngine& engine, const std::string& name) {
        for (const auto& node : engine.get_m_Nodes()) {
            if (node.nameHash == hashName(name)) {
                return node;
            }
        }
        throw std::runtime_error("Node '" + name + "' not found");
    }

    static void expectRect(const LayoutEngine& engine, const std::string& name, const float x, const float y, const float width, const float height) {
        const LayoutNode node = findNode(engine, name);
        EXPECT_FLOAT_EQ(node.position.x, x) << name;
        EXPECT_FLOAT_EQ(node.position.y, y) << name;
        EXPECT_FLOAT_EQ(node.size.x, width) << name;
        EXPECT_FLOAT_EQ(node.size.y, height) << name;
    }

    LayoutEngine engine;
};

TEST_F(TestLayoutEngine, CalculateRatioVerticalSmallLayout) {
    const auto layout = verticalSplit(
        createPanel({.name = "A", .sizeRatio = 0.2f}),
        createPanel({.name = "B", .sizeRatio = 0.4f}),
        createPanel({.name = "C", .sizeRatio = 0.4f})
    );
    engine.calculate(*layout, 0.0f, 0.0f, 1000.0f, 1000.0f);

    ASSERT_EQ(engine.get_m_Nodes().size(), 4);
    EXPECT_FLOAT_EQ(engine.get_m_Nodes()[0].ratio, 1.0f);
    EXPECT_FLOAT_EQ(findNode(engine, "A").ratio, 0.2f);
    EXPECT_FLOAT_EQ(findNode(engine, "B").ratio, 0.4f);
    EXPECT_FLOAT_EQ(findNode(engine, "C").ratio, 0.4f);
}

TEST_F(TestLayoutEngine, CalculateUnknownRatios) {
    const auto layout = verticalSplit(
        createPanel({.name = "A", .sizeRatio = 0.4f}),
        createPanel({.name = "B"}), // Dont set ratio, should be 0.3f
        createPanel({.name = "C"})  // Dont set ratio, should be 0.3f
    );
    engine.calculate(*layout, 0.0f, 0.0f, 1000.0f, 1000.0f);

    EXPECT_FLOAT_EQ(findNode(engine, "A").ratio, 0.4f);
    EXPECT_FLOAT_EQ(findNode(engine, "B").ratio, 0.3f);
    EXPECT_FLOAT_EQ(findNode(engine, "C").ratio, 0.3f);
}

TEST_F(TestLayoutEngine, CalculateRatiosNormalized) {
    // Ratios that do not sum up to 1 are scaled, so the panels still fill the whole box
    const auto layout = horizontalSplit(
        createPanel({.name = "A", .sizeRatio = 1.0f}),
        createPanel({.name = "B", .sizeRatio = 3.0f}),
        createPanel({.name = "C"}) // Nothing remains, gets the average of the known ratios
    );
    engine.calculate(*layout, 0.0f, 0.0f, 1000.0f, 1000.0f);

    EXPECT_FLOAT_EQ(findNode(engine, "A").ratio, 1.0f / 6.0f);
    EXPECT_FLOAT_EQ(findNode(engine, "B").ratio, 3.0f / 6.0f);
    EXPECT_FLOAT_EQ(findNode(engine, "C").ratio, 2.0f / 6.0f);
}

TEST_F(TestLayoutEngine, CalculateRatioNestedLayout) {
    const auto layout = verticalSplit(
        horizontalSplit({.name = "Top"},
            createPanel({.name = "A", .sizeRatio = 0.4f}),
            createPanel({.name = "B"}) // Dont set ratio, should be 0.6f
        ),
        horizontalSplit({.name = "Bottom"},
            createPanel({.name = "C", .sizeRatio = 0.5f}),
            createPanel({.name = "D", .sizeRatio = 0.5f})
        )
    );
    engine.calculate(*layout, 0.0f, 0.0f, 1000.0f, 1000.0f);

    EXPECT_FLOAT_EQ(findNode(engine, "Top").ratio, 0.5f);
    EXPECT_FLOAT_EQ(findNode(engine, "A").ratio, 0.4f);
    EXPECT_FLOAT_EQ(findNode(engine, "B").ratio, 0.6f);
    EXPECT_FLOAT_EQ(findNode(engine, "Bottom").ratio, 0.5f);
    EXPECT_FLOAT_EQ(findNode(engine, "C").ratio, 0.5f);
    EXPECT_FLOAT_EQ(findNode(engine, "D").ratio, 0.5f);
}

TEST_F(TestLayoutEngine, RatioToPixelVerticalSmallLayout) {
    const auto layout = verticalSplit(
        createPanel({.name = "A", .sizeRatio = 0.2f}),
        createPanel({.name = "B", .sizeRatio = 0.4f}),
        createPanel({.name = "C", .sizeRatio = 0.4f})
    );
    engine.calculate(*layout, 10.0f, 10.0f, 1000.0f, 1000.0f);

    expectRect(engine, "A", 10.0f, 10.0f, 1000.0f, 200.0f);
    expectRect(engine, "B", 10.0f, 210.0f, 1000.0f, 400.0f);
    expectRect(engine, "C", 10.0f, 610.0f, 1000.0f, 400.0f);
}

TEST_F(TestLayoutEngine, RatioToPixelHorizontalSmallLayout) {
    const auto layout = horizontalSplit(
        createPanel({.name = "A", .sizeRatio = 0.2f}),
        createPanel({.name = "B", .sizeRatio = 0.4f}),
        createPanel({.name = "C", .sizeRatio = 0.4f})
    );
    engine.calculate(*layout, 10.0f, 10.0f, 1000.0f, 1000.0f);

    expectRect(engine, "A", 10.0f, 10.0f, 200.0f, 1000.0f);
    expectRect(engine, "B", 210.0f, 10.0f, 400.0f, 1000.0f);
    expectRect(engine, "C", 610.0f, 10.0f, 400.0f, 1000.0f);
}

TEST_F(TestLayoutEngine, RatioToPixelNestedLayout) {
    const auto layout = verticalSplit(
        horizontalSplit(
            createPanel({.name = "A", .sizeRatio = 0.4f}),
            createPanel({.name = "B", .sizeRatio = 0.6f})
        ),
        horizontalSplit(
            createPanel({.name = "C", .sizeRatio = 0.5f}),
            createPanel({.name = "D", .sizeRatio = 0.5f})
        )
    );
    engine.calculate(*layout, 10.0f, 10.0f, 1000.0f, 1000.0f);

    expectRect(engine, "A", 10.0f, 10.0f, 400.0f, 500.0f);
    expectRect(engine, "B", 410.0f, 10.0f, 600.0f, 500.0f);
    expectRect(engine, "C", 10.0f, 510.0f, 500.0f, 500.0f);
    expectRect(engine, "D", 510.0f, 510.0f, 500.0f, 500.0f);
}

TEST_F(TestLayoutEngine, RatioToPixelNoGapsWithOddSizes) {
    const auto layout = horizontalSplit(
        createPanel({.name = "A"}),
        createPanel({.name = "B"}),
        createPanel({.name = "C"})
    );
    engine.calculate(*layout, 0.0f, 0.0f, 1001.0f, 100.0f);

    const auto a = findNode(engine, "A");
    const auto b = findNode(engine, "B");
    const auto c = findNode(engine, "C");
    EXPECT_FLOAT_EQ(a.position.x + a.size.x, b.position.x);
    EXPECT_FLOAT_EQ(b.position.x + b.size.x, c.position.x);
    EXPECT_FLOAT_EQ(c.position.x + c.size.x, 1001.0f);
}

TEST_F(TestLayoutEngine, SplitRatio) {
    // 3x3 grid where the columns and rows do not have the same size
    const auto layout = horizontalSplit(
        verticalSplit({.sizeRatio = 0.2f},
            createPanel({.name = "A", .sizeRatio = 0.5f}),
            createPanel({.name = "B"}),
            createPanel({.name = "C"})
        ),
        verticalSplit({.sizeRatio = 0.5f},
            createPanel({.name = "D"}),
            createPanel({.name = "E"}),
            createPanel({.name = "F"})
        ),
        verticalSplit( // Gets the remaining 0.3
            createPanel({.name = "G"}),
            createPanel({.name = "H"}),
            createPanel({.name = "I", .sizeRatio = 0.6f})
        )
    );
    engine.calculate(*layout, 0.0f, 0.0f, 1000.0f, 1000.0f);

    expectRect(engine, "A", 0.0f, 0.0f, 200.0f, 500.0f);
    expectRect(engine, "B", 0.0f, 500.0f, 200.0f, 250.0f);
    expectRect(engine, "C", 0.0f, 750.0f, 200.0f, 250.0f);
    expectRect(engine, "D", 200.0f, 0.0f, 500.0f, 333.0f);
    expectRect(engine, "G", 700.0f, 0.0f, 300.0f, 200.0f);
    expectRect(engine, "I", 700.0f, 400.0f, 300.0f, 600.0f);
}

TEST_F(TestLayoutEngine, NullChildrenAreSkipped) {
    const auto layout = horizontalSplit(
        createPanel({.name = "A"}),
        nullptr,
        createPanel({.name = "C"})
    );
    engine.calculate(*layout, 0.0f, 0.0f, 1000.0f, 1000.0f);

    ASSERT_EQ(engine.get_m_Nodes().size(), 3);
    expectRect(engine, "A", 0.0f, 0.0f, 500.0f, 1000.0f);
    expectRect(engine, "C", 500.0f, 0.0f, 500.0f, 1000.0f);
}

TEST_F(TestLayoutEngine, PanelAsRoot) {
    const auto layout = createPanel({.name = "A"});
    engine.calculate(*layout, 0.0f, 0.0f, 800.0f, 600.0f);

    ASSERT_EQ(engine.get_m_Nodes().size(), 1);
    expectRect(engine, "A", 0.0f, 0.0f, 800.0f, 600.0f);
}

TEST_F(TestLayoutEngine, NodeIDsStableBetweenFrames) {
    auto buildLayout = [] {
        return horizontalSplit(
            verticalSplit(
                createPanel({.name = "A"}),
                createPanel({.name = "B"})
            ),
            createPanel({.name = "C"})
        );
    };
    engine.calculate(*buildLayout(), 0.0f, 0.0f, 1000.0f, 1000.0f);
    std::vector<NodeID> firstIDs;
    for (const auto& node : engine.get_m_Nodes()) {
        firstIDs.push_back(node.id);
    }
    engine.calculate(*buildLayout(), 0.0f, 0.0f, 1000.0f, 1000.0f);
    std::vector<NodeID> secondIDs;
    for (const auto& node : engine.get_m_Nodes()) {
        secondIDs.push_back(node.id);
    }

    EXPECT_EQ(firstIDs, secondIDs);
    // All IDs are unique
    std::sort(firstIDs.begin(), firstIDs.end());
    EXPECT_EQ(std::adjacent_find(firstIDs.begin(), firstIDs.end()), firstIDs.end());
}

TEST_F(TestLayoutEngine, ResizeKeptWhenLayoutIsRebuilt) {
    auto buildLayout = [] {
        return horizontalSplit(
            createPanel({.name = "A", .sizeRatio = 0.5f}),
            createPanel({.name = "B", .sizeRatio = 0.5f})
        );
    };
    engine.calculate(*buildLayout(), 0.0f, 0.0f, 1000.0f, 1000.0f);
    engine.resizePanel("A", 100.0f, 0.0f, Right);
    ASSERT_EQ(engine.get_m_NodeStates().size(), 2);

    engine.calculate(*buildLayout(), 0.0f, 0.0f, 1000.0f, 1000.0f);
    expectRect(engine, "A", 0.0f, 0.0f, 600.0f, 1000.0f);
    expectRect(engine, "B", 600.0f, 0.0f, 400.0f, 1000.0f);

    engine.resetResizes();
    engine.calculate(*buildLayout(), 0.0f, 0.0f, 1000.0f, 1000.0f);
    expectRect(engine, "A", 0.0f, 0.0f, 500.0f, 1000.0f);
}

TEST_F(TestLayoutEngine, ResizeDiscardedWhenDescriptionChanges) {
    engine.calculate(*horizontalSplit(
        createPanel({.name = "A", .sizeRatio = 0.5f}),
        createPanel({.name = "B", .sizeRatio = 0.5f})
    ), 0.0f, 0.0f, 1000.0f, 1000.0f);
    engine.resizePanel("A", 100.0f, 0.0f, Right);
    ASSERT_EQ(engine.get_m_NodeStates().size(), 2);

    engine.calculate(*horizontalSplit(
        createPanel({.name = "A", .sizeRatio = 0.3f}),
        createPanel({.name = "B", .sizeRatio = 0.7f})
    ), 0.0f, 0.0f, 1000.0f, 1000.0f);
    expectRect(engine, "A", 0.0f, 0.0f, 300.0f, 1000.0f);
}

TEST_F(TestLayoutEngine, ResizeLeftBorderChangesPreviousSibling) {
    const auto layout = horizontalSplit(
        createPanel({.name = "A"}),
        createPanel({.name = "B"}),
        createPanel({.name = "C"}),
        createPanel({.name = "D"})
    );
    engine.calculate(*layout, 0.0f, 0.0f, 1000.0f, 1000.0f);
    engine.resizePanel("C", 50.0f, 0.0f, Left);
    engine.calculate(*layout, 0.0f, 0.0f, 1000.0f, 1000.0f);

    expectRect(engine, "A", 0.0f, 0.0f, 250.0f, 1000.0f);
    expectRect(engine, "B", 250.0f, 0.0f, 200.0f, 1000.0f);
    expectRect(engine, "C", 450.0f, 0.0f, 300.0f, 1000.0f);
    expectRect(engine, "D", 750.0f, 0.0f, 250.0f, 1000.0f);
}

TEST_F(TestLayoutEngine, ResizePropagatesToParentSplit) {
    // Dragging the right border of A moves the border between the left and right column
    const auto layout = horizontalSplit(
        verticalSplit({.name = "Left"},
            createPanel({.name = "A"}),
            createPanel({.name = "B"})
        ),
        createPanel({.name = "C"})
    );
    engine.calculate(*layout, 0.0f, 0.0f, 1000.0f, 1000.0f);
    engine.resizePanel("A", 100.0f, 0.0f, Right);
    engine.calculate(*layout, 0.0f, 0.0f, 1000.0f, 1000.0f);

    expectRect(engine, "A", 0.0f, 0.0f, 600.0f, 500.0f);
    expectRect(engine, "B", 0.0f, 500.0f, 600.0f, 500.0f);
    expectRect(engine, "C", 600.0f, 0.0f, 400.0f, 1000.0f);
}

TEST_F(TestLayoutEngine, ResizeOuterBorderIgnored) {
    const auto layout = horizontalSplit(
        createPanel({.name = "A"}),
        createPanel({.name = "B"})
    );
    engine.calculate(*layout, 0.0f, 0.0f, 1000.0f, 1000.0f);
    engine.resizePanel("A", 100.0f, 0.0f, Left);
    engine.resizePanel("B", 100.0f, 0.0f, Right);
    engine.calculate(*layout, 0.0f, 0.0f, 1000.0f, 1000.0f);

    EXPECT_TRUE(engine.get_m_NodeStates().empty());
    expectRect(engine, "A", 0.0f, 0.0f, 500.0f, 1000.0f);
}

TEST_F(TestLayoutEngine, ResizeClampedToMinimumRatio) {
    const auto layout = horizontalSplit(
        createPanel({.name = "A"}),
        createPanel({.name = "B"})
    );
    engine.calculate(*layout, 0.0f, 0.0f, 1000.0f, 1000.0f);
    engine.resizePanel("A", 490.0f, 0.0f, Right);
    engine.calculate(*layout, 0.0f, 0.0f, 1000.0f, 1000.0f);

    expectRect(engine, "A", 0.0f, 0.0f, 500.0f, 1000.0f);
}
