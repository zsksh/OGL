#pragma once 
#include<memory>
#include<vector>
#include<core/vector.h>
#include<core/matrix.h>
#include<renderer/light.h>
#include<renderer/scene.h>
#include<renderer/shadow_map.h>
namespace CGL {
	class ShadowMap;
	class Texture2D;
	struct VertexData {
		vec3 m_pos;
		vec3 m_nor;
		vec2 m_tex;
		vec4 m_cpos;
		vec3 m_spos;

		float m_rhw = 0.0f; // for perspective-correction;

		static VertexData lerp(const VertexData& v0, const VertexData& v1, float frac) {
			//Linear interpolation
			VertexData result;
			result.m_pos = (1.0f - frac) * v0.m_pos + frac * v1.m_pos;
			result.m_nor = (1.0f - frac) * v0.m_nor + frac * v1.m_nor;
			result.m_tex = (1.0f - frac) * v0.m_tex + frac * v1.m_tex;
			result.m_cpos = (1.0f - frac) * v0.m_cpos + frac * v1.m_cpos;
			result.m_spos.x = (1.0f - frac) * v0.m_spos.x + frac * v1.m_spos.x;
			result.m_spos.y = (1.0f - frac) * v0.m_spos.y + frac * v1.m_spos.y;
			result.m_rhw = (1.0f - frac) * v0.m_rhw + frac * v1.m_rhw;

			return result;//
		}

		static void  pre_persp_correction(VertexData& v) {
			//Perspective correction: the world space properties should be multipy by 1/w before rasterization
			//https://zhuanlan.zhihu.com/p/144331875
			v.m_rhw = 1.0f / v.m_cpos.w;
			v.m_pos *= v.m_rhw;
			v.m_tex *= v.m_rhw;
			v.m_nor *= v.m_rhw;
		}
		unsigned vertex_id;// hack for postprocessing;
	};

	struct FragmentData {
	public:
		vec3 m_pos;  //World space position
		vec3 m_nor;  //World space normal
		vec2 m_tex;	//World space texture coordinate
		ivec2 m_spos;//Screen space position
		float m_rhw;
		double m_depth{ 0.0f };
		static FragmentData barycentric_lerp(const VertexData& v0, const VertexData& v1, const VertexData& v2, float w0, float w1, float w2) {
			FragmentData result;
			result.m_pos = w0 * v0.m_pos + w1 * v1.m_pos + w2 * v2.m_pos;
			result.m_nor = w0 * v0.m_nor + w1 * v1.m_nor + w2 * v2.m_nor;
			result.m_tex = w0 * v0.m_tex + w1 * v1.m_tex + w2 * v2.m_tex;
			result.m_rhw = w0 * v0.m_rhw + w1 * v1.m_rhw + w2 * v2.m_rhw;
			result.m_spos.x = w0 * v0.m_spos.x + w1 * v1.m_spos.x + w2 * v2.m_spos.x;
			result.m_spos.y = w0 * v0.m_spos.y + w1 * v1.m_spos.y + w2 * v2.m_spos.y;
			return result;
		}
		static void aft_persp_correction(FragmentData& v) {
			//Perspective correction: the world space properties should be multipy by w after rasterization
			//https://zhuanlan.zhihu.com/p/144331875
			float w = 1.0f / v.m_rhw;
			v.m_pos *= w;
			v.m_tex *= w;
			v.m_nor *= w;
		}
	};

	// Data that can change for each object
	struct ObjectUniforms {
		mat4 model;
		mat4 inv_transpose_model;
		// Maybe material properties could go here too if not using textures
	};
	struct SceneUniforms {
		mat4 view;
		mat4 projection;
		mat4 light_space_matrix;
		vec3 view_pos;
		std::shared_ptr<ShadowMap> shadow_map;
		std::vector<std::shared_ptr<Light>> lights;
		std::shared_ptr<Texture2D> scene_texture;

	};

	enum class CullMode {
		None,
		Front,
		Back
	};
}