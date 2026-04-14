#include <iostream>
#include <VelyraAppFramework/Application.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>
#include <VelyraAppFramework/Widgets/Panel.hpp>

using namespace Velyra;

class ExamplePopup: public App::Widgets::Popup {
public:
    explicit ExamplePopup(): App::Widgets::Popup("Example Popup") {}

protected:
    void drawContent() override {
        ImGui::Text("This is an example popup!");
        if (ImGui::Button("Close")) {
            setOpen(false);
        }
    }
};

class ExamplePanel: public App::Widgets::Panel {
public:
    explicit ExamplePanel(App::AppData& app_data, const std::string& name): App::Widgets::Panel(app_data, name) {}

protected:
    void drawContent() override {
        ImGui::Text("This is an example panel!");
    }
};

struct ExampleSetting {
    unsigned int value = 0;

    VL_GENERATE_JSON_SERIALIZER(ExampleSetting, value)
};

class ExampleLayer: public App::Layer {
public:
    explicit ExampleLayer(App::AppData& app_data): App::Layer(app_data) {}

    void onAttach(const UP<Core::Window> &, const UP<Core::Context> &) override {
        // Settings
        m_ExampleSetting = m_AppData.settings.getSetting<ExampleSetting>("ExampleSetting");

        // Popups
        m_AppData.addPopup(m_ExamplePopup);

        // Layout
        using namespace Velyra::App;


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

        m_AppData.layoutEngine.registerLayout("DEFAULT", layout);
        m_AppData.layoutEngine.setActiveLayout("DEFAULT");
    }

    void onDetach(const UP<Core::Window> &, const UP<Core::Context> &) override {
        m_AppData.settings.setSetting("ExampleSetting", m_ExampleSetting);
    }

    void onImGui(const UP<Core::Window> &, const UP<Core::Context> &) override {
        drawMainMenuBar();
        drawLayout();
    }

    void drawLayout() {
        m_AppData.layoutEngine.beginPanel("A");
        ImGui::Text("This is panel A");
        if (ImGui::Button("Open Popup")) {
            m_ExamplePopup->setOpen(true);
        }
        if (ImGui::Button("Add new Panel")) {
            const std::string panelName = "New Panel " + std::to_string(m_AppData.getPanelCount() + 1);
            m_AppData.addPanel(createSP<ExamplePanel>(m_AppData, panelName));
        }
        m_AppData.layoutEngine.endPanel("A");

        m_AppData.layoutEngine.beginPanel("B");
        ImGui::Text("Button was clicked %u times", m_ExampleSetting.value);
        if (ImGui::Button("Increment!")) {
            m_ExampleSetting.value++;
        }
        if (ImGui::Button("Reset")) {
            m_ExampleSetting.value = 0;
        }
        m_AppData.layoutEngine.endPanel("B");

        m_AppData.layoutEngine.beginPanel("C");
        ImGui::Text("This is panel C");
        m_AppData.layoutEngine.endPanel("C");

        m_AppData.layoutEngine.beginPanel("D");
        ImGui::Text("This is panel D");
        m_AppData.layoutEngine.endPanel("D");

        m_AppData.layoutEngine.beginPanel("E");
        ImGui::Text("This is panel E");
        m_AppData.layoutEngine.endPanel("E");

        m_AppData.layoutEngine.beginPanel("F");
        ImGui::Text("This is panel F");
        m_AppData.layoutEngine.endPanel("F");
    }

    void drawMainMenuBar() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("Open");
                ImGui::MenuItem("Load Additional Files");
                ImGui::Separator();
                ImGui::MenuItem("Settings");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools")) {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("About")) {
                ImGui::MenuItem("VelyraAppFramework v0.1", nullptr, false, false);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

private:
    SP<ExamplePopup> m_ExamplePopup = createSP<ExamplePopup>();
    ExampleSetting m_ExampleSetting;
};

int main(const int argc, char* argv[]) {
    const App::ProgramArgs args(argv, argv + argc);

    try {
        App::ApplicationDesc desc;
        desc.applicationName = "VelyraAppDemo";

        App::Application app(desc, args);
        app.pushLayer<ExampleLayer>();
        app.run();

    } catch (const std::exception& e) {
        std::cerr << "Application failed to start: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}