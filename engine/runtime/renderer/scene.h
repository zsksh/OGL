#pragma once
#include<memory>
#include<string>
#include<mesh/mesh.h>
#include<resource/assetmanager.h>
#include<renderer/material.h>
#include<renderer/light.h>
#include<core/matrix.h>
namespace CGL {
	struct Transform {
		void set_position(vec3 pos) { m_pos = pos; }
		void set_scale(vec3 scale) { m_s = scale; }
		void set_rotation(vec3 rot) { m_rot = rot; }
		Transform(vec3 pos = vec3(0, 0, 0), vec3 rot = vec3(0, 0, 0), vec3 s = vec3(1, 1, 1))
			: m_pos(pos), m_rot(rot), m_s(s) {
		};
		mat4 get_matrix() {
			//glm::translate
			glm::mat4 gmodel = glm::mat4(1.0f);
			gmodel = glm::translate(gmodel, glm::vec3(m_pos[0], m_pos[1], m_pos[2])); // row-major = S * R * T
			gmodel = glm::scale(gmodel, glm::vec3(m_s[0], m_s[1], m_s[2]));

			gmodel = glm::rotate(gmodel, m_rot.x, glm::vec3(1, 0, 0));
			gmodel = glm::rotate(gmodel, m_rot.y, glm::vec3(0, 1, 0));
			gmodel = glm::rotate(gmodel, m_rot.z, glm::vec3(0, 0, 1));


			return gmodel;
			mat4 model;


			for (int x = 0; x < 4; x++) {
				for (int y = 0; y < 4; y++) {
					model[x][y] = gmodel[y][x];
				}
			}
			return model;

			//mat4 model = CGL::translate(mat4::identity(), m_pos);
			//model = CGL::rotate(model, m_rot.x, vec3(1,0,0));
			//model = CGL::rotate(model, m_rot.y, vec3(0,1,0));
			//model = CGL::rotate(model, m_rot.z, vec3(0,0,1));
			/*model = CGL::scale(model, m_s);
			//return model;

			mat4 scale = CGL::scale(mat4::identity(), m_s);
			mat4 rot = CGL::rotate(mat4::identity(), m_rot.x, vec3(1, 0, 0));
			rot = CGL::rotate(rot, m_rot.y, vec3(0, 1, 0));
			rot = CGL::rotate(rot, m_rot.z, vec3(0, 0, 1));

			mat4 trans = CGL::translate(mat4::identity(), m_pos);

			// Combine in correct order: T * R * S
			return  scale * rot * trans;

			//trans * rot * scale;*/


		};
		vec3 m_pos;
		vec3 m_rot;// to do
		vec3 m_s;
	};
	struct RenderObject {
		RenderObject(std::shared_ptr<StaticMesh> mmesh) :
			mesh(mmesh), transform(std::make_shared<Transform>()) {}
		std::shared_ptr<StaticMesh> mesh;
		std::shared_ptr<Material> material;
		std::shared_ptr<Transform> transform;
		std::string name;
	};


	class Scene {
	public:
		void addObject(const RenderObject& object) {
			m_objects.push_back(object);
		}
		const std::vector<RenderObject>& getObjects() const {
			return m_objects;
		}
		std::vector<RenderObject>& getObjects() {
			return m_objects;
		}
		void addLight(const std::shared_ptr<Light> light) {  // Add const
			m_lights.push_back(light);  // Now works for both const/non-const
		}
		std::vector<std::shared_ptr<Light>>& getLights() {
			return m_lights;
		}

		const std::vector<std::shared_ptr<Light>>& getLights() const {
			return m_lights;
		}
		virtual void update() {};

	private:
		std::vector<RenderObject> m_objects;
		std::vector<std::shared_ptr<Light>> m_lights;
	};

	inline RenderObject createRenderObject(std::string mesh_path, std::string texture_path,
		vec3 position = vec3(0, 0, 0), vec3 scale = vec3(1, 1, 1), vec3 rot = vec3(0, 0, 0)) {
		auto static_mesh = AssetManager::loadMesh(mesh_path);
		//auto texture = AssetManager::loadTexture(texture_path);
		std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>();;
		texture->load_texture(texture_path);
		auto material = std::make_shared<Material>();
		material->m_diffuse_texture = texture;
		RenderObject renderobj(static_mesh);
		renderobj.transform->set_position(position);
		renderobj.transform->set_scale(scale);
		renderobj.transform->set_rotation(rot);
		renderobj.material = material;
		return renderobj;
	}
	inline RenderObject createRenderObject(std::string mesh_path, vec3 color=vec3(0.1f,0.1f,0.1f),
		vec3 position = vec3(0, 0, 0), vec3 scale = vec3(1, 1, 1),vec3 rot=vec3(0,0,0)) {
		auto static_mesh = AssetManager::loadMesh(mesh_path);
		//auto texture = AssetManager::loadTexture(texture_path);
		auto material = std::make_shared<Material>();
		material->m_diffuse_texture = nullptr;
		material->diffuse_color = color;
		RenderObject renderobj(static_mesh);
		renderobj.transform->set_position(position);
		renderobj.transform->set_scale(scale);
		renderobj.transform->set_rotation(rot);
		renderobj.material = material;
		return renderobj;
	}

}


