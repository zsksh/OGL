#include <stb/stb_image.h>
#include<resource/assetmanager.h>
#include<mesh/mesh_io.h>
namespace CGL {
	std::shared_ptr<StaticMesh> AssetManager::loadMesh(std::string filename) {
		std::shared_ptr<StaticMesh> mesh = std::make_shared<StaticMesh>();
		load_obj_file(filename, *mesh);
		return mesh;
	}

	std::shared_ptr<Texture2D> AssetManager::loadTexture(std::string path) {
		std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>();
		//stbi_set_flip_vertically_on_load(true);
		TextureData textureData;
		unsigned char* pixelData = stbi_load(path.c_str(), &textureData.width, &textureData.height, &textureData.channels, 0);
		if (!pixelData) {
			throw std::runtime_error("Failed to load texture file: " + path);
		}
		textureData.pixels = pixelData;
		texture->initialize(textureData);
		return texture;
	}

}