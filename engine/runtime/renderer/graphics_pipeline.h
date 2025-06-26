#pragma once
#include<memory>
#include<functional>
#include<renderer/rhi_struct.h>
#include<core/matrix.h>
#include<renderer/material.h>
#include<core/math.h>
namespace CGL {
	struct RenderContext {
		std::shared_ptr<Scene> scene;
		std::shared_ptr<ShadowMap> shadow_map;
		std::shared_ptr<Texture2D> lit_scene_tex;
		std::shared_ptr<std::vector<uint32_t>> final_color_buffer;
	};
	class VertexShader {
	public:
		virtual void shade(VertexData& vertex, const ObjectUniforms& ou, const SceneUniforms& su) = 0;
		virtual ~VertexShader() = default;
	};

	class FragmentShader {
	public:
		virtual Color shade(const FragmentData& frag, const Material& material,
			const SceneUniforms& scene) = 0;
		virtual ~FragmentShader() = default;
	};

	class PhongVertexShader : public VertexShader {
	public:
		void shade(VertexData& vertex, const ObjectUniforms& ou, const SceneUniforms& su) override {
			vertex.m_pos = vec3(ou.model * vec4(vertex.m_pos, 1.0f));
			//vertex.m_nor = normalize(vec3(object_uniforms.model.inverse().transpose() * vec4(vertex.m_nor, 0.0f)));
			vertex.m_nor = normalize(vec3(transpose(inverse(ou.model)) * vec4(vertex.m_nor, 0.0f)));
			vertex.m_cpos = su.projection * su.view * vec4(vertex.m_pos, 1.0f);
		}
	};


	class PhongFragmentShader : public FragmentShader {
	public:
		Color shade(const FragmentData& frag, const Material& material,
			const SceneUniforms& scene) override {
			auto& tex = frag.m_tex;
			vec3 final_color = vec3(0.0f, 0.0f, 0.0f);
			vec3 ambient_color, diffuse_color, specular_color, glowColor;
			if (material.m_diffuse_texture) {
				Color tex_color = material.m_diffuse_texture->Sample2D(tex[0], tex[1]);
				vec3 base_color = vec3(tex_color.r, tex_color.g, tex_color.b) / 255.0f;
				ambient_color = diffuse_color = base_color;
				specular_color = vec3(0, 0, 0);
			}
			else {
				ambient_color = material.ambient_color;
				diffuse_color = material.diffuse_color;
				return Color(diffuse_color.x * 255.0f, diffuse_color.y * 255.0f, diffuse_color.z * 255.0f, 255.0f);
			}
			vec3 normal = (frag.m_nor);
			vec3 view_dir = normalize(scene.view_pos - frag.m_pos);
			for (const auto& light : scene.lights) {
				vec3 light_dir = light->direction(frag.m_pos);
				vec3 ambient, diffuse, specular;
				float attenuation = 1.0f;
				ambient = light->intensity() * ambient_color;
				float diff_cof = std::max(dot(normal, light_dir), 0.0f);
				diffuse = light->intensity() * diffuse_color * diff_cof;
				vec3 halfwayDir = normalize(view_dir + light_dir);
				float spec = pow(std::max(dot(halfwayDir, normal), 0.0f), material.shininess);
				specular = light->intensity() * spec * specular_color;
				attenuation = light->attenuation(frag.m_pos);

				float cutoff = light->cutoff(light_dir);
				final_color += vec3((ambient + diffuse + specular) * attenuation * cutoff);

			}
			final_color[0] = std::min(final_color[0], 1.0f);
			final_color[1] = std::min(final_color[1], 1.0f);
			final_color[2] = std::min(final_color[2], 1.0f);
			return Color(final_color.x * 255.0f, final_color.y * 255.0f, final_color.z * 255.0f, 255.0f);
		}
	};
	class ShadowVertexShader : public VertexShader {
	public:
		void shade(VertexData& vertex, const ObjectUniforms& ou, const SceneUniforms& su) override {
			vertex.m_pos = vec3(ou.model * vec4(vertex.m_pos, 1.0f));
			//vertex.m_nor = normalize(vec3(object_uniforms.model.inverse().transpose() * vec4(vertex.m_nor, 0.0f)));
			vertex.m_nor = normalize(transpose(inverse(ou.model)) * vec4(vertex.m_nor, 0.0f));
			vertex.m_cpos = su.light_space_matrix * vec4(vertex.m_pos, 1.0f);
		}
	};
	class ShadowFragmentShader : public FragmentShader {
	public:
		Color shade(const FragmentData& frag, const Material& mat, const SceneUniforms& scene)  override {
			return { 255, 255, 255, 255 }; // Return white (or any dummy color).
		}
	};

	class PCFVertexShader :public VertexShader {
	public:
		void shade(VertexData& vertex, const ObjectUniforms& ou, const SceneUniforms& su) override {
			vertex.m_pos = vec3(ou.model * vec4(vertex.m_pos, 1.0f));
			vertex.m_nor = normalize(transpose(inverse(ou.model)) * vec4(vertex.m_nor, 0.0f));
			vertex.m_cpos = su.projection * su.view * vec4(vertex.m_pos, 1.0f);
		}
	};

