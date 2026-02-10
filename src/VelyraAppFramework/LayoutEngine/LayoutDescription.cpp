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

    PanelNode::PanelNode(const Panel &desc_):
    Node(NodeType::Panel),
    desc(desc_){

    }

}