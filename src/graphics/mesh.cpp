#include "mesh.hpp"
namespace cyx {
    auto TriangularMesh::alloc_mesh(i32 vertices_count,i32 normals_count, i32 uvs_count, i32 faces_count) {
        this-> vertices_count = vertices_count;
        this-> normals_count = normals_count;
        this-> uvs_count = uvs_count;
        this-> faces_count = faces_count;


        auto vertices_size = vertices_count * sizeof(vertices[0]);
        auto normals_size  = normals_count * sizeof(normals[0]);
        // Every 3 u32 we form a triangle
        auto uvs_size = uvs_count * sizeof(uvs[0]);
        auto faces_size = faces_count * sizeof(faces[0]);
        // Alloc the data we need
        data = malloc(vertices_size + normals_size + uvs_size + faces_size);

        // Set up pointers 
        vertices = (vec3*) ((byte*)data);
        normals  = (vec3*) ((byte*)data + vertices_size);
        uvs      = (vec2*) ((byte*)data + vertices_size + normals_size);
        faces    = (Face*) ((byte*)data + vertices_size + normals_size + uvs_size);
        allocated = true;
    }

    auto TriangularMesh::free_mesh() {
        free(data);
        allocated = false;
    }

    auto TriangularMesh::from_obj(const char* filename, bool normalize)  -> TriangularMesh* {
        TriangularMesh* mesh =  new TriangularMesh;
        f32 max_val = 0;
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
        }

        // Read the vertex and face data
        std::vector<vec3> vertices;
        std::vector<vec3> normals;
        std::vector<vec2> uvs;
        std::vector<Face> faces;
        std::string line;
        bool has_normals = false;
        bool has_uvs    = false;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }
            // String Stream read from line and "sends" (>>) data to various variable types
            std::istringstream ss(line);
            std::string type;
            ss >> type;
            if (type == "v") {
                vec3 v;
                ss >> v.x >> v.y >> v.z;
                for (auto& val: v) {
                    f32 abs_val = std::abs(val);
                    if ( abs_val > max_val) {
                        max_val = abs_val;
                    } 
                }
                vertices.push_back(v);
            } 
            else if (type == "vt"){
                vec2 vt;
                int nil;
                auto&& s = split_string(line, " ");
                ss >> vt.u >> vt.v;
                if (s.size() > 3) {
                    ss >> nil;
                }
                uvs.push_back(vt);
                has_uvs = true;
            }
            else if (type == "vn"){
                vec3 vn;
                ss >> vn.x >> vn.y >> vn.z;
                normals.push_back(vn);
                has_normals = true;
            }
            else if (type == "f") {
                Face face;
                u32 v0=-1, t0=-1, n0=-1, v1=-1, t1=-1, n1=-1, v2=-1, t2=-1, n2=-1;
                if (std::sscanf(line.c_str(), "f %u/%u/%u %u/%u/%u %u/%u/%u", &v0, &t0, &n0, &v1, &t1, &n1, &v2, &t2, &n2) == 9){
                    face.v[0] = --v0; face.v[1] = --v1; face.v[2] = --v2;
                    face.t[0] = --t0; face.t[1] = --t1; face.t[2] = --t2;
                    face.n[0] = --n0; face.n[1] = --n1; face.n[2] = --n2;
                }
                else if (std::sscanf(line.c_str(), "f %u/%u %u/%u %u/%u", &v0, &t0, &v1, &t1, &v2, &t2) == 6){
                    if(has_normals) {
                        face.v[0] = --v0; face.v[1] = --v1; face.v[2] = --v2;
                        face.n[0] = --t0; face.n[1] = --t1; face.n[2] = --t2;
                    }
                    else{
                        face.v[0] = --v0; face.v[1] = --v1; face.v[2] = --v2;
                        face.t[0] = --t0; face.t[1] = --t1; face.t[2] = --t2;
                    }
                }
                else if (std::sscanf(line.c_str(), "f %u//%u %u//%u %u//%u", &v0, &n0, &v1, &n1, &v2, &n2) == 6) {
                    face.v[0] = --v0; face.v[1] = --v1; face.v[2] = --v2;
                    face.n[0] = --n0; face.n[1] = --n1; face.n[2] = --n2;
                } 
                else if (std::sscanf(line.c_str(), "f %u %u %u", &v0, &v1, &v2) == 3) {
                    // Vertex Only:
                    face.v[0] = --v0; face.v[1] = --v1; face.v[2] = --v2;
                }


                faces.push_back(face);
            }
        }

        if (!has_normals && normals.size() == 0) {
            u32 normal_index = 0;
            for (auto& face : faces) {
                // We manufacture normals gere
                vec3 
                    &v0 = vertices[face.v[0]], 
                    &v1 = vertices[face.v[1]], 
                    &v2 = vertices[face.v[2]];
                vec3 normal = vec3::cross(v2 - v0,v1 - v0).normalize();
                normal_index++;
                face.n[0] = normal_index;
                face.n[1] = normal_index;
                face.n[2] = normal_index;
                normals.push_back(normal);
            }
        }
        if (normalize) {
            for (auto&& v: vertices) {
                v  /= max_val;
            }
        }

        std::cout 
            << "vertices.size =" << vertices.size() << '\n'
            << "normals.size = " << normals.size() << '\n'
            << "uvs.size = " << uvs.size() << '\n'
            << "faces.size = "<< faces.size();

        // Allocate memory for the mesh
        mesh->alloc_mesh(
            vertices.size(), 
            normals.size(), 
            uvs.size(), 
            faces.size()
        );

        // Copy the vertex and index data into the mesh
        memcpy(mesh->vertices, vertices.data(), vertices.size() * sizeof(mesh->vertices[0]));
        memcpy(mesh->normals, normals.data(), normals.size() * sizeof(mesh->normals[0]));
        memcpy(mesh->uvs, uvs.data(), uvs.size() * sizeof(mesh->normals[0]));
        memcpy(mesh->faces, faces.data(), faces.size() * sizeof(mesh->faces[0]));

        // Close the file and return the mesh
        file.close();
        return mesh;
    }


    static std::vector<std::string> split_string(const std::string& str, const std::string& delimiter) {
        std::vector<std::string> tokens;
        size_t prev_pos = 0, pos = 0;
        while ((pos = str.find(delimiter, pos)) != std::string::npos) {
            std::string token = str.substr(prev_pos, pos - prev_pos);
            tokens.push_back(token);
            prev_pos = pos + delimiter.length();
            pos = prev_pos;
        }
        tokens.push_back(str.substr(prev_pos, pos - prev_pos));
        return tokens;
    }
}	

