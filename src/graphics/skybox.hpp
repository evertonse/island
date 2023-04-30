#pragma once
#include <string>
#include <regex>
#include "stb_image.h"

#include "utils/common.h"
#include "graphics/shader.hpp"
#include "math/vec.hpp"
#include "math/mat.hpp"

//#define CYX_DEFAULT_SKYBOX "assets/textures/skybox/cloud/"
#define CYX_DEFAULT_SKYBOX "assets/textures/skybox/sky/"

namespace cyx {
    /*
    Resource : https://www.khronos.org/opengl/wiki/Cubemap_Texture

    This class is a skybox, a simple cube at the far edge the scene
    If you look at the 'vertices' for example, all of them are 1.0f which is the 
    "edge" of the opengl clip space.

    Then "we're inside" it, camera position is always at the center of this cube
    we make it that way.

    Also each face has a texture, so 6 2D texture at total, the texture have to be
    very intricatelly aligned to create that feeling of continuity

    And the final thing is that instead of a uv, we sample the textures differently
    we use a direction vector from the camera position, so you can sample all six sides 
    of the cube by knowing where the camera is pointing at.

    If we follow the direction the camera is position we will know where intersects the cube
    and if we know that, then we know which color we need to paint in that specific location;

    Thats pretty much it =P
    */
struct Skybox {

    Shader shader;
    
    u32 vao, vbo, ib;
    u32 texture;

    static const f32 vertices[];
    static const u32 indices[];

    void init(
        const char* directory_path = CYX_DEFAULT_SKYBOX , const char* extension = ".png");
    void draw(const mat4& view, mat4 projection);

};
}
