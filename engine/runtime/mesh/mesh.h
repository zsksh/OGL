#pragma once
#include<vector>
#include<core/vector.h>
namespace CGL {
    struct Vertex {
        vec3 position;
        vec3 normal;
        vec3 color;
        vec2 tex_coords;
        unsigned vertex_id = 0;// hack
    };

    struct StaticMesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };


}