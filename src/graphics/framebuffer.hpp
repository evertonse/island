#pragma once
#include "utils/common.h"
#include "stb_image.h"
#include "graphics/shader.hpp"
#include "math/vec.hpp"
#include "math/mat.hpp"
#include <string>
#include <regex>

namespace cyx {
    struct ShadowMap {
        u32 
            width = 2048,
            height = 2048;
        u32 fbo,texture;
        Shader shader;
        void init(const vec3& light_position);
    };

}