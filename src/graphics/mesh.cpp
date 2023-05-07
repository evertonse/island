#include "mesh.hpp"

namespace cyx {
    auto TriangularMesh::alloc_mesh(u32 vertices_count,u32 normals_count, u32 uvs_count, u32 faces_count) {
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

    auto TriangularMesh::load(const char* filename, bool normalize, bool auto_init)  -> TriangularMesh& {
        f32 max_val = 0;
        std::ifstream file(filename);
        
        assert(file.is_open() && "Failed to open file: ");

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
            // Here we need to check against various formats
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

        // Allocate memory for the this
        this->alloc_mesh(
            vertices.size(), 
            normals.size(), 
            uvs.size(), 
            faces.size()
        );

        // Copy the vertex and index data into the this
        memcpy(this->vertices, vertices.data(), vertices.size() * sizeof(this->vertices[0]));
        memcpy(this->normals, normals.data(), normals.size() * sizeof(this->normals[0]));
        memcpy(this->uvs, uvs.data(), uvs.size() * sizeof(this->normals[0]));
        memcpy(this->faces, faces.data(), faces.size() * sizeof(this->faces[0]));

        // Close the file and return the this
        file.close();
        if (auto_init){
            this->init();
        }
        return *this;
    }

    void TriangularMesh::init() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        // 3 buffers because  easier
        glGenBuffers(vbo_count, vbo);
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, vertices_count* sizeof(vertices[0]), (f32*)vertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, normals_count * sizeof(normals[0]), (f32*)normals, GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
            glBufferData(GL_ARRAY_BUFFER, uvs_count * sizeof(uvs[0]), (f32*)uvs, GL_STATIC_DRAW);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }
    }

    void TriangularMesh::draw() {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices_count);
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

