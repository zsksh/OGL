#pragma once
#include<memory>
#include<string>
#include<mesh/mesh.h>
#include<renderer/texture.h>
namespace CGL {
	class AssetManager {
	public:
		static std::shared_ptr<StaticMesh> loadMesh(std::string filename);
		static std::shared_ptr<Texture2D> loadTexture(std::string filename);

		// to do:: cach resources;
	};



}