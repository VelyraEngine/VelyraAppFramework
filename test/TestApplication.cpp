#include "TestPch.hpp"

#include <VelyraAppFramework/Application.hpp>
#include <VelyraAppFramework/AppData.hpp>
#include <VelyraAppFramework/Layer.hpp>
#include <VelyraAppFramework/Layout.hpp>
#include <VelyraAppFramework/Widgets/Panel.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>
#include <VelyraCore/Window/Window.hpp>

#include <cstdlib>

using namespace Velyra;
using namespace Velyra::App;

namespace {

    // Order in which things happened, to check the lifetime of objects
    std::vector<std::string> g_Events;
    AppData* g_AppData = nullptr;

    class TrackedLayout: public Layout {
    public:
        ~TrackedLayout() override { g_Events.emplace_back("layout destroyed"); }

        SP<Node> getLayout() override { return nullptr; }
    };

    class EmptyPanel: public Widgets::Panel {
    public:
        explicit EmptyPanel(AppData& appData): Panel(appData, "Empty") {}

    protected:
        void drawContent() override {}
    };

    // Removes itself and adds enough panels to force a rehash of the panel map
    class SpawningPanel: public Widgets::Panel {
    public:
        explicit SpawningPanel(AppData& appData): Panel(appData, "Spawning") {}

    protected:
        void drawContent() override {
            m_AppData.removePanel(m_PanelID);
            for (int i = 0; i < 50; i++) {
                m_AppData.addPanel(createSP<EmptyPanel>(m_AppData));
            }
        }
    };

    class CountingPopup: public Widgets::Popup {
    public:
        explicit CountingPopup(AppData& appData): Popup(appData, "Counting") {}

        int openCount = 0;
        int closeCount = 0;

    protected:
        void drawContent() override {}

        void onOpen() override { openCount++; }

        void onClose() override { closeCount++; }
    };

    /**
     * @brief Registers a layout, a panel and a popup, and closes the window after the first frame
     */
    class TestLayer: public Layer {
    public:
        explicit TestLayer(AppData& appData): Layer(appData) {}

        ~TestLayer() override { g_Events.emplace_back("layer destroyed"); }

        void onAttach() override {
            g_AppData = &m_AppData;
            g_Events.emplace_back("layer attached");
            m_AppData.registerLayout(createSP<TrackedLayout>());
            m_AppData.addPanel(createSP<EmptyPanel>(m_AppData));
            m_AppData.addPopup(createSP<CountingPopup>(m_AppData));
        }

        void onImGui() override {
            m_AppData.getWindow().close();
        }
    };

    class SecondLayer: public TestLayer {
    public:
        explicit SecondLayer(AppData& appData): TestLayer(appData) {}

        void onAttach() override {
            g_Events.emplace_back("second layer attached");
            activeLayout = m_AppData.getActiveLayout();
            panelCount = m_AppData.getPanels().size();
            popupCount = m_AppData.getPopups().size();
        }

        static inline LayoutID activeLayout = NO_LAYOUT;
        static inline Size panelCount = 0;
        static inline Size popupCount = 0;
    };

}

/**
 * @brief These tests create a real window, they are skipped when there is no display
 */
class TestApplication: public ::testing::Test {
protected:
    void SetUp() override {
#if defined(__linux__)
        if (std::getenv("DISPLAY") == nullptr && std::getenv("WAYLAND_DISPLAY") == nullptr) {
            GTEST_SKIP() << "No display available";
        }
#endif
        g_Events.clear();
        g_AppData = nullptr;
    }

    UP<Application> createApplication() const {
        ApplicationDesc desc;
        desc.applicationName = "VelyraTestApplication";
        desc.settings.enableSave = false;
        desc.settings.settingsFile = fs::temp_directory_path() / "VelyraTestApplication" / "settings.json";
        return createUP<Application>(desc, m_ProgramArgs);
    }

    ProgramArgs m_ProgramArgs;
};

TEST_F(TestApplication, LayerDestroyedBeforeAppData) {
    {
        auto app = createApplication();
        app->createAppLayer<TestLayer>();
        app->run();
    }
    // The layer can own resources of the context, it must be destroyed before AppData, ImGui and the context
    const std::vector<std::string> expected = {"layer attached", "layer destroyed", "layout destroyed"};
    EXPECT_EQ(g_Events, expected);
}

TEST_F(TestApplication, ReplacingLayerRemovesItsRegistrations) {
    auto app = createApplication();
    app->createAppLayer<TestLayer>();
    ASSERT_NE(g_AppData->getActiveLayout(), NO_LAYOUT);

    app->createAppLayer<SecondLayer>();

    // The layouts, panels and popups of the first layer reference it, so they are removed before the next layer
    const std::vector<std::string> expected = {"layer attached", "layer destroyed", "layout destroyed", "second layer attached"};
    EXPECT_EQ(g_Events, expected);
    EXPECT_EQ(SecondLayer::activeLayout, NO_LAYOUT);
    EXPECT_EQ(SecondLayer::panelCount, 0);
    EXPECT_EQ(SecondLayer::popupCount, 0);
}

TEST_F(TestApplication, PanelsCanAddAndRemovePanelsWhileDrawn) {
    auto app = createApplication();
    app->createAppLayer<TestLayer>();
    g_AppData->addPanel(createSP<SpawningPanel>(*g_AppData));
    const Size panelsBefore = g_AppData->getPanels().size();

    app->run(); // One frame

    // The spawning panel removed itself and added 50 panels
    EXPECT_EQ(g_AppData->getPanels().size(), panelsBefore - 1 + 50);
}

TEST_F(TestApplication, OpenPopupOnlyNotifiesChangedPopups) {
    auto app = createApplication();
    app->createAppLayer<TestLayer>();
    const auto popupA = createSP<CountingPopup>(*g_AppData);
    const auto popupB = createSP<CountingPopup>(*g_AppData);
    g_AppData->addPopup(popupA);
    g_AppData->addPopup(popupB);

    g_AppData->openPopup(popupA->getID());
    EXPECT_EQ(popupA->openCount, 1);
    EXPECT_EQ(popupB->closeCount, 0); // Was never open, so it is not closed either

    g_AppData->openPopup(popupB->getID());
    EXPECT_EQ(popupA->closeCount, 1);
    EXPECT_EQ(popupB->openCount, 1);

    g_AppData->openPopup(popupB->getID());
    EXPECT_EQ(popupB->openCount, 1); // Already open

    g_AppData->closePopup(popupB->getID());
    g_AppData->closePopup(popupB->getID());
    EXPECT_EQ(popupB->closeCount, 1);
}
