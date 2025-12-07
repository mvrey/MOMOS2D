#ifndef SPRITE_ATLAS_H
#define SPRITE_ATLAS_H 1

#include <MOMOS/texture.h>
#include <string>
#include <map>
#include <vector>

namespace MOMOS {

	// Represents a region within a sprite atlas
	struct SpriteRegion {
		// UV coordinates (normalized 0-1)
		float u_min, v_min;  // Top-left corner
		float u_max, v_max;  // Bottom-right corner
		
		// Pixel dimensions of the sprite
		int width, height;
		
		// Optional: offset from original image (for trimmed sprites)
		int offset_x, offset_y;
		
		SpriteRegion() : u_min(0), v_min(0), u_max(1), v_max(1), 
		                 width(0), height(0), offset_x(0), offset_y(0) {}
		
		SpriteRegion(float u0, float v0, float u1, float v1, int w, int h, int ox = 0, int oy = 0)
			: u_min(u0), v_min(v0), u_max(u1), v_max(v1), 
			  width(w), height(h), offset_x(ox), offset_y(oy) {}
	};

	// Manages a sprite atlas texture and its regions
	class SpriteAtlas {
	public:
		SpriteAtlas();
		~SpriteAtlas();
		
		// Load atlas texture from file
		bool LoadFromFile(const char* texture_path, bool alpha = true);
		
		// Load atlas from JSON file (TexturePacker format or custom format)
		bool LoadFromJSON(const char* json_path, const char* texture_path = nullptr);
		
		// Add a sprite region to the atlas
		// x, y, width, height are in pixels relative to the atlas texture
		void AddRegion(const std::string& name, int x, int y, int width, int height);
		
		// Add a sprite region with custom UV coordinates
		void AddRegion(const std::string& name, const SpriteRegion& region);
		
		// Get a sprite region by name
		const SpriteRegion* GetRegion(const std::string& name) const;
		
		// Get the atlas texture
		Texture2D* GetTexture() { return texture_; }
		const Texture2D* GetTexture() const { return texture_; }
		
		// Check if a region exists
		bool HasRegion(const std::string& name) const;
		
		// Get all region names
		std::vector<std::string> GetRegionNames() const;
		
		// Get atlas dimensions
		int GetWidth() const { return texture_ ? texture_->Width : 0; }
		int GetHeight() const { return texture_ ? texture_->Height : 0; }
		
	private:
		Texture2D* texture_;
		std::map<std::string, SpriteRegion> regions_;
	};

} // namespace MOMOS

#endif // SPRITE_ATLAS_H
