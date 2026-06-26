#include <iostream>
#include <VelyraAppFramework/Application.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>
#include <VelyraAppFramework/Widgets/Panel.hpp>

#include "VelyraAppFramework/Layer.hpp"

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

class ExampleStore {
public:
    ExampleStore() = default;

    SP<ExamplePopup> examplePopup = createSP<ExamplePopup>();
    ExampleSetting exampleSetting;
};

class Layout1: public App::Layout {
public:
    explicit Layout1(ExampleStore& store): Layout(1),
                                           m_Store(store){}
    ~Layout1() override = default;

    void drawA(Core::Window& window, Core::Context& context) {
        ImGui::Text("This is panel A");
    }

    void drawB(Core::Window& window, Core::Context& context) {
        ImGui::Text("Button was clicked %u times", m_Store.exampleSetting.value);
        if (ImGui::Button("Increment!")) {
             m_Store.exampleSetting.value++;
        }
        if (ImGui::Button("Reset")) {
             m_Store.exampleSetting.value = 0;
        }
    }

    void drawC(Core::Window& window, Core::Context& context) {
        ImGui::Text("This is panel C");
    }

    void drawD(Core::Window& window, Core::Context& context) {
        ImGui::Text("This is panel D");
    }

    void drawE(Core::Window& window, Core::Context& context) {
        ImGui::Text("This is panel E");
    }

    void drawF(Core::Window& window, Core::Context& context) {
        ImGui::Text("This is panel F");
    }

    SP<App::Node> getLayout() override {
        using namespace Velyra::App;

        return createLayout(
            horizontalSplit(
                verticalSplit(
                    createPanel({.name = "A", .drawFunction = bindDraw(this, &Layout1::drawA), .sizeRatio = 0.2f}),
                    createPanel({.name = "B", .drawFunction = bindDraw(this, &Layout1::drawB), .sizeRatio = 0.4f}),
                    createPanel({.name = "C", .drawFunction = bindDraw(this, &Layout1::drawC), .sizeRatio = 0.4f})
                ),
                createPanel({.name = "D", .drawFunction = bindDraw(this, &Layout1::drawD), .sizeRatio = 0.6f}),
                verticalSplit(
                    createPanel({.name = "E", .drawFunction = bindDraw(this, &Layout1::drawE), .sizeRatio = 0.3f}),
                    createPanel({.name =  "F", .drawFunction = bindDraw(this, &Layout1::drawF), .sizeRatio = 0.7f})
                )
            )
        );
    }

private:
    ExampleStore& m_Store;
};

class ExampleLayer: public App::Layer {
public:
    explicit ExampleLayer(App::AppData& app_data): App::Layer(app_data),
    m_Store(),
    m_Layout1(m_Store){

    }

    void onAttach(Core::Window &window, Core::Context &context) override {
        // Settings
         m_Store.exampleSetting = m_AppData.settings.getSetting<ExampleSetting>("ExampleSetting");

        // Popups
        m_AppData.addPopup(m_Store.examplePopup);

        m_AppData.layoutEngine.registerLayout(m_Layout1);
        m_AppData.layoutEngine.setActiveLayout(m_Layout1.getID());
    }

    void onDetach(Core::Window &window, Core::Context &context) override {
        m_AppData.settings.setSetting("ExampleSetting", m_Store.exampleSetting);
    }

    void mainMenuBar(Core::Window &window, Core::Context &context) override{
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
    ExampleStore m_Store;
    Layout1 m_Layout1;
};

int main(const int argc, char* argv[]) {
    const App::ProgramArgs args(argv, argv + argc);

    try {
        App::ApplicationDesc desc;
        desc.applicationName = "VelyraAppDemo";

        App::Application app(desc, args);
        app.createAppLayer<ExampleLayer>();
        app.run();

    } catch (const std::exception& e) {
        std::cerr << "Application failed to start: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}