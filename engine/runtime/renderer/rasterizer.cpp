#include<cmath>
#include<algorithm>
#include<renderer/rasterizer.h>
#include<renderer/graphics_pipeline.h>
namespace CGL {

	/*static PhongFragmentShader shader;
	void Rasterizer::rasterizeTriangle(VertexData& v0, VertexData& v1, VertexData& v2, int width, int height) {
		// rasterizeTriangleBarycentric_aux(v0, v1, v2, width, height);
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

					Color color = shader.shade(frag);
					//shader(frag);
					m_framebuffer->write_color(x,y, frag.m_depth,color);
				}
			}
		}




	}*/

	/*/void Rasterizer::rasterizeLine(VertexData& v0, VertexData& v1, int width, int height) {
		BresenhamLine_aux(v0.m_spos.x, v0.m_spos.y, v1.m_spos.x, v1.m_spos.y);
		//drawLineDDA_aux(v0.m_spos.x, v0.m_spos.y, v1.m_spos.x, v1.m_spos.y);


	}



	/////////////////////Implementation///////////////////////////////////////
	void Rasterizer::BresenhamLine_aux(int x0, int y0, int x1, int y1) {
		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int err = dx + dy;  // Error accumulator
		while (true) {
			m_target->write_color(x0, y0);
			if (x0 == x1 && y0 == y1) break;
			int e2 = 2 * err;
			if (e2 >= dy) { err += dy; x0 += sx; }  // X step
			if (e2 <= dx) { err += dx; y0 += sy; }  // Y step
		}
	}

	void Rasterizer::drawLineDDA_aux(float x0, float y0, float x1, float y1) {
		int dx = std::abs(x1 - x0);
		int dy = std::abs(y1 - y0);
		int steps = std::max(dx, dy);
		float xInc = (x1 - x0) / steps;
		float yInc = (y1 - y0) / steps;

		float x = x0;
		float y = y0;
		for (int i = 0; i <= steps; i++) {
			m_target->write_color((int)x, (int)y);
			x += xInc;
			y += yInc;
		}
	}

	std::vector<VertexData> Rasterizer::homogeneous_clipping(VertexData& v0, VertexData& v1, VertexData& v2, float near, float far) {
		return homogeneous_clipping_SutherlandHodgeman_aux(v0, v1, v2, near, far);
	}


	std::vector<VertexData> Rasterizer::homogeneous_clipping_fast_aux(VertexData& v0, VertexData& v1, VertexData& v2, float near, float far) {
		std::vector<VertexData> res;
		return res;
	}

	std::vector<VertexData> Rasterizer::homogeneous_clipping_SutherlandHodgeman_aux(VertexData& v0, VertexData& v1, VertexData& v2, float near, float far) {
		//return { v0,v1,v2 };

		std::vector<VertexData> res;
		auto is_point_inside_frustum = [](const vec4& p, float& near, float& far)->bool {
			return (p.x <= p.w && p.x >= -p.w) && (p.y <= p.w && p.y >= -p.w)
				&& (p.z <= p.w && p.z >= -p.w)
				&& (p.w <= far && p.w >= near);
		};
		//Totally inside
		if (is_point_inside_frustum(v0.m_cpos, near, far) &&
			is_point_inside_frustum(v1.m_cpos, near, far) &&
			is_point_inside_frustum(v2.m_cpos, near, far)) {
			return { v0,v1,v2 };
		}
		//return {};
		//Totally outside
		if (v0.m_cpos.w < near && v1.m_cpos.w < near && v2.m_cpos.w < near)
			return{};
		if (v0.m_cpos.w > far && v1.m_cpos.w > far && v2.m_cpos.w > far)
			return{};

		if (v0.m_cpos.x > v0.m_cpos.w && v1.m_cpos.x > v1.m_cpos.w && v2.m_cpos.x > v2.m_cpos.w)
			return{};
		if (v0.m_cpos.x < -v0.m_cpos.w && v1.m_cpos.x < -v1.m_cpos.w && v2.m_cpos.x < -v2.m_cpos.w)
			return{};

		if (v0.m_cpos.y > v0.m_cpos.w && v1.m_cpos.y > v1.m_cpos.w && v2.m_cpos.y > v2.m_cpos.w)
			return{};
		if (v0.m_cpos.y < -v0.m_cpos.w && v1.m_cpos.y < -v1.m_cpos.w && v2.m_cpos.y < -v2.m_cpos.w)
			return{};

		if (v0.m_cpos.z > v0.m_cpos.w && v1.m_cpos.z > v1.m_cpos.w && v2.m_cpos.z > v2.m_cpos.w)
			return{};
		if (v0.m_cpos.z < -v0.m_cpos.w && v1.m_cpos.z < -v1.m_cpos.w && v2.m_cpos.z < -v2.m_cpos.w)
			return{};

		std::vector<VertexData> insideVertices;
		std::vector<VertexData> tmp = { v0, v1, v2 };
		enum Axis { X = 0, Y = 1, Z = 2 };

		//w=x plane & w=-x plane
		{
			insideVertices = SutherlandHodgeman_clipping_aux(tmp, Axis::X, +1);
			tmp = insideVertices;

			insideVertices = SutherlandHodgeman_clipping_aux(tmp, Axis::X, -1);
			tmp = insideVertices;

			//return insideVertices;
		}

		//w=y plane & w=-y plane
		{
			insideVertices = SutherlandHodgeman_clipping_aux(tmp, Axis::Y, +1);
			tmp = insideVertices;

			insideVertices = SutherlandHodgeman_clipping_aux(tmp, Axis::Y, -1);
			tmp = insideVertices;
		}

		//w=z plane & w=-z plane
		{
			insideVertices = SutherlandHodgeman_clipping_aux(tmp, Axis::Z, +1);
			tmp = insideVertices;

			insideVertices = SutherlandHodgeman_clipping_aux(tmp, Axis::Z, -1);
			tmp = insideVertices;
		}

		//w=1e-5 plane
		{
			insideVertices = {};
			int numVerts = tmp.size();
			constexpr float wClippingPlane = 1e-5;
			for (int i = 0; i < numVerts; ++i)
			{
				const auto& begVert = tmp[(i - 1 + numVerts) % numVerts];
				const auto& endVert = tmp[i];
				float begIsInside = (begVert.m_cpos.w < wClippingPlane) ? -1 : 1;
				float endIsInside = (endVert.m_cpos.w < wClippingPlane) ? -1 : 1;
				//One of them is outside
				if (begIsInside * endIsInside < 0)
				{
					// t = (w_clipping_plane-w1)/((w1-w2)
					float t = (wClippingPlane - begVert.m_cpos.w) / (begVert.m_cpos.w - endVert.m_cpos.w);
					auto intersectedVert = VertexData::lerp(begVert, endVert, t);
					insideVertices.push_back(intersectedVert);
				}
				//If current vertices is inside
				if (endIsInside > 0)
				{
					insideVertices.push_back(endVert);
				}
			}
		}
		return insideVertices;
	}

	std::vector<VertexData> Rasterizer::SutherlandHodgeman_clipping_aux(
		const std::vector<VertexData>& polygon, const int& axis, const int& side) {
		std::vector<VertexData> inside_polygon;
		int num_verts = polygon.size();
		for (int i = 0; i < num_verts; ++i) {
			const auto& start_vert = polygon[(i - 1 + num_verts) % num_verts];
			const auto& end_vert = polygon[i];

			int is_start_vert_inside = ((side * (start_vert.m_cpos[axis]) <= start_vert.m_cpos.w) ? 1 : -1);
			int is_end_vert_inside = ((side * (end_vert.m_cpos[axis]) <= end_vert.m_cpos.w) ? 1 : -1);

			if (is_start_vert_inside * is_end_vert_inside < 0) { // intersection
				float t = (start_vert.m_cpos.w - side * start_vert.m_cpos[axis])
					/ ((start_vert.m_cpos.w - side * start_vert.m_cpos[axis]) - (end_vert.m_cpos.w - side * end_vert.m_cpos[axis]));
				auto intersected = VertexData::lerp(start_vert, end_vert, t);
				inside_polygon.push_back(intersected);
			}
			//If current vertices is inside
			if (is_end_vert_inside > 0) {
				inside_polygon.push_back(end_vert);
			}
		}
		return inside_polygon;
	}*/

}