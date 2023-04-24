#pragma once
#include "utils/common.h"

namespace cyx {
    template<typename T>
	constexpr f32 radians(T degrees) {
		return 
              static_cast<f32>(degrees) 
            * static_cast<f32>(0.01745329251994329576923690768489);
	}
}