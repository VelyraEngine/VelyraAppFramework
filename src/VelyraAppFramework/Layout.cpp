#include <Pch.hpp>

#include <VelyraAppFramework/Layout.hpp>

namespace Velyra::App {

    Layout::Layout(const LayoutID id):
    m_ID(id) {
        VL_PRECONDITION(id != NO_LAYOUT, "Layout ID {} is reserved for no layout", NO_LAYOUT)
    }

    Layout::~Layout() = default;

    void Layout::resetResizes() {
        m_LayoutEngine.resetResizes();
    }

    void Layout::draw() {
        const SP<Node> root = getLayout();
        if (root != nullptr) {
            m_LayoutEngine.draw(*root);
        }
    }

}
