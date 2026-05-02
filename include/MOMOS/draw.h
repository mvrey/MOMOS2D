/* Copyright 2017. All rights reserved.			   _____   ________      _____   ________     _________
* Author: Marcos Vazquez <mrkvr84@gmail.com>	  /     \  \_____  \    /     \  \_____  \   /   _____/
*												 /  \ /  \  /   |   \  /  \ /  \  /   |   \  \_____  \
* Drawing commands          					/    Y    \/    |    \/    Y    \/    |    \ /        \
*												\____|____/\_________/\____|____/\_________//_________/
*/

#ifndef MOMOS_DRAW_H
#define MOMOS_DRAW_H 1

namespace MOMOS {

  enum class BlendMode {
    Normal,   // SRC_ALPHA, ONE_MINUS_SRC_ALPHA
    Additive  // SRC_ALPHA, ONE  (for light/glow sprites)
  };

  void DrawSetBlendMode(BlendMode mode);

  // Filled rectangle at (x,y) with given width/height. Respects current fill color and alpha.
  void DrawRect(float x, float y, float w, float h);

  // Filled circle centered at (cx,cy). Respects current fill color and blend mode.
  void DrawCircle(float cx, float cy, float radius, int segments = 32);

  // Call this before start sending drawing commands
  void DrawBegin();
  // Call this after all drawing is done
  void DrawEnd();

  void DrawSetStrokeColor(
      unsigned char R,
      unsigned char G, 
      unsigned char B,
      unsigned char Alpha = 255);

  void DrawSetFillColor(
      unsigned char R,
      unsigned char G,
      unsigned char B,
      unsigned char Alpha = 255);

  void DrawClear(
      unsigned char R,
      unsigned char G,
      unsigned char B,
      unsigned char Alpha = 255);

  void DrawLine(float x1,float y1,float x2,float y2);

  // Draws a continuous line (path)
  void DrawPath(float *pairs_of_points, int num_points);

  void DrawSolidPath(float *pairs_of_points, int num_points, bool stroke = true);

  void DrawSetTextFont(const char *name);
  void DrawSetTextSize(float size);
  void DrawSetTextBlur(float blur_radius);
  void DrawText(float x, float y, const char *text);

  // ============================================================================
  // Convenience rect helpers (inline wrappers — no .lib changes required).
  //
  // These collapse the very common 5-point closed-path-rectangle pattern that
  // appears across the project's UI code:
  //
  //     float pts[10] = { x,y, x+w,y, x+w,y+h, x,y+h, x,y };
  //     DrawSetFillColor(r,g,b,a);
  //     DrawSolidPath(pts, 5);
  //
  // into a single call. The implementations live in the header (inline) so
  // existing pre-built Momos2D.lib does not need to be rebuilt.
  // ============================================================================

  // Filled rectangle, no stroke. Sets the current fill color as a side effect.
  inline void DrawSolidRect(float x, float y, float w, float h,
                            unsigned char r, unsigned char g, unsigned char b,
                            unsigned char a = 255) {
    float pts[10] = {
      x,     y,
      x + w, y,
      x + w, y + h,
      x,     y + h,
      x,     y,
    };
    DrawSetFillColor(r, g, b, a);
    DrawSolidPath(pts, 5, /*stroke=*/false);
  }

  // Filled rectangle + outline using the current stroke color. Sets the
  // current fill color as a side effect; stroke color must be set by the caller
  // beforehand if a non-default outline color is desired.
  inline void DrawSolidRectStroked(float x, float y, float w, float h,
                                   unsigned char r, unsigned char g, unsigned char b,
                                   unsigned char a = 255) {
    float pts[10] = {
      x,     y,
      x + w, y,
      x + w, y + h,
      x,     y + h,
      x,     y,
    };
    DrawSetFillColor(r, g, b, a);
    DrawSolidPath(pts, 5, /*stroke=*/true);
  }

  // Outline-only rectangle (no fill). Sets the current stroke color as a side effect.
  inline void DrawRectOutline(float x, float y, float w, float h,
                              unsigned char r, unsigned char g, unsigned char b,
                              unsigned char a = 255) {
    float pts[10] = {
      x,     y,
      x + w, y,
      x + w, y + h,
      x,     y + h,
      x,     y,
    };
    DrawSetStrokeColor(r, g, b, a);
    DrawPath(pts, 5);
  }

} /* MOMOS */

#endif