#include "TestPch.hpp"

#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutEngine.hpp>
#include <VelyraCore/ImGui/imgui_internal.h>

using namespace Velyra;
using namespace Velyra::App;

/**
 * @brief Runs ImGui frames without a window or renderer, so the dock layout ImGui calculates can be checked
 */
class TestLayoutDocking: public ::testing::Test {
protected:
    static constexpr float WIDTH = 1000.0f;
    static constexpr float HEIGHT = 800.0f;
    static constexpr float TOLERANCE = 12.0f; // Splitters and borders take a few pixels

    void SetUp() override {
        m_Context = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
        io.DisplaySize = ImVec2(WIDTH, HEIGHT);
        io.DeltaTime = 1.0f / 60.0f;
    }

    void TearDown() override {
        ImGui::DestroyContext(m_Context);
    }

    /**
     * @brief Draws the layout for a few frames, docking needs a frame or two before every window has its final rect
     */
    void draw(const SP<Node>& root, const int frames = 3) {
        for (int i = 0; i < frames; i++) {
            ImGui::NewFrame();
            const ImGuiID dockSpaceID = ImGui::DockSpaceOverViewport();
            engine.drawDocked(*root, dockSpaceID);
            ImGui::Render();
        }
    }

    static ImGuiWindow* window(const std::string& name) {
        ImGuiWindow* result = ImGui::FindWindowByName(name.c_str());
        if (result == nullptr) {
            throw std::runtime_error("Window '" + name + "' not found");
        }
        return result;
    }

    static ImRect rect(const std::string& name) {
        const ImGuiWindow* w = window(name);
        return {w->Pos, ImVec2(w->Pos.x + w->Size.x, w->Pos.y + w->Size.y)};
    }

    static bool isDocked(const std::string& name) {
        return window(name)->DockNode != nullptr && window(name)->DockIsActive;
    }

    static void expectSameRect(const ImRect& a, const ImRect& b) {
        EXPECT_NEAR(a.Min.x, b.Min.x, 1.0f);
        EXPECT_NEAR(a.Min.y, b.Min.y, 1.0f);
        EXPECT_NEAR(a.Max.x, b.Max.x, 1.0f);
        EXPECT_NEAR(a.Max.y, b.Max.y, 1.0f);
    }

    static SP<Node> panel(const std::string& name, const float ratio = DONT_CARE) {
        return createPanel({.name = name, .sizeRatio = ratio});
    }

    ImGuiContext* m_Context = nullptr;
    LayoutEngine engine;
};

TEST_F(TestLayoutDocking, TreeIsBuiltOnFirstFrame) {
    const SP<Node> layout = horizontalSplit(
        panel("A", 0.2f),
        verticalSplit(panel("B", 0.75f), panel("C", 0.25f))
    );
    draw(layout);

    for (const std::string name: {"A", "B", "C"}) {
        EXPECT_TRUE(isDocked(name)) << name;
    }
    EXPECT_NEAR(rect("A").Min.x, 0.0f, TOLERANCE);
    EXPECT_NEAR(rect("A").GetWidth(), 0.2f * WIDTH, TOLERANCE);
    EXPECT_NEAR(rect("B").Min.x, 0.2f * WIDTH, TOLERANCE);
    EXPECT_NEAR(rect("B").GetHeight(), 0.75f * HEIGHT, TOLERANCE);
    EXPECT_NEAR(rect("C").Min.y, 0.75f * HEIGHT, TOLERANCE);
    EXPECT_NEAR(rect("C").Max.x, WIDTH, TOLERANCE);

    // The largest panel holds the central node
    EXPECT_TRUE(window("B")->DockNode->IsCentralNode());
}

TEST_F(TestLayoutDocking, SplitWithMoreThanTwoChildren) {
    const SP<Node> layout = verticalSplit(panel("A", 0.2f), panel("B", 0.5f), panel("C", 0.3f));
    draw(layout);

    EXPECT_NEAR(rect("A").GetHeight(), 0.2f * HEIGHT, TOLERANCE);
    EXPECT_NEAR(rect("B").GetHeight(), 0.5f * HEIGHT, TOLERANCE);
    EXPECT_NEAR(rect("C").GetHeight(), 0.3f * HEIGHT, TOLERANCE);
    EXPECT_LT(rect("A").Min.y, rect("B").Min.y);
    EXPECT_LT(rect("B").Min.y, rect("C").Min.y);
}

TEST_F(TestLayoutDocking, PanelComesBackAtTheSamePlace) {
    draw(horizontalSplit(panel("A", 0.3f), panel("B", 0.7f)));
    const ImRect before = rect("A");

    draw(horizontalSplit(panel("B", 0.7f)));
    EXPECT_NEAR(rect("B").Min.x, 0.0f, TOLERANCE) << "B takes the space A left";

    draw(horizontalSplit(panel("A", 0.3f), panel("B", 0.7f)));
    EXPECT_TRUE(isDocked("A"));
    expectSameRect(rect("A"), before);
}

