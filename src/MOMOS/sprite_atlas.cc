#include <MOMOS/sprite_atlas.h>
#include <MOMOS/resource_manager.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cstring>

namespace MOMOS {

	SpriteAtlas::SpriteAtlas() : texture_(nullptr) {
	}

	SpriteAtlas::~SpriteAtlas() {
		// Texture is managed by ResourceManager if loaded through it
		// Otherwise, it should be cleaned up by the caller
		texture_ = nullptr;
	}

	bool SpriteAtlas::LoadFromFile(const char* texture_path, bool alpha) {
		if (texture_path == nullptr) {
			return false;
		}

		// Load texture through ResourceManager
		texture_ = ResourceManager::LoadTexture(texture_path, alpha ? GL_TRUE : GL_FALSE, texture_path);
		
		return texture_ != nullptr && texture_->ID != 0;
	}

	void SpriteAtlas::AddRegion(const std::string& name, int x, int y, int width, int height) {
		if (texture_ == nullptr || width <= 0 || height <= 0) {
			return;
		}

		// Calculate normalized UV coordinates
		float u_min = static_cast<float>(x) / static_cast<float>(texture_->Width);
		float v_min = static_cast<float>(y) / static_cast<float>(texture_->Height);
		float u_max = static_cast<float>(x + width) / static_cast<float>(texture_->Width);
		float v_max = static_cast<float>(y + height) / static_cast<float>(texture_->Height);

		// Clamp to valid range
		u_min = std::max(0.0f, std::min(1.0f, u_min));
		v_min = std::max(0.0f, std::min(1.0f, v_min));
		u_max = std::max(0.0f, std::min(1.0f, u_max));
		v_max = std::max(0.0f, std::min(1.0f, v_max));

		regions_[name] = SpriteRegion(u_min, v_min, u_max, v_max, width, height, 0, 0);
	}

	void SpriteAtlas::AddRegion(const std::string& name, const SpriteRegion& region) {
		regions_[name] = region;
	}

	const SpriteRegion* SpriteAtlas::GetRegion(const std::string& name) const {
		auto it = regions_.find(name);
		if (it != regions_.end()) {
			return &it->second;
		}
		return nullptr;
	}

	bool SpriteAtlas::HasRegion(const std::string& name) const {
		return regions_.find(name) != regions_.end();
	}

	std::vector<std::string> SpriteAtlas::GetRegionNames() const {
		std::vector<std::string> names;
		names.reserve(regions_.size());
		for (const auto& pair : regions_) {
			names.push_back(pair.first);
		}
		return names;
	}

	// Simple JSON parsing helpers
	namespace {
		void SkipWhitespace(const char*& p) {
			while (*p && std::isspace(*p)) ++p;
		}

		bool ExpectChar(const char*& p, char c) {
			SkipWhitespace(p);
			if (*p == c) {
				++p;
				return true;
			}
			return false;
		}

		std::string ParseString(const char*& p) {
			SkipWhitespace(p);
			if (*p != '"') return "";
			++p;
			std::string result;
			while (*p && *p != '"') {
				if (*p == '\\' && p[1]) {
					++p;
					switch (*p) {
						case 'n': result += '\n'; break;
						case 't': result += '\t'; break;
						case 'r': result += '\r'; break;
						case '\\': result += '\\'; break;
						case '"': result += '"'; break;
						default: result += *p; break;
					}
				} else {
					result += *p;
				}
				++p;
			}
			if (*p == '"') ++p;
			return result;
		}

		int ParseInt(const char*& p) {
			SkipWhitespace(p);
			int sign = 1;
			if (*p == '-') {
				sign = -1;
				++p;
			}
			int result = 0;
			while (*p && std::isdigit(*p)) {
				result = result * 10 + (*p - '0');
				++p;
			}
			return result * sign;
		}

		bool ParseBool(const char*& p) {
			SkipWhitespace(p);
			if (strncmp(p, "true", 4) == 0) {
				p += 4;
				return true;
			}
			if (strncmp(p, "false", 5) == 0) {
				p += 5;
				return false;
			}
			return false;
		}
	}