	class PCFFragmentShader : public FragmentShader {
	public:
		Color shade(const FragmentData& frag, const Material& material, const SceneUniforms& scene) override {
			//float shadow = calculate_PCF_shadow(frag, scene);

			float shadow_factor = 1-calculate_PCF_shadow(frag, scene);
			Color final_color_vec = base_color(frag, material, scene);
			//return final_color_vec;
			return Color(final_color_vec.r * shadow_factor,
				final_color_vec.g * shadow_factor, final_color_vec.b * shadow_factor, 255);
		}
	private:
		float calculate_PCF_shadow(const FragmentData& frag, const SceneUniforms& scene) {
			vec4 frag_pos_light_space = scene.light_space_matrix * vec4(frag.m_pos, 1.0f);
			// Perform perspective divide to get Normalized Device Coordinates (NDC) [-1, 1].
			vec3 frag_ndc = vec3(frag_pos_light_space) / frag_pos_light_space.w;
			// Convert NDC to texture coordinates [0, 1] to sample the shadow map.
			vec2 shadow_map_uv;
			shadow_map_uv.x = frag_ndc.x * 0.5f + 0.5f;
			shadow_map_uv.y = frag_ndc.y * 0.5f + 0.5f;
			// The fragment's depth from the light's perspective.
			float current_depth = frag_ndc.z * 0.5f + 0.5f;
			// Check if the coordinate is outside the shadow map's range [0, 1]
			if (shadow_map_uv.x < 0.0 || shadow_map_uv.x > 1.0 || shadow_map_uv.y < 0.0 || shadow_map_uv.y > 1.0) {
				return 1.0; // Not in shadow if outside the map
			}
			float shadow_bias = 0.005f;
			int pcf_radius = 2;
			float texel_size_x = 1.0f / scene.shadow_map->getWidth();
			float texel_size_y = 1.0f / scene.shadow_map->getHeight();
			float shadow = 0.0f;
			for (int y = -pcf_radius; y <= pcf_radius; ++y) {
				for (int x = -pcf_radius; x <= pcf_radius; ++x) {
					vec2 sample_uv = { shadow_map_uv.x + x * texel_size_x, (shadow_map_uv.y + y * texel_size_y) };
					float depth_from_map = scene.shadow_map->getSample(sample_uv[0], sample_uv[1]);
					depth_from_map = depth_from_map * 0.5f + 0.5f;
					if (current_depth - shadow_bias > depth_from_map) {
						shadow += 1.0f;
					}
				}
			}
			int sample_count = (2 * pcf_radius + 1) * (2 * pcf_radius + 1);
			shadow /= sample_count; // Average the results
			return shadow;
		}
		Color base_color(const FragmentData& frag, const Material& material,
			const SceneUniforms& scene) {
			auto& tex = frag.m_tex;
			vec3 final_color = vec3(0.0f, 0.0f, 0.0f);
			vec3 ambient_color, diffuse_color, specular_color, glowColor;
			if (material.m_diffuse_texture) {
				Color tex_color = material.m_diffuse_texture->Sample2D(tex[0], tex[1]);
				vec3 base_color = vec3(tex_color.r, tex_color.g, tex_color.b) / 255.0f;
				ambient_color = diffuse_color = base_color;
				specular_color = vec3(0, 0, 0);
			}
			else {
				ambient_color = material.ambient_color;
				diffuse_color = material.diffuse_color;
				return Color(diffuse_color.x * 255.0f, diffuse_color.y * 255.0f, diffuse_color.z * 255.0f, 255.0f);
			}
			vec3 normal = (frag.m_nor);
			vec3 view_dir = normalize(scene.view_pos - frag.m_pos);
			for (const auto& light : scene.lights) {
				vec3 light_dir = light->direction(frag.m_pos);
				vec3 ambient, diffuse, specular;
				float attenuation = 1.0f;
				ambient = light->intensity() * ambient_color;
				float diff_cof = std::max(dot(normal, light_dir), 0.0f);
				diffuse = light->intensity() * diffuse_color * diff_cof;
				vec3 halfwayDir = normalize(view_dir + light_dir);
				float spec = pow(std::max(dot(halfwayDir, normal), 0.0f), material.shininess);
				specular = light->intensity() * spec * specular_color;
				attenuation = light->attenuation(frag.m_pos);

				float cutoff = light->cutoff(light_dir);
				final_color += vec3((ambient + diffuse + specular) * attenuation * cutoff);

			}
			final_color[0] = std::min(final_color[0], 1.0f);
			final_color[1] = std::min(final_color[1], 1.0f);
			final_color[2] = std::min(final_color[2], 1.0f);
			return Color(final_color.x * 255.0f, final_color.y * 255.0f, final_color.z * 255.0f, 255.0f);
		}
	};
	class ExtractBrightsFragmentShader : public FragmentShader {
	public:
		Color shade(const FragmentData& frag,
			const Material& material,
			const SceneUniforms& su) override
		{
			//const auto& uniforms = dynamic_cast<const ExtractBrightsUniforms&>(su);
			//assert(&uniforms);

			// Sample the original scene color
			Color scene_color = su.scene_texture->Sample2D(frag.m_tex.x, frag.m_tex.y);
			vec3 color = vec3(scene_color.r, scene_color.g, scene_color.b) / 255.0f;

			// Calculate the brightness (luminance) of the pixel
			float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

			// If the brightness is below the threshold, output black. Otherwise, output the color.
			
			float threshold = 0.3;
			if (brightness > threshold) {
				return scene_color;
			}
			else {
				return Color(0, 0, 0, 255);
			}
		}
	};
	class GaussianBlurFragmentShader : public FragmentShader {
	public:
		Color shade(const FragmentData& frag,
			const Material& material,
			const SceneUniforms& su) override
		{
			//const auto& uniforms = dynamic_cast<const BlurUniforms&>(su);
			//assert(&uniforms);

			// A 9-tap Gaussian kernel. The weights add up to ~1.0.
			const float weights[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };

			// Get the size of one pixel in UV coordinates
			vec2 texel_size = { 1.0f / su.scene_texture->m_width, 1.0f / su.scene_texture->m_height };

			vec3 result_color = vec3(0.0f);
			vec2 initial_uv = frag.m_tex;

			// Sample the center pixel
			Color center_color = su.scene_texture->Sample2D(initial_uv.x, initial_uv.y);
			result_color += vec3(center_color.r, center_color.g, center_color.b) * weights[0];

			// Sample neighboring pixels along one axis
			for (int i = 1; i < 5; ++i) {
				vec2 offset = true ? vec2(texel_size.x * i, 0.0f) : vec2(0.0f, texel_size.y * i);

				// Sample positive offset
				Color color1 = su.scene_texture->Sample2D(initial_uv.x + offset.x, initial_uv.y + offset.y);
				result_color += vec3(color1.r, color1.g, color1.b) * weights[i];

				// Sample negative offset
				Color color2 = su.scene_texture->Sample2D(initial_uv.x - offset.x, initial_uv.y - offset.y);
				result_color += vec3(color2.r, color2.g, color2.b) * weights[i];
			}

			// Convert back to 8-bit color
			//result_color /= 255.0f; // Adjust if your Color struct stores differently
			result_color.x = std::min(result_color.x, 255.0f);
			result_color.y = std::min(result_color.y, 255.0f);
			result_color.z = std::min(result_color.z, 255.0f);

			return Color(result_color.x, result_color.y, result_color.z, 255);
		}
	};


