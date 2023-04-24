#pragma once
#include <vector>
#include <iostream>
#include <fstream>

#include "utils/common.h"
#include "math/vec.hpp"

namespace cyx {

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
		i32   vertices_count = 0;
		i32   faces_count = 0; 
		i32   uvs_count = 0;
		i32   normals_count = 0; 
        bool  allocated = false;

		auto alloc_mesh(i32 vertices_count,i32 normals_count, i32 uvs_count, i32 faces_count);
        auto free_mesh();
        static auto from_obj(const char* filename, bool normalize = false)  -> TriangularMesh*;
    };

    static std::vector<std::string> split_string(const std::string& str, const std::string& delimiter);
}	