	bool SpriteAtlas::LoadFromJSON(const char* json_path, const char* texture_path) {
		if (json_path == nullptr) {
			return false;
		}

		std::ifstream file(json_path);
		if (!file.is_open()) {
			return false;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string json_content = buffer.str();
		file.close();

		const char* p = json_content.c_str();
		SkipWhitespace(p);

		// Try to parse TexturePacker JSON format
		if (!ExpectChar(p, '{')) {
			return false;
		}

		std::string image_path;
		int atlas_width = 0, atlas_height = 0;

		// Parse metadata
		while (*p && *p != '}') {
			SkipWhitespace(p);
			std::string key = ParseString(p);
			SkipWhitespace(p);
			
			if (key == "meta") {
				if (!ExpectChar(p, ':')) continue;
				if (!ExpectChar(p, '{')) continue;
				while (*p && *p != '}') {
					std::string meta_key = ParseString(p);
					SkipWhitespace(p);
					if (!ExpectChar(p, ':')) continue;
					if (meta_key == "image") {
						image_path = ParseString(p);
					} else if (meta_key == "size") {
						if (ExpectChar(p, '{')) {
							std::string size_key = ParseString(p);
							if (size_key == "w") {
								SkipWhitespace(p);
								if (ExpectChar(p, ':')) {
									atlas_width = ParseInt(p);
								}
							} else if (size_key == "h") {
								SkipWhitespace(p);
								if (ExpectChar(p, ':')) {
									atlas_height = ParseInt(p);
								}
							}
							SkipWhitespace(p);
							if (*p == ',') ++p;
							size_key = ParseString(p);
							if (size_key == "h") {
								SkipWhitespace(p);
								if (ExpectChar(p, ':')) {
									atlas_height = ParseInt(p);
								}
							} else if (size_key == "w") {
								SkipWhitespace(p);
								if (ExpectChar(p, ':')) {
									atlas_width = ParseInt(p);
								}
							}
							ExpectChar(p, '}');
						}
					}
					SkipWhitespace(p);
					if (*p == ',') ++p;
				}
				ExpectChar(p, '}');
			} else if (key == "frames") {
				if (!ExpectChar(p, ':')) continue;
				if (!ExpectChar(p, '{')) continue;
				
				// Load texture if not provided
				if (texture_ == nullptr) {
					if (texture_path != nullptr) {
						LoadFromFile(texture_path, true);
					} else if (!image_path.empty()) {
						// Try to resolve image path relative to JSON file
						std::string json_dir = json_path;
						size_t last_slash = json_dir.find_last_of("/\\");
						if (last_slash != std::string::npos) {
							json_dir = json_dir.substr(0, last_slash + 1);
							LoadFromFile((json_dir + image_path).c_str(), true);
						} else {
							LoadFromFile(image_path.c_str(), true);
						}
					}
				}

				// Parse frames
				while (*p && *p != '}') {
					SkipWhitespace(p);
					std::string frame_name = ParseString(p);
					SkipWhitespace(p);
					if (!ExpectChar(p, ':')) break;
					if (!ExpectChar(p, '{')) break;

					int x = 0, y = 0, w = 0, h = 0;
					int offset_x = 0, offset_y = 0;
					int orig_w = 0, orig_h = 0;

					while (*p && *p != '}') {
						std::string prop = ParseString(p);
						SkipWhitespace(p);
						if (!ExpectChar(p, ':')) break;

						if (prop == "x") {
							x = ParseInt(p);
						} else if (prop == "y") {
							y = ParseInt(p);
						} else if (prop == "w") {
							w = ParseInt(p);
						} else if (prop == "h") {
							h = ParseInt(p);
						} else if (prop == "offX") {
							offset_x = ParseInt(p);
						} else if (prop == "offY") {
							offset_y = ParseInt(p);
						} else if (prop == "sourceW") {
							orig_w = ParseInt(p);
						} else if (prop == "sourceH") {
							orig_h = ParseInt(p);
						} else if (prop == "frame") {
							// Nested frame object
							if (ExpectChar(p, '{')) {
								while (*p && *p != '}') {
									std::string frame_prop = ParseString(p);
									SkipWhitespace(p);
									if (!ExpectChar(p, ':')) break;
									if (frame_prop == "x") x = ParseInt(p);
									else if (frame_prop == "y") y = ParseInt(p);
									else if (frame_prop == "w") w = ParseInt(p);
									else if (frame_prop == "h") h = ParseInt(p);
									SkipWhitespace(p);
									if (*p == ',') ++p;
								}
								ExpectChar(p, '}');
							}
						} else {
							// Skip unknown properties
							SkipWhitespace(p);
							if (*p == '"') ParseString(p);
							else if (*p == '{') {
								int depth = 1;
								++p;
								while (*p && depth > 0) {
									if (*p == '{') ++depth;
									else if (*p == '}') --depth;
									++p;
								}
							} else if (*p == '[') {
								int depth = 1;
								++p;
								while (*p && depth > 0) {
									if (*p == '[') ++depth;
									else if (*p == ']') --depth;
									++p;
								}
							} else {
								while (*p && *p != ',' && *p != '}') ++p;
							}
						}
						SkipWhitespace(p);
						if (*p == ',') ++p;
					}
					ExpectChar(p, '}');

					if (w > 0 && h > 0) {
						AddRegion(frame_name, x, y, w, h);
						if (offset_x != 0 || offset_y != 0) {
							// Update region with offset
							auto it = regions_.find(frame_name);
							if (it != regions_.end()) {
								it->second.offset_x = offset_x;
								it->second.offset_y = offset_y;
							}
						}
					}

					SkipWhitespace(p);
					if (*p == ',') ++p;
				}
				ExpectChar(p, '}');
			} else {
				// Skip unknown top-level keys
				SkipWhitespace(p);
				if (*p == '"') ParseString(p);
				else if (*p == '{') {
					int depth = 1;
					++p;
					while (*p && depth > 0) {
						if (*p == '{') ++depth;
						else if (*p == '}') --depth;
						++p;
					}
				} else if (*p == '[') {
					int depth = 1;
					++p;
					while (*p && depth > 0) {
						if (*p == '[') ++depth;
						else if (*p == ']') --depth;
						++p;
					}
				} else {
					while (*p && *p != ',' && *p != '}') ++p;
				}
			}
			SkipWhitespace(p);
			if (*p == ',') ++p;
		}

		return texture_ != nullptr && texture_->ID != 0;
	}

} // namespace MOMOS
