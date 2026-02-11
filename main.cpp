#include <iostream>
#include <VelyraAppFramework/Application.hpp>

using namespace Velyra;

class ExampleLayer: public App::Layer {
public:
    explicit ExampleLayer(App::AppData& app_data): App::Layer(app_data) {}

    void onAttach(const UP<Core::Window> &, const UP<Core::Context> &) override {
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
    }

    void onImGui(const UP<Core::Window> &, const UP<Core::Context> &) override {
        m_AppData.layoutEngine.setActiveLayout("DEFAULT");

        m_AppData.layoutEngine.beginPanel("A");
        ImGui::Text("This is panel A");
        m_AppData.layoutEngine.endPanel("A");

        m_AppData.layoutEngine.beginPanel("B");
        ImGui::Text("This is panel B");
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
};

int main(const int argc, char* argv[]) {
    const App::ProgramArgs args(argv, argv + argc);

    try {
        App::ApplicationDesc desc;
        desc.windowSettings.title = "Application Demo";
        desc.imGuiSettings.useViewports = true;

        App::Application app(desc, args);
        app.push_layer<ExampleLayer>();
        app.run();

    } catch (const std::exception& e) {
        std::cerr << "Application failed to start: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}