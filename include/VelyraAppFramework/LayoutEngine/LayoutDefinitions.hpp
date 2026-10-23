#pragma once

#include <VelyraAppFramework/AppInclude.hpp>
#include <functional>

#define DONT_CARE (-1.0f)

namespace Velyra::App {

    /**
     * @brief Stable identifier of a node in the layout tree. It is derived from the path of names (or sibling indices
     *        for unnamed splits) from the root to the node, so the same node in a layout tree that is rebuilt every
     *        frame keeps the same NodeID.
     */
    using NodeID = U64;

    using DrawFunction = std::function<void()>;

    typedef enum ResizeDirection: int {
        None = 0,
        Top = 1 << 0,
        Bottom = 1 << 1,
        Left = 1 << 2,
        Right = 1 << 3
    } ResizeDirection;

    inline ResizeDirection operator|(const ResizeDirection a, const ResizeDirection b) {
        return static_cast<ResizeDirection>(
            static_cast<int>(a) | static_cast<int>(b)
        );
    }

    inline ResizeDirection& operator|=(ResizeDirection& a, const ResizeDirection b) {
        a = a | b;
        return a;
    }

    inline bool hasFlag(const ResizeDirection dir, const ResizeDirection flag) {
        return (dir & flag) != ResizeDirection::None;
    }
}
