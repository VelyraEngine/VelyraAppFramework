#include <Pch.hpp>

#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>

namespace Velyra::App {

    inline NodeID generateID() {
        static NodeID currentID = 0;
        return ++currentID;
    }

    Node::Node(const NodeType type_):
    id(generateID()),
    type(type_){

    }

    void defaultDrawFunction(Core::Window &/*window*/, Core::Context &/*context*/) {
        ImGui::Text("Default draw function. Please provide a custom draw function for this panel.");
    }

    PanelNode::PanelNode(const Panel &desc_):
    Node(NodeType::Panel),
    desc(desc_){

    }

}