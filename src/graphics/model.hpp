#pragma once
#include "graphics/texture.hpp"
#include "graphics/mesh.hpp"


namespace cyx {

// Simple Class to encapsulate a Texture and a Mesh
struct Model {
    TripleBufferMesh mesh;
    SimpleTexture texture;
    
    void load(const char* mesh_path, const char* texture_path);
    void draw();

    static void from(
        Model* self,
        TripleBufferMesh mesh,
        SimpleTexture texture
    );

    static void from(
        Model* self,
        const char* mesh_path,
        const char* texture_path
    );
};

}