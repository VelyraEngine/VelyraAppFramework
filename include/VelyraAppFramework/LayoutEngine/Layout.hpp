#pragma once

#include <VelyraUtils/VelyraUtils.hpp>

namespace Velyra::App {

    class Node;

    using LayoutID = Size;

    class VL_API Layout {
    public:
        explicit Layout(const LayoutID id) : m_ID(id) {}

        virtual ~Layout() = default;

        LayoutID getID() const { return m_ID; }

        virtual SP<Node> getLayout() = 0;

    private:
        const LayoutID m_ID;

    };

}
