#include <MOMOS/sprite.h>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <MOMOS/shader.h>
#include <MOMOS/texture.h>

namespace MOMOS {

	SpriteRenderer* renderer;

	SpriteHandle SpriteFromFile(const char *path) {
		// Load textures
		Texture2D* texture = ResourceManager::LoadTexture(path, GL_TRUE, path);
		// Set render-specific controls
		MOMOS::renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

		return texture;
	}


	SpriteHandle SpriteFromMemory(int width, int height, const unsigned char *data_RGBA) {
		return nullptr;
	}


	void SpriteUpdateFromMemory(SpriteHandle, const unsigned char *data_RGBA) {

	}


	SpriteHandle SubSprite(SpriteHandle orig, int x, int y, int width, int height) {
		return nullptr;
	}


	void SpriteRelease(SpriteHandle img) {

	}


	int SpriteHeight(SpriteHandle img) {
		Texture2D* tex = static_cast<Texture2D*>(img);
		return tex->Height;
	}


	int SpriteWidth(SpriteHandle img) {
		Texture2D* tex = static_cast<Texture2D*>(img);
		return tex->Width;
	}


	void SpriteGetPixel(SpriteHandle img, int x, int y, unsigned char outRGBA[4]) {

	}


	void DrawSprite(SpriteHandle img, float x, float y) {
		Texture2D* tex = static_cast<Texture2D*>(img);
		MOMOS::renderer->DrawSprite(*tex, glm::vec2(x,y), glm::vec2(tex->Width, tex->Height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}


	void DrawSpriteWithMatrix(SpriteHandle img, const float transform_matrix[9]) {
		Texture2D* tex = static_cast<Texture2D*>(img);
		MOMOS::renderer->DrawSprite(*tex, glm::vec2(transform_matrix[2], transform_matrix[5]), glm::vec2(tex->Width*transform_matrix[0], tex->Height*transform_matrix[3]), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}


	void DrawSpriteWithMatrix(SpriteHandle img, const MOMOS::Mat3 &m) {
		Texture2D* tex = static_cast<Texture2D*>(img);
		MOMOS::renderer->DrawSprite(*tex, glm::vec2(m.d[2], m.d[5]), glm::vec2(tex->Width*m.d[0], tex->Height*m.d[3]), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}


	void DrawSprite(SpriteHandle img, const SpriteTransform &st) {
		Texture2D* tex = static_cast<Texture2D*>(img);
		MOMOS::renderer->DrawSprite(*tex,glm::vec2(st.x,st.y),
			glm::vec2(tex->Width * st.scale_x, tex->Height * st.scale_y),
			st.angle,
			glm::vec3(1.0f, 1.0f, 1.0f));
	}

} /* MOMOS */