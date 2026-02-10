#pragma once

#include <VelyraAppFramework/AppInclude.hpp>

#define DONT_CARE (-1.0f)

namespace Velyra::App {

    using NodeID = Size;

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

    inline bool PIXEL_EQUAL(const ImVec2& a, const ImVec2& b) {
        constexpr float epsilon = 1.0f; // 1 pixel tolerance
        return (std::abs(a.x - b.x) < epsilon) && (std::abs(a.y - b.y) < epsilon);
    }

    inline ImVec2 PIXEL_CEIL(const ImVec2& vec) {
        return {std::ceil(vec.x), std::ceil(vec.y)};
    }

}