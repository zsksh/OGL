#pragma once
#include<string>
#include<vector>
#include<fstream>
#include<sstream>
#include <stdexcept> // For std::runtime_error
#include<unordered_map>
#include<map>
#include<mesh/mesh.h>
#include<core/vector.h>
namespace CGL {
    inline void process_face_vertex(
        const std::string& face_vertex_str,
        const std::vector<vec3>& temp_positions,
        const std::vector<vec3>& temp_normals,
        const std::vector<vec2>& temp_tex_coords,
        std::vector<Vertex>& out_vertices,
        std::map<std::string, uint32_t>& vertex_map,
        std::vector<uint32_t>& out_indices)
    {
        // If this combination of pos/uv/normal is already in our final vertex list,
        // just grab its index and add it to our indices list.
        if (vertex_map.count(face_vertex_str)) {
            out_indices.push_back(vertex_map[face_vertex_str]);
            return;
        }

        // This is a new, unique vertex. We need to parse its indices.
        std::stringstream fv_ss(face_vertex_str);
        std::string token;

        // OBJ indices are 1-based, so we subtract 1.
        uint32_t p_idx = 0, t_idx = 0, n_idx = 0;

        // Parse position index
        std::getline(fv_ss, token, '/');
        p_idx = std::stoul(token) - 1;

        // Parse texture coordinate index (if it exists)
        if (std::getline(fv_ss, token, '/')) {
            if (!token.empty()) {
                t_idx = std::stoul(token) - 1;
            }
        }

        // Parse normal index (if it exists)
        // This handles both v/vt/vn and v//vn formats
        if (std::getline(fv_ss, token, '/')) {
            if (!token.empty()) {
                n_idx = std::stoul(token) - 1;
            }
        }

        // Create a new vertex using the parsed indices
        Vertex new_vertex;
        new_vertex.position = temp_positions[p_idx];

        if (!temp_tex_coords.empty() && t_idx < temp_tex_coords.size()) {
            new_vertex.tex_coords = temp_tex_coords[t_idx];
        }
        if (!temp_normals.empty() && n_idx < temp_normals.size()) {
            new_vertex.normal = temp_normals[n_idx];
        }

        // Add the new vertex to our list
        out_vertices.push_back(new_vertex);

        // Get the index of the newly added vertex
        uint32_t new_index = static_cast<uint32_t>(out_vertices.size() - 1);

        // Add the new index to our indices list
        out_indices.push_back(new_index);

        // Record this new vertex combo in our map for future lookups
        vertex_map[face_vertex_str] = new_index;
    }
    inline void load_obj_file(const std::string& filename, StaticMesh& mesh_data) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Error: Could not open file " + filename);
        }

        // Temporary storage for attributes read from the OBJ file
        std::vector<vec3> temp_positions;
        std::vector<vec3> temp_normals;
        std::vector<vec2> temp_tex_coords;

        // Map to track unique vertex combinations (pos/tex/norm) to avoid duplicating vertex data
        // The key is a string like "p_idx/t_idx/n_idx", the value is the index in the final vertex buffer.
        std::map<std::string, uint32_t> vertex_map;

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix;

            if (prefix == "v") { // Vertex position
                vec3 pos;
                ss >> pos.x >> pos.y >> pos.z;
                temp_positions.push_back(pos);
            }
            else if (prefix == "vt") { // Vertex texture coordinate
                vec2 tex;
                ss >> tex.x >> tex.y;
                temp_tex_coords.push_back(tex);
            }
            else if (prefix == "vn") { // Vertex normal
                vec3 norm;
                ss >> norm.x >> norm.y >> norm.z;
                temp_normals.push_back(norm);
            }
            else if (prefix == "f") { // Face
                std::vector<std::string> face_vertex_strings;
                std::string face_vertex_str;
                while (ss >> face_vertex_str) {
                    face_vertex_strings.push_back(face_vertex_str);
                }

                // Triangulate if the face is a polygon (e.g., a quad)
                // We create a triangle fan starting from the first vertex.
                for (size_t i = 1; i < face_vertex_strings.size() - 1; ++i) {
                    process_face_vertex(face_vertex_strings[0], temp_positions, temp_normals, temp_tex_coords, mesh_data.vertices, vertex_map, mesh_data.indices);
                    process_face_vertex(face_vertex_strings[i], temp_positions, temp_normals, temp_tex_coords, mesh_data.vertices, vertex_map, mesh_data.indices);
                    process_face_vertex(face_vertex_strings[i + 1], temp_positions, temp_normals, temp_tex_coords, mesh_data.vertices, vertex_map, mesh_data.indices);
                }
            }
        }
        file.close();
    }

}

