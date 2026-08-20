#pragma once

#include <functional>

#define DONT_CARE (-1.0f)

namespace Velyra::Core {

    class Window;
    class Context;

}

namespace Velyra::App {

    using NodeID = Size;

    using DrawFunction = std::function<void(Core::Window&, Core::Context&)>;

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