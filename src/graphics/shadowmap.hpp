#pragma once
#include "utils/common.h"
#include "stb_image.h"
#include "graphics/shader.hpp"
#include "math/vec.hpp"
#include "math/mat.hpp"
#include <string>
#include <regex>

namespace cyx {
    // Not being used at the moment @incomplete
    // @todo make shadows :)
    struct ShadowMap {
        u32 
            width = 2048,
            height = 2048;
        u32 fbo,texture;
        Shader shader;
        void init();
        void begin();
        void bind_texture();
        void end(veci2 viewport);
        mat4 matrix(); 
    };
    
}