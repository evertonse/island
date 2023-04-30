#pragma once
#include "utils/common.h"
#include <random>
#include <algorithm>

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
    T lerp(const T& a, const T& b, f32 t) {
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

    template <typename T>
    T ease_in_out(T t, T p) {
        return pow(t, p) / (pow(t, p) + pow(1 - t, p));
    }

    int random_int(int index) {
        persistent_data std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> dist(0, index);  // define the distribution

        return dist(gen);  // generate and return a random number between 0 and index (inclusive)
    }

    template<typename T>
    void shuffle(std::vector<T>& v){
        std::mt19937 gen(std::random_device{}());
        std::shuffle(v.begin(), v.end(), gen);
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
        os << "[";
        for (std::size_t i = 0; i < v.size(); ++i) {
            os << v[i];
            if (i != v.size() - 1) {
                os << ", ";
            }
        }
        os << "]";
        return os;
    }

    f32 sigmoid(f32 x, f32 t = 0.0f, f32 k = 1.0f) {
        return 1.0f / (1.0f + std::exp(k * x + t));
    }

        
}