//*********************** ---------------- ***********************//
//*********************** TripleBufferMesh ***********************//
//*********************** ---------------- ***********************//

    TripleBufferMesh::TripleBufferMesh() : vbo { 0 }
    {
        vao = nullbuffer;

        verts.clear();
        uvs.clear();
        normals.clear();
    }

    TripleBufferMesh::TripleBufferMesh(std::string fname)
    {
        load(fname);
    }

    TripleBufferMesh::~TripleBufferMesh()
    {
        glDeleteBuffers(vbo_count, vbo);
        glDeleteVertexArrays(1, &vao);
    }

    // This function is a mix of my Triangular Mesh and some other adaptation
    void TripleBufferMesh::load(std::string filepath)
    {
        std::ifstream fin(filepath);
        if (!fin) 
        {
            vao    = nullbuffer;
            vbo[0] = nullbuffer;
            vbo[1] = nullbuffer;
            vbo[2] = nullbuffer;
            std::cout << filepath << " [Mesh]: could not open file "<< filepath ;
            assert(0 && " Could not load mesh  because file could not be open");
            return;
        }

        // Temporaries vars
        //  we take a pallete to sample are vertices, uvs and textures
        std::vector<f32> vert_palette;
        std::vector<f32> uv_palette;
        bool is3Dtexture = false;

        std::string line;
        while (!fin.eof()) {
            std::getline(fin, line);
        
            if (line.find("v ") == 0) {
                std::stringstream ss(line.c_str() + 2);
                f32 x, y, z;
                ss >> x >> y >> z;
                vert_palette.push_back(x);
                vert_palette.push_back(y);
                vert_palette.push_back(z);
            }

            else if (line.find("vt ") == 0) {
                std::stringstream ss(line.c_str() + 3);
                f32 u, v, w;
                ss >> u >> v >> w;
                uv_palette.push_back(u);
                uv_palette.push_back(v);
                if (!ss.fail()) {
                    uv_palette.push_back(w);
                    is3Dtexture = true;
                }
            }

            else if (line.find("c ") == 0) {
                u32 a = 0, b = 0, c = 0;
                if (line[2] == '*') {
                    const u32 v_ix = (u32)vert_palette.size() / 3;
                    a = v_ix - 2; b = v_ix - 1; c = v_ix;
                }
                else {
                    std::stringstream ss(line.c_str() + 2);
                    ss >> a >> b >> c;
                }
                const vec3 v1(&vert_palette[(a - 1) * 3]);
                const vec3 v2(&vert_palette[(b - 1) * 3]);
                const vec3 v3(&vert_palette[(c - 1) * 3]);
            }

            else if (line.find("f ") == 0) {
                //Count the slashes
                int num_slashes = 0;
                size_t last_slash_ix = 0;
                bool doubleslash = false;
                for (size_t i = 0; i < line.size(); ++i) {
                    if (line[i] == '/') {
                        line[i] = ' ';
                        if (last_slash_ix == i - 1) {
                            assert(vert_palette.size() == uv_palette.size() || uv_palette.empty());
                            doubleslash = true;
                        }
                        last_slash_ix = i;
                        num_slashes++;
                    }
                }
                u32 a = 0, b = 0, c = 0, d = 0;
                u32 at = 0, bt = 0, ct = 0, dt = 0;
                u32 _tmp;
                std::stringstream ss(line.c_str() + 2);
                const bool wild = (line[2] == '*');
                const bool wild2 = (line[3] == '*');
                bool is_quad = false;

                //Interpret face based on slash
                if (wild) 
                {
                    assert(num_slashes == 0);
                    const u32 v_ix = (u32)vert_palette.size() / 3;
                    const u32 t_ix = (u32)uv_palette.size() / (is3Dtexture ? 3 : 2);
                    if (wild2) 
                    {
                        a = v_ix - 3; b = v_ix - 2; c = v_ix - 1; d = v_ix - 0;
                        at = t_ix - 3; bt = t_ix - 2; ct = t_ix - 1; dt = t_ix - 0;
                        is_quad = true;
                    }
                    else
                    {
                        a = v_ix - 2; b = v_ix - 1; c = v_ix;
                        at = t_ix - 2; bt = t_ix - 1; ct = t_ix;
                    }
                }
                else if (num_slashes == 0) 
                {
                    ss >> a >> b >> c >> d;
                    at = a; bt = b; ct = c; dt = d;
                    if (!ss.fail()) {
                        is_quad = true;
                    }
                }
                else if (num_slashes == 3) 
                {
                    ss >> a >> at >> b >> bt >> c >> ct;
                }
                else if (num_slashes == 4)
                {
                    is_quad = true;
                    ss >> a >> at >> b >> bt >> c >> ct >> d >> dt;
                }
                else if (num_slashes == 6) 
                {
                    if (doubleslash) {
                        ss >> a >> _tmp >> b >> _tmp >> c >> _tmp;
                        at = a; bt = b; ct = c;
                    }
                    else 
                    {
                        ss >> a >> at >> _tmp >> b >> bt >> _tmp >> c >> ct >> _tmp;
                    }
                }
                else if (num_slashes == 8) 
                {
                    is_quad = true;
                    if (doubleslash) 
                    {
                        ss >> a >> _tmp >> b >> _tmp >> c >> _tmp >> d >> _tmp;
                        at = a; bt = b; ct = c; dt = d;
                    }
                    else 
                    {
                        ss >> a >> at >> _tmp >> b >> bt >> _tmp >> c >> ct >> _tmp >> d >> dt >> _tmp;
                    }
                }
                else 
                {
                    assert(false);
                    continue;
                }

                //Add face to list
                if (is3Dtexture) {
                    std::cout << "Texture from " << filepath 
                            << " is 3D, You have no Support right now to hanlde that\n"
                            << std::cin.get();
                }
                add_face(vert_palette, uv_palette, a, at, b, bt, c, ct, is3Dtexture);
                if (is_quad)
                {
                    add_face(vert_palette, uv_palette, c, ct, d, dt, a, at, is3Dtexture);
                }
            }
        }

        init_gl();
        std::cout << "Mesh loaded from " << filepath
            << "verts.size():" << verts.size() << " \n"
            << "uvs.size():" << uvs.size() << " \n"
            << "normals.size():" << normals.size() << " \n"
            << "\n";
        return;
    }

    void TripleBufferMesh::clear()
    {
        glDeleteBuffers(vbo_count, vbo);
        glDeleteVertexArrays(1, &vao);
    }

    void TripleBufferMesh::draw() {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)verts.size());
    }

    void TripleBufferMesh::from(
        TripleBufferMesh *self,
        const f32 vertices[][3],
        const f32 normals[][3],
        const f32 uvs[][2],
        const u32* indexes,
        const u32 indexes_count,
        const f32 scale
    ){

        self->verts   = std::vector<f32>();
        self->normals = std::vector<f32>();
        self->uvs     = std::vector<f32>();

        for (size_t it = 0; it < indexes_count; it++) {
            u32 i = indexes[it];
            for (size_t j = 0; j < 3; j++) {
                self->verts.push_back(vertices[i][j]*scale);
                self->normals.push_back(normals[i][j]);
                if (j < 2) {
                    self->uvs.push_back(uvs[i][j]);
                }
            }
        }
        assert(self->verts.size() == self->normals.size());
        assert(self->uvs.size() == (2*self->normals.size())/3);
        self->init_gl();
        
    }

    void TripleBufferMesh::goblin(TripleBufferMesh *self) {
        from(
            self,
            goblin_objVerts,
            goblin_objNormals,
            goblin_objTexCoords, 
            goblin_objIndexes,
            goblin_objIndexesCount,
            0.82f
        );
    }

    void TripleBufferMesh::tiger(TripleBufferMesh *self) {
        from(
        self,
        tiger_objVerts,
        tiger_objNormals,
        tiger_objTexCoords, 
        tiger_objIndexes,
        tiger_objIndexesCount
        );
    }

    void TripleBufferMesh::horse(TripleBufferMesh *self) {
        from(
        self,
        horse_objVerts,
        horse_objNormals,
        horse_objTexCoords, 
        horse_objIndexes,
        horse_objIndexesCount
        );
    }

    void TripleBufferMesh::cube(TripleBufferMesh *self) {
        from(
        self,
        cube_objVerts,
        cube_objNormals,
        cube_objTexCoords, 
        cube_objIndexes,
        cube_objIndexesCount
        );
    }

    void TripleBufferMesh::surface(TripleBufferMesh *self) {
        from(
        self,
        surface_objVerts,
        surface_objNormals,
        surface_objTexCoords, 
        surface_objIndexes,
        surface_objIndexesCount
        );
    }

    void TripleBufferMesh::bamboo(TripleBufferMesh *self) {
        from(
        self,
        bamboo_objVerts,
        bamboo_objNormals,
        bamboo_objTexCoords, 
        bamboo_objIndexes,
        bamboo_objIndexesCount
        );
    }

    void TripleBufferMesh::plant1(TripleBufferMesh * self) {
        from(
            self,
            vase_plant_objVerts,
            vase_plant_objNormals,
            vase_plant_objTexCoords, 
            vase_plant_objIndexes,
            vase_plant_objIndexesCount
        );
    }

    void TripleBufferMesh::enemy(TripleBufferMesh * self) {
        from(
            self,
            enemy_objVerts,
            enemy_objNormals,
            enemy_objTexCoords, 
            enemy_objIndexes,
            enemy_objIndexesCount
        );
    }


    void TripleBufferMesh::terrain(
        TripleBufferMesh * self, 
        int width, int length, f32 spacing, f32 amplitude, f32 frequency
    ) {
        std::vector<f32> vertices;
        std::vector<f32> normals;
        std::vector<f32> uvs;
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < length; j++) {
                // calculate vertex position
                float x = j * spacing;
                float z = i * spacing;
                float y = amplitude * std::sin(x * frequency) * std::cos(z * frequency);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // calculate normal
                float dx = -amplitude * frequency * std::cos(x * frequency) * std::cos(z * frequency);
                float dy = amplitude * std::sin(x * frequency) * std::sin(z * frequency);
                float dz = -amplitude * frequency * std::sin(x * frequency) * std::cos(z * frequency);
                float length = std::sqrt(dx * dx + dy * dy + dz * dz);
                normals.push_back(dx / length);
                normals.push_back(dy / length);
                normals.push_back(dz / length);

                // calculate texture coordinate
                float u = j / (float)(length- 1);
                float v = i / (float)(width - 1);
                uvs.push_back(u);
                uvs.push_back(v);
            }
        }
    #if 0
        // Generate vertices, texture coordinates, and normals
        for (int z = 0; z < length; z++) {
            for (int x = 0; x < width; x++) {
                // Calculate vertex position
                f32 xPos = (x - (width / 2.0f)) * spacing;
                f32 zPos = (z - (length / 2.0f)) * spacing;
                f32 yPos = amplitude * (sinf(xPos * frequency) + cosf(zPos * frequency));

                vertices.push_back(xPos);
                vertices.push_back(yPos);
                vertices.push_back(zPos);

                // Calculate texture coordinates
                f32 u = (f32)x / (f32)width;
                f32 v = (f32)z / (f32)length;

                uvs.push_back(u);
                uvs.push_back(v);

                f32 heightL = amplitude * sin(((x - 1) - (width / 2.0f) + z * width - (width / 2.0f)) * spacing * frequency);
                f32 heightR = amplitude * sin(((x + 1) - (width / 2.0f) + z * width - (width / 2.0f)) * spacing * frequency);
                f32 heightD = amplitude * sin((x - (width / 2.0f) + (z - 1) * width - (width / 2.0f)) * spacing * frequency);
                f32 heightU = amplitude * sin((x - (width / 2.0f) + (z + 1) * width - (width / 2.0f)) * spacing * frequency);
                vec3 normal = vec3(heightL - heightR, 2.0f, heightD - heightU).normalize();

                // Calculate normal
                //f32 dx = amplitude * frequency * cosf(xPos * frequency);
                //f32 dy = 1.0f;
                //f32 dz = amplitude * frequency * sinf(zPos * frequency);
                //vec3 normal = vec3(dx, dy, dz).normalize();

                // Add normal to list
                normals.push_back(normal.x);
                normals.push_back(normal.y);
                normals.push_back(normal.z);
            }
        }
        #endif
        self->verts = vertices;
        self->normals = normals;
        self->uvs= uvs;
        self->init_gl();
    }


    void TripleBufferMesh::init_gl() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        // 3 buffers because  easier
        glGenBuffers(vbo_count, vbo);
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(verts[0]), verts.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), normals.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
            glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(uvs[0]), uvs.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }
    }


    void TripleBufferMesh::add_face(const std::vector<f32>& vert_palette, const std::vector<f32>& uv_palette,
        u32 a, u32 at, u32 b, u32 bt, u32 c, u32 ct, bool is3Dtexture)
    {
        //Merge texture and vertex indicies
        assert(a > 0 && b > 0 && c > 0);
        assert(at > 0 && bt > 0 && ct > 0);
        a -= 1; b -= 1; c -= 1;
        at -= 1; bt -= 1; ct -= 1;
        const u32 v_ix[3] = { a, b, c };
        const u32 uv_ix[3] = { at, bt, ct };

        //Calcuate the normal for this face
        vec3 v1(&vert_palette[a * 3]);
        vec3 v2(&vert_palette[b * 3]);
        vec3 v3(&vert_palette[c * 3]);
        vec3 dv1 = v2 - v1;
        vec3 dv2 = v3 - v1;
        vec3 normal = vec3::cross(dv1, dv2).normalize();

        for (int i = 0; i < 3; ++i) 
        {
            const u32 v = v_ix[i];
            const u32 vt = uv_ix[i];
            assert(v < vert_palette.size() / 3);
            verts.push_back(vert_palette[v * 3]);
            verts.push_back(vert_palette[v * 3 + 1]);
            verts.push_back(vert_palette[v * 3 + 2]);
            if (!uv_palette.empty()) 
            {
                if (is3Dtexture) 
                {
                    assert(vt < uv_palette.size() / 3);
                    uvs.push_back(uv_palette[vt * 3]);
                    uvs.push_back(uv_palette[vt * 3 + 1]);
                    uvs.push_back(uv_palette[vt * 3 + 2]);
                }
                else 
                {
                    assert(vt < uv_palette.size() / 2);
                    uvs.push_back(uv_palette[vt * 2]);
                    uvs.push_back(uv_palette[vt * 2 + 1]);
                }
            }
            else 
            {
                uvs.push_back(0.0f);
                uvs.push_back(0.0f);
            }
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }
    }
}	

