/* Copyright 2017. All rights reserved.			   _____   ________      _____   ________     _________
* Author: Marcos Vazquez <mrkvr84@gmail.com>	  /     \  \_____  \    /     \  \_____  \   /   _____/
*												 /  \ /  \  /   |   \  /  \ /  \  /   |   \  \_____  \
* Sprite commands	          					/    Y    \/    |    \/    Y    \/    |    \ /        \
*												\____|____/\_________/\____|____/\_________//_________/
*/

#ifndef MOMOS_SPRITE_H
#define MOMOS_SPRITE_H 1

#include <MOMOS/math.h>
#include <MOMOS/sprite_renderer.h>
#include <MOMOS/resource_manager.h>

namespace MOMOS {
  // Forward declaration
  class SpriteAtlas;
  struct SpriteRegion;

  typedef void* SpriteHandle;
  typedef void* SpriteAtlasHandle;

  SpriteHandle SpriteFromFile(const char *path);
  SpriteHandle SpriteFromMemory(int width, int height, const unsigned char *data_RGBA = 0L);
  void SpriteUpdateFromMemory(SpriteHandle, const unsigned char *data_RGBA);

  SpriteHandle SubSprite(SpriteHandle orig, int x, int y, int width, int height);

  void SpriteRelease(SpriteHandle img);
  int SpriteHeight(SpriteHandle img);
  int SpriteWidth(SpriteHandle img);
  void SpriteGetPixel(SpriteHandle img, int x, int y, unsigned char outRGBA[4]);  
  
  void DrawSprite(SpriteHandle img, float x, float y);

  // Sets a global alpha multiplier applied to subsequent DrawSprite calls. The
  // texture's per-pixel alpha is multiplied by this value before blending.
  // Default is 1.0; reset to 1.0 after drawing the affected sprite.
  void DrawSetSpriteAlpha(float alpha);

  // Draws an Sprite with a transformation matrix. The matrix values are specified as
  // a column-major 3x3 matrix. Only six values are used as shown:
  //     | m1 m4 m7 |
  // M = | m2 m5 m8 |  --> m = [m1,m2,m3,m4,m5,m6,m7,m8,m9]
  //     | m3 m6 m9 |

  void DrawSpriteWithMatrix(SpriteHandle img, const float transform_matrix[9]);

  // Draws an Sprite with MOMOS::Mat3 as input
  void DrawSpriteWithMatrix(SpriteHandle img, const MOMOS::Mat3 &m);

  struct SpriteTransform {
    float x,y;                // position where to draw the sprite (default 0,0) 
    float angle;              // rotation in radians (default 0)
    float scale_x, scale_y;   // scale (default 1,1)
    float sprite_origin_x,
          sprite_origin_y;    // origin relative to sprite Coordinates in pixels (default 0,0)
  };

  // Initializes SpriteTransform with default values:
  inline void SpriteTransformInit(SpriteTransform *st) {
    st->x = 0.0f;
    st->y = 0.0f;
    st->angle = 0.0f;
    st->scale_x = 1.0f;
    st->scale_y = 1.0f;
    st->sprite_origin_x = 0.0f;
    st->sprite_origin_y = 0.0f;
  }

  void DrawSprite(SpriteHandle img, const SpriteTransform &st);

  // ============================================================================
  // Sprite Atlas API
  // ============================================================================
  
  // Create a sprite atlas from a texture file (programmatic creation)
  SpriteAtlasHandle SpriteAtlasCreate(const char* texture_path, bool alpha = true);
  
  // Load a sprite atlas from a JSON file (TexturePacker format)
  // If texture_path is nullptr, it will try to use the path from the JSON metadata
  SpriteAtlasHandle SpriteAtlasLoadFromJSON(const char* json_path, const char* texture_path = nullptr);
  
  // Add a sprite region to an atlas (for programmatic creation)
  void SpriteAtlasAddRegion(SpriteAtlasHandle atlas, const char* name, int x, int y, int width, int height);
  
  // Get sprite dimensions from an atlas region
  int SpriteAtlasGetWidth(SpriteAtlasHandle atlas, const char* region_name);
  int SpriteAtlasGetHeight(SpriteAtlasHandle atlas, const char* region_name);
  
  // Check if a region exists in the atlas
  bool SpriteAtlasHasRegion(SpriteAtlasHandle atlas, const char* region_name);
  
  // Draw a sprite from an atlas
  void DrawSpriteFromAtlas(SpriteAtlasHandle atlas, const char* region_name, float x, float y);
  
  // Draw a sprite from an atlas with transform
  void DrawSpriteFromAtlas(SpriteAtlasHandle atlas, const char* region_name, const SpriteTransform &st);
  
  // Release an atlas (cleanup)
  void SpriteAtlasRelease(SpriteAtlasHandle atlas);

} /* MOMOS */

#endif
