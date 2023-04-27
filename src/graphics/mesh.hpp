#pragma once
#include <vector>
#include <iostream>
#include <fstream>

#include "utils/common.h"
#include "math/vec.hpp"
#include "graphics/texture.hpp"
#include "assets/all_obj.h"

#define nullbuffer 0;

namespace cyx {

    struct Vertex {
        f32 position[3];
        f32 normal[3];
        f32 uv[2];
    };


    struct Face {
        veci3 v;
        veci3 n;
        veci3 t;
    }; 


	struct TriangularMesh {
		void* data;
		vec3* vertices;
		vec3* normals;
		vec2* uvs;
		Face* faces;
        // Every place we have a count we should allow it to be negative 
		u32   vertices_count = 0;
		u32   faces_count = 0; 
		u32   uvs_count = 0;
		u32   normals_count = 0; 
        bool  allocated = false;

		auto alloc_mesh(u32 vertices_count,u32 normals_count, u32 uvs_count, u32 faces_count);
        auto free_mesh();

        static constexpr int vbo_count = 3;
        GLuint vao = 0;
        GLuint vbo[vbo_count];

        auto load(const char* filename, bool normalize = false, bool auto_init = true)  -> TriangularMesh&;
        auto init() -> void;
        auto draw() -> void;
    };

    static std::vector<std::string> split_string(const std::string& str, const std::string& delimiter);

 
    class TripleBufferMesh 
    {
    public:
        static constexpr int vbo_count = 3;

        TripleBufferMesh();
        TripleBufferMesh(std::string fname);
        ~TripleBufferMesh();

        void load(std::string fname);
        void clear();
        void draw();

        static void from(
            TripleBufferMesh *self,
            const f32 vertices [][3],
            const f32 normals  [][3],
            const f32 uvs      [][2],
            const u32* indexes,
            const u32 indexes_count
        );

        static void goblin(TripleBufferMesh*self);
        static void tiger(TripleBufferMesh * self);
        static void horse(TripleBufferMesh*self);

        static void cube(TripleBufferMesh *self);
        static void bamboo(TripleBufferMesh *self);

    private:

        GLuint vao;
        GLuint vbo[vbo_count];

        std::vector<f32> verts;
        std::vector<f32> uvs;
        std::vector<f32> normals;

        void add_face(const std::vector<f32>& vert_palette, const std::vector<f32>& uv_palette,
            u32 a, u32 at, u32 b, u32 bt, u32 c, u32 ct, bool is3DTex);

        void init_gl();
    };
}	

