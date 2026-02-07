#include <iostream>
#include <VelyraAppFramework/Application.hpp>

using namespace Velyra;

class ExampleLayer: public App::Layer {
public:
    explicit ExampleLayer(App::AppData& app_data): App::Layer(app_data) {}
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