	class PostProcessVertexShader :public VertexShader {
	public:
		// This shader is very simple. It ignores all uniforms and just
		// passes through hard-coded vertices to form a giant triangle
		// that covers the entire screen in Normalized Device Coordinates.
		void shade(VertexData& vertex, const ObjectUniforms& ou, const SceneUniforms& su) override {
			const vec3 positions[3] = {
						vec3(-1.0f, -1.0f, 0.0f),
						vec3(3.0f, -1.0f, 0.0f),
						vec3(-1.0f,  3.0f, 0.0f)
			};

			// Use the ID to get the correct position.
			// It completely ignores the vertex.position from the (empty) VBO.
			vec3 final_pos = positions[vertex.vertex_id];

			// Output final clip-space position
			vertex.m_cpos = vec4(final_pos, 1.0);

			// Calculate and output the correct UV coordinates
			vertex.m_tex.x = (final_pos.x + 1.0f) * 0.5f;
			vertex.m_tex.y = 1.0-(final_pos.y + 1.0f) * 0.5f;
		}
	};

	class GrayscaleFragmentShader : public FragmentShader {
		Color shade(const FragmentData& frag, const Material& material, const SceneUniforms& scene) override {
			// Sample the color from the texture rendered by the previous pass
			Color original_color = scene.scene_texture->Sample2D(frag.m_tex.x, frag.m_tex.y);
			//return original_color;
			// Calculate the grayscale value (a simple average)
			unsigned char gray = (original_color.r + original_color.g + original_color.b) / 3;

			// Return the new color
			return Color(gray, gray, gray, 255);

		}

	};




	
	class GraphicsPipeline {
	public:
		GraphicsPipeline(std::shared_ptr<VertexShader> vs,
			std::shared_ptr<FragmentShader> fs)
			: m_vertex_shader(vs), m_fragment_shader(fs) {}
		std::shared_ptr<VertexShader> get_vertex_shader() const { return m_vertex_shader; }
		std::shared_ptr<FragmentShader> get_fragment_shader() const { return m_fragment_shader; }

	private:
		// Immutable state baked at creation
		std::shared_ptr<VertexShader> m_vertex_shader;
		std::shared_ptr<FragmentShader> m_fragment_shader;
		CullMode m_cull_mode;
	};

}
