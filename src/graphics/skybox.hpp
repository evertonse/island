#pragma once
#include "utils/common.h"
#include "stb_image.h"
#include "graphics/shader.hpp"
#include "math/vec.hpp"
#include "math/mat.hpp"
#include <string>
#include <regex>

//#define CYX_DEFAULT_SKYBOX "assets/textures/skybox/cloud/"
#define CYX_DEFAULT_SKYBOX "assets/textures/skybox/sky/"

namespace cyx {
    /*
    Resource : https://www.khronos.org/opengl/wiki/Cubemap_Texture

    This class is a skybox, a simple cube at the edge of every geometry
    look the 'vertices' for example, all of them are 1.0f which is the 
    "edge" of the opengl clip space.
    Then we're inside it, comera position is alway at the center of this cube
    we make it that way.
    Also each face has a texture, so 6 2D texture at total, the texture have to be
    very intricatelly aligned to create that feeling
    And the final thing is that instead of a uv, we sample the textures differently
    we use a direction vetor from the camera position, so you can sample all six sides 
    of the cube by knowing there the camera is pointing at.
    If we follow the direction the camera is pointion we will know where intersects the cube
    and if we know that we know which color we need to paint there; Thats pretty much it
    */
struct Skybox {

    Shader shader;
    
	u32 vao, vbo, ib;
    u32 texture;
    static constexpr f32 vertices[] {
        -1.0f, -1.0f,  1.0f,//        7--------6
         1.0f, -1.0f,  1.0f,//       /|       /|
         1.0f, -1.0f, -1.0f,//      4--------5 |
        -1.0f, -1.0f, -1.0f,//      | |      | |
        -1.0f,  1.0f,  1.0f,//      | 3------|-2
         1.0f,  1.0f,  1.0f,//      |/       |/
         1.0f,  1.0f, -1.0f,//      0--------1
        -1.0f,  1.0f, -1.0f
    };


    static constexpr u32 indices[] {
        // Right
        1, 2, 6,
        6, 5, 1,
        // Left
        0, 4, 7,
        7, 3, 0,
        // Top
        4, 5, 6,
        6, 7, 4,
        // Bottom
        0, 3, 2,
        2, 1, 0,
        // Back
        0, 1, 5,
        5, 4, 0,
        // Front
        3, 7, 6,
        6, 2, 3
    };
    void init(
        const char* directory_path = CYX_DEFAULT_SKYBOX , const char* extension = ".png");
    void draw(const mat4& view, mat4 projection);

};
}