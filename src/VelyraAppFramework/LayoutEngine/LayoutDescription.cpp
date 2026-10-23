#include <Pch.hpp>

#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>

namespace Velyra::App {

    Node::Node(const NodeType type_):
    type(type_){

    }

    void defaultDrawFunction() {
        ImGui::Text("Default draw function. Please provide a custom draw function for this panel.");
    }

    PanelNode::PanelNode(const Panel &desc_):
    Node(NodeType::Panel),
    desc(desc_){

    }

}
