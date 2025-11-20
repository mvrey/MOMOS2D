#include <MOMOS/sprite.h>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstring>

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
	if (outRGBA == nullptr) {
		return;
	}

	outRGBA[0] = outRGBA[1] = outRGBA[2] = 0;
	outRGBA[3] = 255;

	if (img == nullptr) {
		return;
	}

	Texture2D* tex = static_cast<Texture2D*>(img);
	if (tex->PixelData() == nullptr || tex->BytesPerPixel() == 0) {
		return;
	}

	if (x < 0 || y < 0 || x >= static_cast<int>(tex->Width) || y >= static_cast<int>(tex->Height)) {
		return;
	}

	const unsigned char* data = tex->PixelData();
	const unsigned int bpp = tex->BytesPerPixel();
	const size_t index = (static_cast<size_t>(y) * tex->Width + static_cast<size_t>(x)) * bpp;

	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 255;

	switch (tex->Image_Format) {
	case GL_BGRA:
		b = data[index + 0];
		g = data[index + 1];
		r = data[index + 2];
		if (bpp > 3) a = data[index + 3];
		break;
	case GL_BGR:
		b = data[index + 0];
		g = data[index + 1];
		r = data[index + 2];
		break;
	case GL_RGBA:
		r = data[index + 0];
		g = data[index + 1];
		b = data[index + 2];
		if (bpp > 3) a = data[index + 3];
		break;
	case GL_RGB:
		r = data[index + 0];
		g = data[index + 1];
		b = data[index + 2];
		break;
	case GL_ALPHA:
		a = data[index];
		break;
	case GL_RED:
	default:
		r = data[index];
		g = r;
		b = r;
		break;
	}

	outRGBA[0] = r;
	outRGBA[1] = g;
	outRGBA[2] = b;
	outRGBA[3] = a;
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