#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "allocator.h"
#include "model.h"
#include "texture.h"

#define SOMETYPE_MONO_REGULAR std::string("SometypeMono-Regular.ttf")
#define ATARI_CLASIC_GRY_3 std::string("AtariClassic-gry3.ttf")

struct Glyph {
  glm::vec2 offset;
  glm::vec2 advance;
  glm::ivec2 textureCoords;
  glm::ivec2 size;
};

class Font {
 public:
  Font(
      Device* device,
      MemoryAllocator* transientAllocator,
      const std::string& fontName,
      const unsigned int fontSize);

  void getGlyphCodepoint();
  std::vector<Model::Vertex> getVertices(
      const std::string& text, const glm::vec2& position, VkExtent2D extent);
  Texture* getTexture() { return &texture; }
  void prepare() { texture.prepare(); }
  glm::ivec2 getGlyphSize(const char character) { return glyphs[character].size; }
  glm::vec2 getGlyphOffset(const char character) { return glyphs[character].offset; }
  glm::vec2 getGlyphAdvance(const char character) { return glyphs[character].advance; }
  glm::ivec2 getTextureCoords(const char character) { return glyphs[character].textureCoords; }
  int getTextureWidth() { return textureWidth; }

 private:
  unsigned int fontSize;
  const int textureWidth = 512;

  Texture texture;

  // freetype change
  int fontHeight;
  Glyph glyphs[127];

  int min(int a, int b) { return (a < b) ? a : b; }

  int max(int a, int b) { return (a > b) ? a : b; }
};