#include <MOMOS/sprite.h>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstring>
#include <vector>

#include <MOMOS/shader.h>
#include <MOMOS/texture.h>

namespace MOMOS {

	SpriteRenderer* renderer = nullptr;

namespace {

void EnsureRenderer() {
	if (MOMOS::renderer == nullptr) {
		MOMOS::renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	}
}

bool IsManagedTexture(Texture2D* texture) {
	for (auto& entry : ResourceManager::Textures) {
		if (&entry.second == texture) {
			return true;
		}
	}
	return false;
}

}

	SpriteHandle SpriteFromFile(const char *path) {
		EnsureRenderer();
		// Load textures
		Texture2D* texture = ResourceManager::LoadTexture(path, GL_TRUE, path);
		return texture;
	}


	SpriteHandle SpriteFromMemory(int width, int height, const unsigned char *data_RGBA) {
		if (width <= 0 || height <= 0 || data_RGBA == nullptr) {
			return nullptr;
		}

		EnsureRenderer();

		Texture2D* texture = new Texture2D();
		texture->Internal_Format = GL_RGBA;
		texture->Image_Format = GL_RGBA;
		texture->Wrap_S = GL_CLAMP_TO_EDGE;
		texture->Wrap_T = GL_CLAMP_TO_EDGE;

		const size_t byte_count = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
		std::vector<unsigned char> buffer(data_RGBA, data_RGBA + byte_count);
		texture->Generate(static_cast<GLuint>(width), static_cast<GLuint>(height), buffer.data());

		return texture;
	}


	void SpriteUpdateFromMemory(SpriteHandle img, const unsigned char *data_RGBA) {
		if (img == nullptr || data_RGBA == nullptr) {
			return;
		}

		Texture2D* texture = static_cast<Texture2D*>(img);
		const size_t byte_count = static_cast<size_t>(texture->Width) * static_cast<size_t>(texture->Height) * texture->BytesPerPixel();
		if (byte_count == 0) {
			return;
		}

		std::vector<unsigned char> buffer(data_RGBA, data_RGBA + byte_count);
		texture->Generate(texture->Width, texture->Height, buffer.data());
	}


	SpriteHandle SubSprite(SpriteHandle orig, int x, int y, int width, int height) {
		if (orig == nullptr || width <= 0 || height <= 0) {
			return nullptr;
		}

		Texture2D* source = static_cast<Texture2D*>(orig);
		if (x < 0 || y < 0 || x + width > static_cast<int>(source->Width) || y + height > static_cast<int>(source->Height)) {
			return nullptr;
		}

		const unsigned char* data = source->PixelData();
		unsigned int bpp = source->BytesPerPixel();
		if (data == nullptr || bpp == 0) {
			return nullptr;
		}

		std::vector<unsigned char> buffer(static_cast<size_t>(width) * static_cast<size_t>(height) * bpp);
		for (int row = 0; row < height; ++row) {
			size_t src_index = (static_cast<size_t>(y + row) * source->Width + static_cast<size_t>(x)) * bpp;
			size_t dst_index = static_cast<size_t>(row) * width * bpp;
			std::memcpy(buffer.data() + dst_index, data + src_index, static_cast<size_t>(width) * bpp);
		}

		Texture2D* texture = new Texture2D();
		texture->Internal_Format = source->Internal_Format;
		texture->Image_Format = source->Image_Format;
		texture->Wrap_S = source->Wrap_S;
		texture->Wrap_T = source->Wrap_T;
		texture->Filter_Min = source->Filter_Min;
		texture->Filter_Max = source->Filter_Max;
		texture->Generate(static_cast<GLuint>(width), static_cast<GLuint>(height), buffer.data());

		return texture;
	}


	void SpriteRelease(SpriteHandle img) {
		if (img == nullptr) {
			return;
		}

		Texture2D* texture = static_cast<Texture2D*>(img);
		if (IsManagedTexture(texture)) {
			return;
		}

		if (texture->ID != 0) {
			glDeleteTextures(1, &texture->ID);
		}
		delete texture;
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