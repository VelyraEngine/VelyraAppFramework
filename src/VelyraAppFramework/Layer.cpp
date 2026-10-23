#include <VelyraAppFramework/Layer.hpp>

namespace Velyra::App {

    Layer::Layer(AppData& appData): m_AppData(appData) {}

    Layer::~Layer() = default;

    void Layer::onAttach() {}

    void Layer::onDetach() {}

    void Layer::onUpdate(Duration /*deltaTime*/) {}

    void Layer::mainMenuBar() {}

    void Layer::onImGui() {}

    void Layer::onEvent(const Core::Event& /*event*/) {}

}