TEST_F(TestLayoutDocking, NewPanelIsDockedNextToItsSibling) {
    draw(horizontalSplit(panel("Navigator", 0.2f), panel("Log", 0.8f)));
    const ImRect navigator = rect("Navigator");

    draw(horizontalSplit(panel("Navigator", 0.2f), verticalSplit(panel("Log", 0.75f), panel("Matches", 0.25f))));

    EXPECT_TRUE(isDocked("Matches"));
    expectSameRect(rect("Navigator"), navigator);
    EXPECT_NEAR(rect("Matches").Min.x, rect("Log").Min.x, 1.0f);
    EXPECT_NEAR(rect("Matches").Min.y, rect("Log").Max.y, TOLERANCE);
    EXPECT_NEAR(rect("Matches").GetHeight(), 0.25f * HEIGHT, TOLERANCE);
}

TEST_F(TestLayoutDocking, ReplacingDocumentTakesTheCentralNode) {
    draw(horizontalSplit(panel("Navigator", 0.2f), panel("Document 1", 0.8f)));
    const ImRect document = rect("Document 1");

    draw(horizontalSplit(panel("Navigator", 0.2f), panel("Document 2", 0.8f)));
    EXPECT_TRUE(isDocked("Document 2"));
    expectSameRect(rect("Document 2"), document);

    draw(horizontalSplit(panel("Navigator", 0.2f), panel("Document 1", 0.8f)));
    EXPECT_TRUE(isDocked("Document 1"));
    expectSameRect(rect("Document 1"), document);
}

TEST_F(TestLayoutDocking, HiddenPanelStaysDockedWhenItsNodeIsSplit) {
    draw(horizontalSplit(panel("Navigator", 0.2f), panel("Document 1", 0.8f)));
    const ImRect document = rect("Document 1");

    // Document 2 replaces document 1, then its node is split while document 1 is not submitted
    draw(horizontalSplit(panel("Navigator", 0.2f), panel("Document 2", 0.8f)));
    draw(horizontalSplit(panel("Navigator", 0.2f), verticalSplit(panel("Document 2", 0.75f), panel("Matches 2", 0.25f))));

    draw(horizontalSplit(panel("Navigator", 0.2f), panel("Document 1", 0.8f)));
    EXPECT_TRUE(isDocked("Document 1"));
    expectSameRect(rect("Document 1"), document);
}

TEST_F(TestLayoutDocking, FloatingPanelIsLeftAlone) {
    const SP<Node> layout = horizontalSplit(panel("A", 0.3f), panel("B", 0.7f));
    draw(layout);

    ImGui::DockContextQueueUndockWindow(m_Context, window("A"));
    draw(layout);
    EXPECT_FALSE(isDocked("A"));

    draw(layout);
    EXPECT_FALSE(isDocked("A")) << "The user made it float, it must not be docked again";
}

TEST_F(TestLayoutDocking, ResetDocksEverythingAgain) {
    const SP<Node> layout = horizontalSplit(panel("A", 0.3f), panel("B", 0.7f));
    draw(layout);
    const ImRect before = rect("A");

    ImGui::DockContextQueueUndockWindow(m_Context, window("A"));
    draw(layout);
    ASSERT_FALSE(isDocked("A"));

    engine.resetResizes();
    draw(layout);
    EXPECT_FALSE(engine.get_m_ResetDockSpace());
    EXPECT_TRUE(isDocked("A"));
    EXPECT_TRUE(isDocked("B"));
    expectSameRect(rect("A"), before);
}

TEST_F(TestLayoutDocking, NullChildrenAreSkipped) {
    draw(horizontalSplit(panel("A", 0.3f), nullptr, panel("B", 0.7f)));

    EXPECT_TRUE(isDocked("A"));
    EXPECT_TRUE(isDocked("B"));
    EXPECT_NEAR(rect("A").GetWidth(), 0.3f * WIDTH, TOLERANCE);
}

TEST_F(TestLayoutDocking, PanelAsRoot) {
    draw(panel("A"));

    EXPECT_TRUE(isDocked("A"));
    EXPECT_NEAR(rect("A").GetWidth(), WIDTH, TOLERANCE);
    EXPECT_NEAR(rect("A").GetHeight(), HEIGHT, TOLERANCE);
}

TEST_F(TestLayoutDocking, NewPanelBeforeItsSibling) {
    draw(horizontalSplit(panel("Log", 0.8f)));
    draw(horizontalSplit(panel("Navigator", 0.2f), panel("Log", 0.8f)));

    EXPECT_TRUE(isDocked("Navigator"));
    EXPECT_NEAR(rect("Navigator").Min.x, 0.0f, TOLERANCE);
    EXPECT_NEAR(rect("Navigator").GetWidth(), 0.2f * WIDTH, TOLERANCE);
    EXPECT_NEAR(rect("Log").Min.x, rect("Navigator").Max.x, TOLERANCE);
}

TEST_F(TestLayoutDocking, PanelIsOnlyDrawnOnceItHasItsSize) {
    // A docked window is hidden on its first frame, it only gets the size of its dock node on the next frame. Content
    // that sizes itself once (e.g. a child with ImGuiChildFlags_ResizeX) would remember the wrong size.
    std::vector<float> widths;
    const SP<Node> layout = horizontalSplit(panel("A", 0.3f), createPanel({
        .name = "B",
        .drawFunction = [&widths] { widths.push_back(ImGui::GetContentRegionAvail().x); },
        .sizeRatio = 0.7f
    }));
    draw(layout, 3);

    ASSERT_FALSE(widths.empty());
    EXPECT_NEAR(widths.front(), widths.back(), 1.0f);
    EXPECT_GT(widths.front(), 0.6f * WIDTH);
}
