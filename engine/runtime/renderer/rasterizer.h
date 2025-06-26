#pragma once
#include<memory>
#include<core/predicate.h>
#include<renderer/rhi_struct.h>
#include<renderer/framebuffer.h>
namespace CGL {

	class Rasterizer {
	public:
		template<typename FragShader>
		void rasterizeTriangle(VertexData& v0, VertexData& v1, VertexData& v2, int width, int height, FragShader shader);

	private:
		template<typename FragShader>
		void rasterizeTriangleBarycentric_aux(VertexData& v0, VertexData& v1, VertexData& v2, int width, int height, FragShader shader);


		//std::shared_ptr<Framebuffer> m_framebuffer = nullptr;


	};

	template<typename FragShader>
	void Rasterizer::rasterizeTriangle(VertexData& v0, VertexData& v1, VertexData& v2, int width, int height, FragShader shader) {
		rasterizeTriangleBarycentric_aux(v0, v1, v2, width, height, shader);

	}

	template<typename FragShader>
	void Rasterizer::rasterizeTriangleBarycentric_aux(VertexData& v0, VertexData& v1, VertexData& v2,
		int width, int height, FragShader shader) {
		int aabb_x_min = std::max(int(std::floor(std::min(std::min(v0.m_spos.x, v1.m_spos.x), v2.m_spos.x))), 0);
		int aabb_y_min = std::max(int(std::floor(std::min(std::min(v0.m_spos.y, v1.m_spos.y), v2.m_spos.y))), 0);
		int aabb_x_max = std::min(int(std::ceil(std::max(std::max(v0.m_spos.x, v1.m_spos.x), v2.m_spos.x))), width - 1);
		int aabb_y_max = std::min(int(std::ceil(std::max(std::max(v0.m_spos.y, v1.m_spos.y), v2.m_spos.y))), height - 1);
		for (int x = aabb_x_min; x <= aabb_x_max; x++) {
			for (int y = aabb_y_min; y <= aabb_y_max; y++) {
				float alpha, beta, gamma;
				if (is_point_inside_triangle(v0.m_spos, v1.m_spos, v2.m_spos, x, y, alpha, beta, gamma)) {
					float screen_z = alpha * v0.m_spos.z + beta * v1.m_spos.z + gamma * v2.m_spos.z;
					auto frag = FragmentData::
						barycentric_lerp(v0, v1, v2, alpha, beta, gamma);
					frag.m_spos.x = x;
					frag.m_spos.y = y;
					frag.m_depth = alpha * v0.m_spos.z + beta * v1.m_spos.z + gamma * v2.m_spos.z;
					frag.aft_persp_correction(frag);
					shader(frag);
				}
			}
		}
	}


}