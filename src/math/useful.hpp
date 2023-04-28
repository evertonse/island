#pragma once
#include "utils/common.h"

namespace cyx {
    template<typename T>
	constexpr f32 radians(T degrees) {
		return 
              static_cast<f32>(degrees) 
            * static_cast<f32>(0.01745329251994329576923690768489);
	}

    template <typename T>
    T clamp(T value, T lower, T upper) {
        if (value < lower) {
            return lower;
        } else if (value > upper) {
            return upper;
        } else {
            return value;
        }
    }

    template <typename T>
    T lerp(const T& a, const T& b, float t) {
        return a * (1.0f - t) + b * t;
    }

    template <typename T>
    T slerp(const T& a, const T& b, float t) {
        float dot = a.dot(b);
        dot = clamp(dot, -1.0f, 1.0f);
        float theta = std::acos(dot) * t;
        T relative = (b - a * dot).normalize();
        return a * std::cos(theta) + relative * std::sin(theta);
    }
}