#include <VelyraAppFramework/Application.hpp>
#include <VelyraAppFramework/Widgets/Panel.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>
#include <VelyraCore/ImGui/ImGui.hpp>
#include <iostream>

#include <VelyraAppFramework/Layer.hpp>
#include <VelyraAppFramework/Layout.hpp>
#include <VelyraAppFramework/AppData.hpp>
#include <VelyraAppFramework/Settings.hpp>
#include <VelyraUtils/DevUtils/JsonSerializer.hpp>

using namespace Velyra;

static constexpr App::LayoutID LAYOUT_1_ID = 1;
static constexpr App::LayoutID LAYOUT_2_ID = 2;
static constexpr App::Widgets::PopupID POPUP_EXAMPLE_ID = 1;

class ExamplePopup: public App::Widgets::Popup {
public:
    ExamplePopup(App::AppData& app_data):
    App::Widgets::Popup(app_data, "Example Popup", POPUP_EXAMPLE_ID) {}

protected:
    void drawContent() override {
        ImGui::Text("This is an example popup!");
        if (ImGui::Button("Close")) {
            setOpen(false);
        }

        drawConfirmationButtons("Add", "Cancel", true);
    }
};

class ExamplePanel: public App::Widgets::Panel {
public:
    explicit ExamplePanel(App::AppData& app_data, const std::string& name):
    App::Widgets::Panel(app_data, name) {}

protected:
    void drawContent() override {
        ImGui::Text("This is an example panel!");
    }
};

struct ExampleSetting {
    unsigned int value = 0;

    VL_GENERATE_JSON_SERIALIZER(ExampleSetting, value)
};

class ExampleStore {
public:
    ExampleStore() = default;

    SP<ExamplePopup> examplePopup;
    ExampleSetting exampleSetting;
    bool showPanelF = true;
};

class Layout1: public App::Layout {
public:
    explicit Layout1(App::AppData& appData, ExampleStore& store): Layout(LAYOUT_1_ID),
    m_AppData(appData), m_Store(store) {}

    // Called every frame, so the layout can depend on the state of the app
    SP<App::Node> getLayout() override {
        using namespace Velyra::App;

        return createLayout(
            horizontalSplit(
                verticalSplit({.name = "Left", .sizeRatio = 0.2f},
                    createPanel({.name = "A", .drawFunction = bindDraw(this, &Layout1::drawA), .sizeRatio = 0.2f}),
                    createPanel({.name = "B", .drawFunction = bindDraw(this, &Layout1::drawB), .sizeRatio = 0.4f}),
                    createPanel({.name = "C", .drawFunction = bindDraw(this, &Layout1::drawC), .sizeRatio = 0.4f})
                ),
                createPanel({.name = "D", .drawFunction = bindDraw(this, &Layout1::drawD), .sizeRatio = 0.6f}),
                verticalSplit({.name = "Right"}, // Takes the remaining 0.2
                    createPanel({.name = "E", .drawFunction = bindDraw(this, &Layout1::drawE), .sizeRatio = 0.3f}),
                    m_Store.showPanelF ? createPanel({.name = "F", .drawFunction = [] { ImGui::Text("This is panel F"); }}) : nullptr
                )
            )
        );
    }

private:
    void drawA() {
        if (ImGui::Button("Open Popup")) {
            m_AppData.openPopup(m_Store.examplePopup->getID());
        }
        if (ImGui::Button("Add new Panel")) {
            const std::string panelName = "New Panel " + std::to_string(m_AppData.getPanels().size() + 1);
            m_AppData.addPanel(createSP<ExamplePanel>(m_AppData, panelName));
        }
        if (ImGui::Button("Switch to layout 2")) {
            m_AppData.setActiveLayout(LAYOUT_2_ID);
        }
    }

    void drawB() {
        ImGui::Text("Button was clicked %u times", m_Store.exampleSetting.value);
        if (ImGui::Button("Increment!")) {
            m_Store.exampleSetting.value++;
        }
        if (ImGui::Button("Reset")) {
            m_Store.exampleSetting.value = 0;
        }
    }

    void drawC() {
        ImGui::Text("This is panel C");
    }

    void drawD() {
        ImGui::Text("This is panel D");
        ImGui::Checkbox("Show panel F", &m_Store.showPanelF);
    }

    void drawE() {
        ImGui::Text("This is panel E");
    }

private:
    App::AppData& m_AppData;
    ExampleStore& m_Store;
};

class Layout2: public App::Layout {
public:
    explicit Layout2(App::AppData& appData): Layout(LAYOUT_2_ID), m_AppData(appData) {}

    SP<App::Node> getLayout() override {
        using namespace Velyra::App;

        return createLayout(
            verticalSplit(
                createPanel({.name = "Top", .drawFunction = [this] {
                    if (ImGui::Button("Switch to layout 1")) {
                        m_AppData.setActiveLayout(LAYOUT_1_ID);
                    }
                }, .sizeRatio = 0.3f}),
                createPanel({.name = "Bottom", .drawFunction = [] { ImGui::Text("This is layout 2"); }})
            )
        );
    }

private:
    App::AppData& m_AppData;
};

class ExampleLayer: public App::Layer {
public:
    explicit ExampleLayer(App::AppData& appData): App::Layer(appData),
    m_Layout1(createSP<Layout1>(appData, m_Store)),
    m_Layout2(createSP<Layout2>(appData)) {}

    void onAttach() override {
        // Settings
        m_Store.exampleSetting = m_AppData.getSettings().getSetting<ExampleSetting>("ExampleSetting");

        // Popups
        m_Store.examplePopup = createSP<ExamplePopup>(m_AppData);
        m_AppData.addPopup(m_Store.examplePopup);

        // Layouts
        m_AppData.registerLayout(m_Layout1);
        m_AppData.registerLayout(m_Layout2);
        m_AppData.setActiveLayout(m_Layout1->getID());
    }

    void onDetach() override {
        m_AppData.getSettings().setSetting("ExampleSetting", m_Store.exampleSetting);
    }

    void mainMenuBar() override {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("Open");
                ImGui::MenuItem("Load Additional Files");
                ImGui::Separator();
                ImGui::MenuItem("Settings");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Show panel F", nullptr, &m_Store.showPanelF);
                if (ImGui::MenuItem("Reset layout")) {
                    m_Layout1->resetResizes();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("About")) {
                ImGui::MenuItem("VelyraAppFramework v0.4", nullptr, false, false);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

private:
    // Declared before the layouts, since they keep a reference to it
    ExampleStore m_Store;
    SP<Layout1> m_Layout1;
    SP<Layout2> m_Layout2;
};

int main(const int argc, char* argv[]) {
    const App::ProgramArgs args(argv, argv + argc);

    try {
        App::ApplicationDesc desc;
        desc.applicationName = "VelyraAppDemo";
        desc.settings.settingsFile = App::getAppdataPath(desc.applicationName) / "settings_VelyraAppDemo.json";

        // test git sync
        desc.settings.gitSync.remoteUrl = "git@github-personal:artick788/AppStorage.git";

        App::Application app(desc, args);
        app.createAppLayer<ExampleLayer>();
        app.run();

    } catch (const std::exception& e) {
        std::cerr << "Application failed to start: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}