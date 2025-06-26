#pragma once
#include<renderer/rhi_struct.h>
#include<renderer/texture.h>
namespace CGL {
	class Material {
	public:
		// Basic material properties
		vec3 ambient_color = vec3(0.2f, 0.2f, 0.2f);
		vec3 diffuse_color = vec3(0.8f, 0.8f, 0.8f);
		vec3 specular_color = vec3(1.0f, 1.0f, 1.0f);
		float shininess = 32;          // Specular exponent (phong)

		std::shared_ptr<Texture2D> m_diffuse_texture=nullptr;


	};
	



}