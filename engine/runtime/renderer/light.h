#pragma once
#include<core/vector.h>
#include<core/math.h>
namespace CGL {
	class Light {
	public:
		typedef std::shared_ptr<Light> ptr;
		Light() : m_intensity(vec3(1.0f, 1.0f, 1.0f)) {}
		Light(const vec3& intensity) : m_intensity(intensity) {}
		virtual ~Light() = default;
		const vec3& intensity() const { return m_intensity; }
		virtual float attenuation(const vec3& fragPos) const = 0;
		virtual float cutoff(const vec3& lightDir) const = 0;
		virtual vec3 direction(const vec3& fragPos) const = 0;
		virtual vec3 getDirection() = 0;
		virtual void setPosition(vec3&) = 0;
		virtual vec3 getPosition() = 0;
	public:
		vec3 m_intensity;
	};

	//Point light source
	class PointLight : public Light {
	public:
		typedef std::shared_ptr<PointLight> ptr;
		PointLight(const vec3& intensity, const vec3& lightPos, const vec3& atten)
			: Light(intensity), m_position(lightPos), m_attenuation(atten) { }

		virtual float attenuation(const vec3& fragPos) const override {
			//Refs: https://learnopengl.com/Lighting/Light-casters
			float distance = glm::length(m_position - fragPos);
			return 1.0 / (m_attenuation.x + m_attenuation.y * distance + m_attenuation.z * (distance * distance));
		}
		virtual vec3 direction(const vec3& fragPos) const override {
			return normalize(m_position - fragPos);
		}
		virtual float cutoff(const vec3& lightDir) const override { return 1.0f; }
		virtual vec3 getPosition() override { return m_position; }
		virtual void setPosition(vec3& val) { m_position = val; }
		virtual vec3 getDirection() { return m_position; }

	public:
		vec3 m_attenuation;
		vec3 m_position;//Note: world space position of light source

	};

	//Spot light source
	class SpotLight final : public PointLight {
	public:
		typedef std::shared_ptr<SpotLight> ptr;
		SpotLight(const vec3& intensity, const vec3& lightPos, const vec3& atten, const vec3& dir,
			const float& innerCutoff, const float& outerCutoff) : PointLight(intensity, lightPos, atten),
			m_spotDir(normalize(dir)), m_innerCutoff(innerCutoff), m_outerCutoff(outerCutoff) { }

		virtual float cutoff(const vec3& lightDir) const override {
			float theta = dot(normalize(lightDir), -normalize(m_spotDir));
			static const float epsilon = m_innerCutoff - m_outerCutoff;
			return clamp((theta - m_outerCutoff) / epsilon, 0.0f, 1.0f);
		}
		vec3& getSpotDirection() { return m_spotDir; }
	private:
		vec3 m_spotDir;
		float m_innerCutoff;
		float m_outerCutoff;
	};

	class DirectionalLight final : public Light {
	public:
		typedef std::shared_ptr<DirectionalLight> ptr;
		DirectionalLight(const vec3& intensity, const vec3& dir)
			: Light(intensity), m_lightDir(normalize(dir)) { }

		virtual float attenuation(const vec3& fragPos) const override { return 1.0f; }
		virtual vec3 direction(const vec3& fragPos) const override { return m_lightDir; }
		virtual float cutoff(const vec3& lightDir) const override { return 1.0f; }

		//virtual vec3 getDirection() { return m_lightDir; }

		virtual vec3 getDirection() override { return m_lightDir; };
		virtual void setPosition(vec3& val) { m_lightDir = val; }
		//virtrual vec3 getPosition() override { return m_position; }
		virtual vec3 getPosition() override { return m_lightDir; };


	private:
		vec3 m_lightDir;
	};
	
}