#pragma once

#include <VelyraUtils/VelyraUtils.hpp>

#include <cmath>
#include <functional>

#define DONT_CARE (-1.0f)

namespace Velyra::Core {

    class Window;
    class Context;

}

namespace Velyra::App {

    using NodeID = Size;

    using DrawFunction = std::function<void(Core::Window&, Core::Context&)>;

    enum class SizeMode {
        Ratio,  // value is a fraction (0..1) of the space left after fixed-pixel siblings
        Pixels  // value is a fixed size in raw screen pixels
    };

    struct Sizing {
        SizeMode mode = SizeMode::Ratio;
        float value = DONT_CARE; // ratio fraction or pixel count; DONT_CARE = auto-distribute
        float minPx = DONT_CARE; // lower clamp in pixels (DONT_CARE = unconstrained)
        float maxPx = DONT_CARE; // upper clamp in pixels (DONT_CARE = unconstrained)
    };

    inline Sizing ratio(const float value, const float minPx = DONT_CARE, const float maxPx = DONT_CARE) {
        return Sizing{SizeMode::Ratio, value, minPx, maxPx};
    }

    inline Sizing pixels(const float value, const float minPx = DONT_CARE, const float maxPx = DONT_CARE) {
        return Sizing{SizeMode::Pixels, value, minPx, maxPx};
    }

    // True when the Sizing carries explicit information (as opposed to the default auto ratio).
    inline bool isSizingSet(const Sizing& sizing) {
        return sizing.mode == SizeMode::Pixels
            || sizing.value > DONT_CARE
            || sizing.minPx > DONT_CARE
            || sizing.maxPx > DONT_CARE;
    }

    // Resolves the effective sizing for a descriptor: an explicit Sizing wins, otherwise the
    // legacy sizeRatio field is used as a plain ratio.
    inline Sizing effectiveSizing(const Sizing& sizing, const float sizeRatio) {
        if (isSizingSet(sizing)) {
            return sizing;
        }
        return Sizing{SizeMode::Ratio, sizeRatio, DONT_CARE, DONT_CARE};
    }

    inline bool sizingEqual(const Sizing& a, const Sizing& b) {
        constexpr float epsilon = 1e-4f;
        return a.mode == b.mode
            && std::abs(a.value - b.value) < epsilon
            && std::abs(a.minPx - b.minPx) < epsilon
            && std::abs(a.maxPx - b.maxPx) < epsilon;
    }

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